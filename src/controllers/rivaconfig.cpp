#include "rivaconfig.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

// ============================================================================
// SINGLETON ACCESS
// ============================================================================

RivaConfig& RivaConfig::instance() {
    static RivaConfig s_instance;
    return s_instance;
}

// ============================================================================
// LOAD/UNLOAD
// ============================================================================

bool RivaConfig::load(const QString& configPath) {
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open RIVA config file:" << configPath;
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject()) {
        qCritical() << "Invalid RIVA config format";
        return false;
    }

    QJsonObject root = doc.object();
    if (!root.contains("riva")) {
        qWarning() << "No 'riva' section in config - voice control disabled";
        return false;
    }

    instance().loadFromJson(root["riva"].toObject());
    instance().m_loaded = true;

    qInfo() << "✅ [RIVA CONFIG] Loaded successfully";
    qInfo() << "   Mode:" << modeString();
    qInfo() << "   Server:" << server().serverUrl;
    qInfo() << "   Voice Control:" << (enableVoiceControl() ? "ENABLED" : "DISABLED");
    qInfo() << "   Biometric Auth:" << (enableBiometricAuth() ? "ENABLED" : "DISABLED");

    return true;
}

void RivaConfig::unload() {
    instance().m_loaded = false;
}

// ============================================================================
// PARSE JSON
// ============================================================================

void RivaConfig::loadFromJson(const QJsonObject& json) {
    // Parse mode
    QString modeStr = json["mode"].toString("dev").toLower();
    m_mode = (modeStr == "prod" || modeStr == "production")
             ? Mode::Production
             : Mode::Development;

    // Parse global settings
    m_enableVoiceControl = json["enableVoiceControl"].toBool(false);
    m_enableBiometricAuth = json["enableBiometricAuth"].toBool(false);
    m_wakeWord = json["wakeWord"].toString("hi harres");

    // Parse dev server config
    if (json.contains("dev")) {
        QJsonObject dev = json["dev"].toObject();
        m_devServer.serverUrl = dev["serverUrl"].toString("grpc.nvcf.nvidia.com:443");
        m_devServer.useSSL = dev["useSSL"].toBool(true);
        m_devServer.apiKey = dev["apiKey"].toString();
        m_devServer.asrFunctionId = dev["asrFunctionId"].toString();
        m_devServer.ttsFunctionId = dev["ttsFunctionId"].toString();
        m_devServer.timeoutMs = dev["timeout"].toInt(30000);
    }

    // Parse prod server config
    if (json.contains("prod")) {
        QJsonObject prod = json["prod"].toObject();
        m_prodServer.serverUrl = prod["serverUrl"].toString("localhost:50051");
        m_prodServer.useSSL = prod["useSSL"].toBool(false);
        m_prodServer.apiKey = prod["apiKey"].toString();
        m_prodServer.asrFunctionId = prod["asrFunctionId"].toString();
        m_prodServer.ttsFunctionId = prod["ttsFunctionId"].toString();
        m_prodServer.timeoutMs = prod["timeout"].toInt(5000);
    }

    // Parse ASR config
    if (json.contains("asr")) {
        QJsonObject asr = json["asr"].toObject();
        m_asr.sampleRate = asr["sampleRate"].toInt(16000);
        m_asr.channels = asr["channels"].toInt(2);
        m_asr.encoding = asr["encoding"].toString("LINEAR_PCM");
        m_asr.languageCode = asr["languageCode"].toString("en-US");
        m_asr.interimResults = asr["interimResults"].toBool(true);
        m_asr.enableAutoPunctuation = asr["enableAutoPunctuation"].toBool(true);
        m_asr.profanityFilter = asr["profanityFilter"].toBool(false);
        m_asr.chunkDurationMs = asr["chunkDurationMs"].toInt(100);
        m_asr.audioDevice = asr["audioDevice"].toString("default");
    }

    // Parse TTS config
    if (json.contains("tts")) {
        QJsonObject tts = json["tts"].toObject();
        m_tts.sampleRate = tts["sampleRate"].toInt(22050);
        m_tts.encoding = tts["encoding"].toString("LINEAR_PCM");
        m_tts.languageCode = tts["languageCode"].toString("en-US");
        m_tts.voiceName = tts["voiceName"].toString();
        m_tts.speakingRate = tts["speakingRate"].toDouble(1.0);
        m_tts.pitch = tts["pitch"].toDouble(0.0);
        m_tts.volumeGainDb = tts["volumeGainDb"].toDouble(0.0);
        m_tts.saveAudioFiles = tts["saveAudioFiles"].toBool(false);
        m_tts.outputPath = tts["outputPath"].toString("./data/tts_audio/");
    }

    // Parse NLU config
    if (json.contains("nlu")) {
        QJsonObject nlu = json["nlu"].toObject();
        m_nlu.confidenceThreshold = nlu["confidenceThreshold"].toDouble(0.5);
        m_nlu.enableSlotExtraction = nlu["enableSlotExtraction"].toBool(true);
        m_nlu.enableContextTracking = nlu["enableContextTracking"].toBool(false);
    }

    // Parse biometric config
    if (json.contains("biometric")) {
        QJsonObject bio = json["biometric"].toObject();
        m_biometric.voiceprintPath = bio["voiceprintPath"].toString("./config/voiceprints.json");
        m_biometric.similarityThreshold = bio["similarityThreshold"].toDouble(0.75);
        m_biometric.enrollmentSamplesRequired = bio["enrollmentSamplesRequired"].toInt(5);
        m_biometric.enableContinuousVerification = bio["enableContinuousVerification"].toBool(true);
        m_biometric.verificationIntervalSec = bio["verificationIntervalSec"].toInt(30);
        m_biometric.sessionTimeoutMin = bio["sessionTimeoutMin"].toInt(10);
    }

    // Parse commands config
    if (json.contains("commands")) {
        QJsonObject cmd = json["commands"].toObject();
        m_commands.enableConfirmation = cmd["enableConfirmation"].toBool(true);
        m_commands.confirmationTimeout = cmd["confirmationTimeout"].toInt(10);
        m_commands.feedbackVolume = cmd["feedbackVolume"].toInt(80);
        m_commands.muteMicDuringTTS = cmd["muteMicDuringTTS"].toBool(true);
        m_commands.logAllCommands = cmd["logAllCommands"].toBool(true);
    }
}

