#ifndef RIVATTSCLIENT_H
#define RIVATTSCLIENT_H

#include <QObject>
#include <QString>
#include <memory>
#include <grpcpp/grpcpp.h>

// Forward declarations for gRPC
namespace nvidia {
namespace riva {
namespace tts {
    class RivaSpeechSynthesis;
}
}
}

/**
 * @class RivaTTSClient
 * @brief NVIDIA RIVA TTS (Text-to-Speech) Client
 *
 * Features:
 * - Converts text to speech using RIVA
 * - Saves audio to WAV files
 * - Plays audio through ALSA
 * - Configurable voice, rate, pitch
 */
class RivaTTSClient : public QObject
{
    Q_OBJECT

public:
    explicit RivaTTSClient(
        std::shared_ptr<grpc::Channel> channel,
        const QString& apiKey = "",
        QObject* parent = nullptr
    );
    ~RivaTTSClient();

    /**
     * @brief Synthesize speech from text
     * @param text Text to convert to speech
     * @return true if successful
     */
    bool speak(const QString& text);

    /**
     * @brief Synthesize speech and save to file
     * @param text Text to convert to speech
     * @param filename Output filename
     * @return true if successful
     */
    bool synthesizeToFile(const QString& text, const QString& filename);

    // Configuration
    void setVoiceName(const QString& voiceName);
    void setSpeakingRate(float rate);  // 0.5 - 2.0
    void setPitch(float pitch);        // -20.0 to 20.0
    void setVolumeGain(float gainDb);

signals:
    void speechStarted(const QString& text);
    void speechFinished();
    void error(const QString& errorMessage);

private:
    void saveAudioToWav(const std::string& audio_data, const QString& filename, int sample_rate);
    void playAudio(const QString& filename);

    std::shared_ptr<grpc::Channel> m_channel;
    std::unique_ptr<nvidia::riva::tts::RivaSpeechSynthesis::Stub> m_stub;
    QString m_apiKey;
    QString m_functionId;

    // TTS configuration
    int m_sampleRate = 22050;
    QString m_voiceName;
    float m_speakingRate = 1.0f;
    float m_pitch = 0.0f;
    float m_volumeGain = 0.0f;
    QString m_outputPath;
    bool m_saveAudioFiles = true;

    int m_feedbackCounter = 0;
};

#endif // RIVATTSCLIENT_H
