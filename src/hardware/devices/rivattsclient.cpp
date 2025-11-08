#include "rivattsclient.h"
#include "controllers/rivaconfig.h"

// Include RIVA proto headers
#include "riva/proto/riva_tts.grpc.pb.h"
#include "riva/proto/riva_tts.pb.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <fstream>
#include <cstdlib>

RivaTTSClient::RivaTTSClient(
    std::shared_ptr<grpc::Channel> channel,
    const QString& apiKey,
    QObject* parent
) : QObject(parent),
    m_channel(channel),
    m_apiKey(apiKey)
{
    // Create gRPC stub
    m_stub = nvidia::riva::tts::RivaSpeechSynthesis::NewStub(m_channel);

    // Load configuration from RivaConfig
    const auto& ttsConfig = RivaConfig::tts();
    m_sampleRate = ttsConfig.sampleRate;
    m_voiceName = ttsConfig.voiceName;
    m_speakingRate = ttsConfig.speakingRate;
    m_pitch = ttsConfig.pitch;
    m_volumeGain = ttsConfig.volumeGainDb;
    m_saveAudioFiles = ttsConfig.saveAudioFiles;
    m_outputPath = ttsConfig.outputPath;

    const auto& serverConfig = RivaConfig::server();
    m_functionId = serverConfig.ttsFunctionId;

    // Create output directory if it doesn't exist
    if (m_saveAudioFiles) {
        QDir dir;
        dir.mkpath(m_outputPath);
    }

    qInfo() << "🔊 [TTS] RivaTTSClient created";
    qInfo() << "   Sample rate:" << m_sampleRate << "Hz";
    qInfo() << "   Speaking rate:" << m_speakingRate;
    qInfo() << "   Output path:" << m_outputPath;
}

RivaTTSClient::~RivaTTSClient()
{
}

bool RivaTTSClient::speak(const QString& text)
{
    qInfo() << "🔊 [TTS] Speaking:" << text;

    emit speechStarted(text);

    // Synthesize speech
    grpc::ClientContext context;

    // Add authentication metadata if using cloud API
    if (!m_apiKey.isEmpty()) {
        context.AddMetadata("authorization", "Bearer " + m_apiKey.toStdString());
    }

    // Add function ID if using cloud API
    if (!m_functionId.isEmpty()) {
        context.AddMetadata("function-id", m_functionId.toStdString());
    }

    // Create request
    nvidia::riva::tts::SynthesizeSpeechRequest request;
    request.set_text(text.toStdString());
    request.set_language_code(RivaConfig::tts().languageCode.toStdString());
    request.set_encoding(nvidia::riva::LINEAR_PCM);
    request.set_sample_rate_hz(m_sampleRate);

    // Optional voice name (may not be supported in all RIVA deployments)
    if (!m_voiceName.isEmpty()) {
        request.set_voice_name(m_voiceName.toStdString());
    }

    // Send request
    nvidia::riva::tts::SynthesizeSpeechResponse response;
    grpc::Status status = m_stub->Synthesize(&context, request, &response);

    if (!status.ok()) {
        QString errorMsg = QString::fromStdString(status.error_message());
        qCritical() << "✗ [TTS] Synthesis failed:" << errorMsg;
        emit error(errorMsg);
        return false;
    }

    // Save audio to file
    QString filename;
    if (m_saveAudioFiles) {
        filename = QString("%1/tts_feedback_%2.wav")
                      .arg(m_outputPath)
                      .arg(m_feedbackCounter++);
        saveAudioToWav(response.audio(), filename, m_sampleRate);
        qInfo() << "   💾 Saved to:" << filename;
    } else {
        // Create temporary file
        filename = QString("/tmp/tts_temp_%1.wav").arg(QDateTime::currentMSecsSinceEpoch());
        saveAudioToWav(response.audio(), filename, m_sampleRate);
    }

    // Play audio
    playAudio(filename);

    // Clean up temporary file if not saving
    if (!m_saveAudioFiles) {
        QFile::remove(filename);
    }

    emit speechFinished();

    return true;
}