// ============================================================================
// ACCESSORS
// ============================================================================

RivaConfig::Mode RivaConfig::mode() {
    if (!instance().m_loaded) {
        qWarning() << "RivaConfig not loaded! Call RivaConfig::load() first";
        return Mode::Development;  // Safe default
    }
    return instance().m_mode;
}

QString RivaConfig::modeString() {
    return (mode() == Mode::Production) ? "PRODUCTION" : "DEVELOPMENT";
}

bool RivaConfig::enableVoiceControl() {
    return instance().m_loaded && instance().m_enableVoiceControl;
}

bool RivaConfig::enableBiometricAuth() {
    return instance().m_loaded && instance().m_enableBiometricAuth;
}

QString RivaConfig::wakeWord() {
    return instance().m_loaded ? instance().m_wakeWord : "hi harres";
}

const RivaConfig::ServerConfig& RivaConfig::server() {
    return (mode() == Mode::Production) ? prodServer() : devServer();
}

const RivaConfig::ServerConfig& RivaConfig::devServer() {
    return instance().m_devServer;
}

const RivaConfig::ServerConfig& RivaConfig::prodServer() {
    return instance().m_prodServer;
}

const RivaConfig::AsrConfig& RivaConfig::asr() {
    return instance().m_asr;
}

const RivaConfig::TtsConfig& RivaConfig::tts() {
    return instance().m_tts;
}

const RivaConfig::NluConfig& RivaConfig::nlu() {
    return instance().m_nlu;
}

const RivaConfig::BiometricConfig& RivaConfig::biometric() {
    return instance().m_biometric;
}

const RivaConfig::CommandsConfig& RivaConfig::commands() {
    return instance().m_commands;
}

// ============================================================================
// UTILITY METHODS
// ============================================================================

QString RivaConfig::getConnectionString() {
    const auto& srv = server();
    return QString::fromStdString(srv.serverUrl.toStdString());
}

bool RivaConfig::requiresAuthentication() {
    return !server().apiKey.isEmpty();
}
