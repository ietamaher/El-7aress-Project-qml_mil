#include "rivaasrdevice.h"
#include "controllers/rivaconfig.h"

// Include RIVA proto headers
// These will be generated from .proto files during build
#include "riva/proto/riva_asr.grpc.pb.h"
#include "riva/proto/riva_asr.pb.h"

#include <QDebug>
#include <algorithm>

// ============================================================================
// RIVAASRDEVICE - MAIN CLASS
// ============================================================================

RivaAsrDevice::RivaAsrDevice(
    std::shared_ptr<grpc::Channel> channel,
    const QString& apiKey,
    QObject* parent
) : QObject(parent),
    m_channel(channel),
    m_apiKey(apiKey)
{
    // Create gRPC stub
    m_stub = nvidia::riva::asr::RivaSpeechRecognition::NewStub(m_channel);

    // Load configuration from RivaConfig
    const auto& asrConfig = RivaConfig::asr();
    m_sampleRate = asrConfig.sampleRate;
    m_channels = asrConfig.channels;
    m_chunkDurationMs = asrConfig.chunkDurationMs;
    m_audioDevice = asrConfig.audioDevice;
    m_wakeWord = RivaConfig::wakeWord();

    const auto& serverConfig = RivaConfig::server();
    m_functionId = serverConfig.asrFunctionId;

    qInfo() << "🎤 [ASR] RivaAsrDevice created";
    qInfo() << "   Audio device:" << m_audioDevice;
    qInfo() << "   Sample rate:" << m_sampleRate << "Hz";
    qInfo() << "   Channels:" << m_channels;
    qInfo() << "   Wake word:" << m_wakeWord;
}

RivaAsrDevice::~RivaAsrDevice()
{
    stop();
}

// ============================================================================
// DEVICE LIFECYCLE
// ============================================================================

bool RivaAsrDevice::start()
{
    if (m_running) {
        qWarning() << "[ASR] Already running";
        return false;
    }

    qInfo() << "🎤 [ASR] Starting device...";

    // Open ALSA audio device
    if (!openAudioDevice()) {
        emit error("Failed to open audio device");
        return false;
    }

    // Initialize gRPC stream
    if (!initializeGrpcStream()) {
        closeAudioDevice();
        emit error("Failed to initialize gRPC stream");
        return false;
    }

    // Send initial configuration
    if (!sendConfigurationRequest()) {
        closeGrpcStream();
        closeAudioDevice();
        emit error("Failed to send configuration");
        return false;
    }

    // Start threads
    m_audioThread = new AudioCaptureThread(this);
    m_responseThread = new ResponseProcessingThread(this);

    connect(m_audioThread, &QThread::finished,
            this, &RivaAsrDevice::onAudioThreadFinished);
    connect(m_responseThread, &QThread::finished,
            this, &RivaAsrDevice::onResponseThreadFinished);

    m_running = true;

    m_audioThread->start();
    m_responseThread->start();

    qInfo() << "✅ [ASR] Device started successfully";
    emit started();

    return true;
}

void RivaAsrDevice::stop()
{
    if (!m_running) {
        return;
    }

    qInfo() << "🎤 [ASR] Stopping device...";
    m_running = false;

    // Stop threads
    if (m_audioThread) {
        m_audioThread->stopCapture();
        m_audioThread->wait(5000);
        delete m_audioThread;
        m_audioThread = nullptr;
    }

    if (m_responseThread) {
        m_responseThread->stopProcessing();
        m_responseThread->wait(5000);
        delete m_responseThread;
        m_responseThread = nullptr;
    }

    // Close connections
    closeGrpcStream();
    closeAudioDevice();

    qInfo() << "✅ [ASR] Device stopped";
    emit stopped();
}

bool RivaAsrDevice::isRunning() const
{
    return m_running;
}

// ============================================================================
// GRPC STREAMING
// ============================================================================

bool RivaAsrDevice::initializeGrpcStream()
{
    qInfo() << "  [ASR] Initializing gRPC stream...";

    m_context = std::make_unique<grpc::ClientContext>();

    // Add authentication metadata if using cloud API
    if (!m_apiKey.isEmpty()) {
        m_context->AddMetadata("authorization", "Bearer " + m_apiKey.toStdString());
    }

    // Add function ID if using cloud API
    if (!m_functionId.isEmpty()) {
        m_context->AddMetadata("function-id", m_functionId.toStdString());
    }

    // Create bidirectional stream
    m_stream = m_stub->StreamingRecognize(m_context.get());

    if (!m_stream) {
        qCritical() << "  ✗ Failed to create gRPC stream";
        return false;
    }

    qInfo() << "  ✓ gRPC stream initialized";
    return true;
}