bool RivaTTSClient::synthesizeToFile(const QString& text, const QString& filename)
{
    qInfo() << "🔊 [TTS] Synthesizing to file:" << filename;

    grpc::ClientContext context;

    if (!m_apiKey.isEmpty()) {
        context.AddMetadata("authorization", "Bearer " + m_apiKey.toStdString());
    }

    if (!m_functionId.isEmpty()) {
        context.AddMetadata("function-id", m_functionId.toStdString());
    }

    nvidia::riva::tts::SynthesizeSpeechRequest request;
    request.set_text(text.toStdString());
    request.set_language_code(RivaConfig::tts().languageCode.toStdString());
    request.set_encoding(nvidia::riva::LINEAR_PCM);
    request.set_sample_rate_hz(m_sampleRate);

    if (!m_voiceName.isEmpty()) {
        request.set_voice_name(m_voiceName.toStdString());
    }

    nvidia::riva::tts::SynthesizeSpeechResponse response;
    grpc::Status status = m_stub->Synthesize(&context, request, &response);

    if (!status.ok()) {
        QString errorMsg = QString::fromStdString(status.error_message());
        qCritical() << "✗ [TTS] Synthesis failed:" << errorMsg;
        emit error(errorMsg);
        return false;
    }

    saveAudioToWav(response.audio(), filename, m_sampleRate);
    qInfo() << "✅ [TTS] Saved to:" << filename;

    return true;
}

void RivaTTSClient::saveAudioToWav(
    const std::string& audio_data,
    const QString& filename,
    int sample_rate
) {
    std::ofstream out(filename.toStdString(), std::ios::binary);

    uint32_t data_size = audio_data.size();
    uint32_t file_size = data_size + 36;
    uint16_t channels = 1;
    uint16_t bits_per_sample = 16;
    uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
    uint16_t block_align = channels * bits_per_sample / 8;

    // Write WAV header
    out.write("RIFF", 4);
    out.write(reinterpret_cast<const char*>(&file_size), 4);
    out.write("WAVE", 4);
    out.write("fmt ", 4);

    uint32_t fmt_size = 16;
    out.write(reinterpret_cast<const char*>(&fmt_size), 4);

    uint16_t audio_format = 1;  // PCM
    out.write(reinterpret_cast<const char*>(&audio_format), 2);
    out.write(reinterpret_cast<const char*>(&channels), 2);
    out.write(reinterpret_cast<const char*>(&sample_rate), 4);
    out.write(reinterpret_cast<const char*>(&byte_rate), 4);
    out.write(reinterpret_cast<const char*>(&block_align), 2);
    out.write(reinterpret_cast<const char*>(&bits_per_sample), 2);

    out.write("data", 4);
    out.write(reinterpret_cast<const char*>(&data_size), 4);
    out.write(audio_data.data(), data_size);
}

void RivaTTSClient::playAudio(const QString& filename)
{
    // Play audio using aplay (Linux ALSA utility)
    QString command = QString("aplay -q %1 2>/dev/null &").arg(filename);
    int result = system(command.toStdString().c_str());

    if (result != 0) {
        qWarning() << "  ⚠️ [TTS] Failed to play audio (aplay not available?)";
    }

    // Simulate speaking time to prevent mic pickup
    const auto& cmdConfig = RivaConfig::commands();
    if (cmdConfig.muteMicDuringTTS) {
        QThread::msleep(1500);  // Wait for audio to finish
    }
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void RivaTTSClient::setVoiceName(const QString& voiceName)
{
    m_voiceName = voiceName;
}

void RivaTTSClient::setSpeakingRate(float rate)
{
    m_speakingRate = qBound(0.5f, rate, 2.0f);
}

void RivaTTSClient::setPitch(float pitch)
{
    m_pitch = qBound(-20.0f, pitch, 20.0f);
}

void RivaTTSClient::setVolumeGain(float gainDb)
{
    m_volumeGain = gainDb;
}
