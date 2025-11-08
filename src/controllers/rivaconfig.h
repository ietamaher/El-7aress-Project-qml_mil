#ifndef RIVACONFIG_H
#define RIVACONFIG_H

#include <QString>
#include <QJsonObject>

/**
 * @brief RIVA Voice AI Configuration
 *
 * Manages RIVA configuration with dev/prod mode support:
 * - Dev mode: NVIDIA Cloud API (for laptop testing)
 * - Prod mode: Local RIVA Server (on Jetson AGX Orin)
 */
class RivaConfig
{
public:
    // Deployment mode
    enum class Mode {
        Development,  // Cloud API (requires internet)
        Production    // Local server (on-premises)
    };

    // Server configuration (mode-specific)
    struct ServerConfig {
        QString serverUrl;          // "grpc.nvcf.nvidia.com:443" or "localhost:50051"
        bool useSSL;                // true for cloud, false for local
        QString apiKey;             // Required for cloud, empty for local
        QString asrFunctionId;      // ASR function ID (cloud only)
        QString ttsFunctionId;      // TTS function ID (cloud only)
        int timeoutMs;              // Connection timeout
    };

    // ASR (Automatic Speech Recognition) settings
    struct AsrConfig {
        int sampleRate;             // 16000 Hz typical
        int channels;               // 1 (mono) or 2 (stereo)
        QString encoding;           // "LINEAR_PCM"
        QString languageCode;       // "en-US", "ar-SA", "fr-FR"
        bool interimResults;        // Enable partial results
        bool enableAutoPunctuation; // Add punctuation automatically
        bool profanityFilter;       // Filter profanity
        int chunkDurationMs;        // Audio chunk size (100ms typical)
        QString audioDevice;        // ALSA device ("default", "plughw:1,0")
    };

    // TTS (Text-to-Speech) settings
    struct TtsConfig {
        int sampleRate;             // 22050 Hz typical
        QString encoding;           // "LINEAR_PCM"
        QString languageCode;       // "en-US", "ar-SA", "fr-FR"
        QString voiceName;          // Optional voice selection
        float speakingRate;         // 0.5 - 2.0 (1.0 = normal)
        float pitch;                // -20.0 to 20.0 (0 = normal)
        float volumeGainDb;         // Volume adjustment in dB
        bool saveAudioFiles;        // Save TTS output to files (debugging)
        QString outputPath;         // Where to save audio files
    };

    // NLU (Natural Language Understanding) settings
    struct NluConfig {
        float confidenceThreshold;  // Minimum confidence (0.0 - 1.0)
        bool enableSlotExtraction;  // Extract intent slots
        bool enableContextTracking; // Track conversation context
    };

    // Voice Biometric Authentication settings
    struct BiometricConfig {
        QString voiceprintPath;          // Path to voiceprint database
        float similarityThreshold;       // Minimum similarity for match (0.0 - 1.0)
        int enrollmentSamplesRequired;   // Number of samples for enrollment
        bool enableContinuousVerification; // Verify throughout session
        int verificationIntervalSec;     // How often to verify (seconds)
        int sessionTimeoutMin;           // Session timeout (minutes)
    };

    // Command execution settings
    struct CommandsConfig {
        bool enableConfirmation;    // Require confirmation for critical commands
        int confirmationTimeout;    // Timeout for confirmation (seconds)
        int feedbackVolume;         // TTS volume (0-100)
        bool muteMicDuringTTS;      // Prevent echo/feedback
        bool logAllCommands;        // Log all voice commands
    };

    // Static methods to access configuration
    static bool load(const QString& configPath);
    static void unload();

    // Get current mode (dev/prod)
    static Mode mode();
    static QString modeString();
    static bool isDevMode() { return mode() == Mode::Development; }
    static bool isProdMode() { return mode() == Mode::Production; }

    // Global settings
    static bool enableVoiceControl();
    static bool enableBiometricAuth();
    static QString wakeWord();

    // Server configuration (auto-selects based on mode)
    static const ServerConfig& server();
    static const ServerConfig& devServer();
    static const ServerConfig& prodServer();

    // Component configurations
    static const AsrConfig& asr();
    static const TtsConfig& tts();
    static const NluConfig& nlu();
    static const BiometricConfig& biometric();
    static const CommandsConfig& commands();

    // Utility methods
    static QString getConnectionString();  // Full gRPC URL
    static bool requiresAuthentication();  // Check if API key needed

private:
    RivaConfig() = default;  // Singleton pattern

    static RivaConfig& instance();
    void loadFromJson(const QJsonObject& json);

    // Configuration data
    Mode m_mode = Mode::Development;
    bool m_enableVoiceControl = false;
    bool m_enableBiometricAuth = false;
    QString m_wakeWord = "hi harres";

    ServerConfig m_devServer;
    ServerConfig m_prodServer;
    AsrConfig m_asr;
    TtsConfig m_tts;
    NluConfig m_nlu;
    BiometricConfig m_biometric;
    CommandsConfig m_commands;

    bool m_loaded = false;
};

#endif // RIVACONFIG_H