void RivaAsrDevice::closeGrpcStream()
{
    if (m_stream) {
        m_stream->WritesDone();
        grpc::Status status = m_stream->Finish();
        if (!status.ok()) {
            qWarning() << "  [ASR] Stream closed with error:"
                      << QString::fromStdString(status.error_message());
        }
        m_stream.reset();
    }
    m_context.reset();
}

bool RivaAsrDevice::sendConfigurationRequest()
{
    qInfo() << "  [ASR] Sending configuration...";

    const auto& asrConfig = RivaConfig::asr();

    nvidia::riva::asr::StreamingRecognizeRequest request;
    auto* streaming_config = request.mutable_streaming_config();
    streaming_config->set_interim_results(asrConfig.interimResults);

    auto* config = streaming_config->mutable_config();
    config->set_sample_rate_hertz(asrConfig.sampleRate);
    config->set_language_code(asrConfig.languageCode.toStdString());

    // Set encoding
    if (asrConfig.encoding == "LINEAR_PCM") {
        config->set_encoding(nvidia::riva::LINEAR_PCM);
    }

    config->set_max_alternatives(1);
    config->set_profanity_filter(asrConfig.profanityFilter);
    config->set_audio_channel_count(1);  // Always mono after conversion
    config->set_enable_word_time_offsets(false);
    config->set_enable_automatic_punctuation(asrConfig.enableAutoPunctuation);

    if (!m_stream->Write(request)) {
        qCritical() << "  ✗ Failed to send configuration";
        return false;
    }

    qInfo() << "  ✓ Configuration sent";
    return true;
}

// ============================================================================
// ALSA AUDIO CAPTURE
// ============================================================================

