#ifndef RIVAASRDEVICE_H
#define RIVAASRDEVICE_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <atomic>
#include <memory>
#include <alsa/asoundlib.h>
#include <grpcpp/grpcpp.h>

// Forward declarations for gRPC
namespace nvidia {
namespace riva {
namespace asr {
    class RivaSpeechRecognition;
    class StreamingRecognizeRequest;
    class StreamingRecognizeResponse;
}
}
}

/**
 * @class RivaAsrDevice
 * @brief NVIDIA RIVA ASR (Automatic Speech Recognition) Device
 *
 * Follows MIL-STD hardware architecture pattern:
 * - Device Layer: Manages ASR lifecycle and state
 * - Transport Layer: gRPC streaming + ALSA audio capture
 * - Protocol Layer: RIVA protobuf messages
 *
 * Features:
 * - Continuous audio streaming to RIVA server
 * - Real-time transcript updates
 * - Wake word detection
 * - Thread-safe operation
 * - Auto-reconnection on failure
 * - Configurable via RivaConfig
 */
class RivaAsrDevice : public QObject
{
    Q_OBJECT

public:
    explicit RivaAsrDevice(
        std::shared_ptr<grpc::Channel> channel,
        const QString& apiKey = "",
        QObject* parent = nullptr
    );
    ~RivaAsrDevice();

    // Device lifecycle
    bool start();
    void stop();
    bool isRunning() const;

    // Configuration
    void setAudioDevice(const QString& deviceName);
    void setSampleRate(int sampleRate);
    void setChannels(int channels);

signals:
    // Emitted for every transcript update
    void transcriptReceived(const QString& transcript, bool isFinal, float confidence);

    // Emitted when wake word is detected
    void wakeWordDetected(const QString& wakeWord);

    // Device status
    void started();
    void stopped();
    void error(const QString& errorMessage);

private slots:
    void onAudioThreadFinished();
    void onResponseThreadFinished();

private:
    // Audio capture thread
    class AudioCaptureThread : public QThread {
    public:
        AudioCaptureThread(RivaAsrDevice* device);
        void run() override;
        void stopCapture();

    private:
        RivaAsrDevice* m_device;
        std::atomic<bool> m_shouldStop{false};
    };

    // Response processing thread
    class ResponseProcessingThread : public QThread {
    public:
        ResponseProcessingThread(RivaAsrDevice* device);
        void run() override;
        void stopProcessing();

    private:
        RivaAsrDevice* m_device;
        std::atomic<bool> m_shouldStop{false};
    };

    // gRPC streaming
    using AsrStream = grpc::ClientReaderWriter<
        nvidia::riva::asr::StreamingRecognizeRequest,
        nvidia::riva::asr::StreamingRecognizeResponse
    >;

    bool initializeGrpcStream();
    void closeGrpcStream();
    bool sendConfigurationRequest();

    // ALSA audio capture
    bool openAudioDevice();
    void closeAudioDevice();
    bool captureAudioChunk(std::vector<char>& buffer);

    // Wake word detection
    bool detectWakeWord(const QString& transcript);

    // gRPC components
    std::shared_ptr<grpc::Channel> m_channel;
    std::unique_ptr<nvidia::riva::asr::RivaSpeechRecognition::Stub> m_stub;
    std::unique_ptr<grpc::ClientContext> m_context;
    std::unique_ptr<AsrStream> m_stream;
    QString m_apiKey;
    QString m_functionId;

    // ALSA components
    snd_pcm_t* m_alsaHandle = nullptr;
    QString m_audioDevice = "default";
    int m_sampleRate = 16000;
    int m_channels = 2;
    int m_chunkDurationMs = 100;

    // Threading
    AudioCaptureThread* m_audioThread = nullptr;
    ResponseProcessingThread* m_responseThread = nullptr;

    // State
    std::atomic<bool> m_running{false};
    QMutex m_mutex;
    QString m_wakeWord = "hi harres";

    friend class AudioCaptureThread;
    friend class ResponseProcessingThread;
};

#endif // RIVAASRDEVICE_H