bool RivaAsrDevice::openAudioDevice()
{
    qInfo() << "  [ASR] Opening audio device:" << m_audioDevice;

    int err;

    if ((err = snd_pcm_open(&m_alsaHandle, m_audioDevice.toStdString().c_str(),
                            SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        qCritical() << "  ✗ Cannot open audio device:" << snd_strerror(err);
        return false;
    }

    // Configure hardware parameters
    snd_pcm_hw_params_t* hw_params;
    snd_pcm_hw_params_alloca(&hw_params);
    snd_pcm_hw_params_any(m_alsaHandle, hw_params);
    snd_pcm_hw_params_set_access(m_alsaHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(m_alsaHandle, hw_params, SND_PCM_FORMAT_S16_LE);

    unsigned int rate = m_sampleRate;
    snd_pcm_hw_params_set_rate_near(m_alsaHandle, hw_params, &rate, 0);
    snd_pcm_hw_params_set_channels(m_alsaHandle, hw_params, m_channels);

    // Set buffer time (100ms typical)
    unsigned int buffer_time = 100000;  // microseconds
    snd_pcm_hw_params_set_buffer_time_near(m_alsaHandle, hw_params, &buffer_time, 0);

    if ((err = snd_pcm_hw_params(m_alsaHandle, hw_params)) < 0) {
        qCritical() << "  ✗ Cannot set hardware parameters:" << snd_strerror(err);
        snd_pcm_close(m_alsaHandle);
        m_alsaHandle = nullptr;
        return false;
    }

    if ((err = snd_pcm_prepare(m_alsaHandle)) < 0) {
        qCritical() << "  ✗ Cannot prepare audio interface:" << snd_strerror(err);
        snd_pcm_close(m_alsaHandle);
        m_alsaHandle = nullptr;
        return false;
    }

    qInfo() << "  ✓ Audio device opened successfully";
    return true;
}

void RivaAsrDevice::closeAudioDevice()
{
    if (m_alsaHandle) {
        snd_pcm_close(m_alsaHandle);
        m_alsaHandle = nullptr;
        qInfo() << "  [ASR] Audio device closed";
    }
}

bool RivaAsrDevice::captureAudioChunk(std::vector<char>& buffer)
{
    if (!m_alsaHandle) {
        return false;
    }

    const size_t frames_per_chunk = m_sampleRate * m_chunkDurationMs / 1000;
    const size_t stereo_chunk_size = frames_per_chunk * sizeof(int16_t) * m_channels;

    std::vector<char> stereo_chunk(stereo_chunk_size);

    int frames_read = snd_pcm_readi(m_alsaHandle, stereo_chunk.data(), frames_per_chunk);

    if (frames_read > 0) {
        // Convert stereo to mono
        if (m_channels == 2) {
            buffer.resize(frames_read * sizeof(int16_t));
            int16_t* stereo_samples = (int16_t*)stereo_chunk.data();
            int16_t* mono_samples = (int16_t*)buffer.data();

            for (int i = 0; i < frames_read; ++i) {
                mono_samples[i] = stereo_samples[i * 2];  // Take left channel
            }
        } else {
            buffer = stereo_chunk;
        }

        return true;
    }
    else if (frames_read == -EPIPE) {
        // Buffer overrun - recover
        qWarning() << "  [ASR] Buffer overrun, recovering...";
        int err = snd_pcm_recover(m_alsaHandle, frames_read, 1);
        if (err < 0) {
            qCritical() << "  ✗ Recovery failed:" << snd_strerror(err);
            return false;
        }
        snd_pcm_prepare(m_alsaHandle);
        return false;  // Skip this chunk
    }
    else if (frames_read < 0) {
        qWarning() << "  [ASR] Read error:" << snd_strerror(frames_read);
        return false;
    }

    return false;
}

// ============================================================================
// WAKE WORD DETECTION
// ============================================================================

bool RivaAsrDevice::detectWakeWord(const QString& transcript)
{
    QString lowerTranscript = transcript.toLower();
    QString lowerWakeWord = m_wakeWord.toLower();

    if (lowerTranscript.contains(lowerWakeWord)) {
        qInfo() << "✅ [ASR] Wake word detected:" << m_wakeWord;
        emit wakeWordDetected(m_wakeWord);
        return true;
    }

    return false;
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void RivaAsrDevice::setAudioDevice(const QString& deviceName)
{
    if (!m_running) {
        m_audioDevice = deviceName;
    } else {
        qWarning() << "[ASR] Cannot change audio device while running";
    }
}

void RivaAsrDevice::setSampleRate(int sampleRate)
{
    if (!m_running) {
        m_sampleRate = sampleRate;
    } else {
        qWarning() << "[ASR] Cannot change sample rate while running";
    }
}

void RivaAsrDevice::setChannels(int channels)
{
    if (!m_running) {
        m_channels = channels;
    } else {
        qWarning() << "[ASR] Cannot change channels while running";
    }
}

// ============================================================================
// THREAD CALLBACKS
// ============================================================================

void RivaAsrDevice::onAudioThreadFinished()
{
    qInfo() << "  [ASR] Audio capture thread finished";
}

void RivaAsrDevice::onResponseThreadFinished()
{
    qInfo() << "  [ASR] Response processing thread finished";
}

// ============================================================================
// AUDIO CAPTURE THREAD
// ============================================================================

RivaAsrDevice::AudioCaptureThread::AudioCaptureThread(RivaAsrDevice* device)
    : m_device(device)
{
}

void RivaAsrDevice::AudioCaptureThread::run()
{
    qInfo() << "  [ASR] Audio capture thread started";

    std::vector<char> audioBuffer;
    nvidia::riva::asr::StreamingRecognizeRequest request;

    while (!m_shouldStop && m_device->m_running) {
        // Capture audio chunk
        if (m_device->captureAudioChunk(audioBuffer)) {
            // Send to RIVA server
            request.set_audio_content(audioBuffer.data(), audioBuffer.size());

            if (!m_device->m_stream->Write(request)) {
                qCritical() << "  ✗ [ASR] Failed to write audio data";
                break;
            }
        }
    }

    // Signal end of audio stream
    m_device->m_stream->WritesDone();

    qInfo() << "  [ASR] Audio capture thread stopping";
}

void RivaAsrDevice::AudioCaptureThread::stopCapture()
{
    m_shouldStop = true;
}

// ============================================================================
// RESPONSE PROCESSING THREAD
// ============================================================================

RivaAsrDevice::ResponseProcessingThread::ResponseProcessingThread(RivaAsrDevice* device)
    : m_device(device)
{
}

void RivaAsrDevice::ResponseProcessingThread::run()
{
    qInfo() << "  [ASR] Response processing thread started";

    nvidia::riva::asr::StreamingRecognizeResponse response;

    while (!m_shouldStop && m_device->m_stream->Read(&response)) {
        for (int r = 0; r < response.results_size(); ++r) {
            const auto& result = response.results(r);

            if (result.alternatives_size() > 0) {
                const auto& alternative = result.alternatives(0);
                QString transcript = QString::fromStdString(alternative.transcript());
                float confidence = alternative.confidence();
                bool isFinal = result.is_final();

                if (!transcript.isEmpty()) {
                    // Emit transcript
                    emit m_device->transcriptReceived(transcript, isFinal, confidence);

                    // Check for wake word
                    if (isFinal) {
                        m_device->detectWakeWord(transcript);
                    }
                }
            }
        }
    }

    qInfo() << "  [ASR] Response processing thread stopping";
}

void RivaAsrDevice::ResponseProcessingThread::stopProcessing()
{
    m_shouldStop = true;
}
