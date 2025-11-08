/**
 * @file systemcontroller_riva_integration_example.cpp
 * @brief Example integration of RIVA Voice Control with configuration-based dev/prod mode
 *
 * This example shows how to integrate the RIVA voice system into SystemController
 * using the RivaConfig class for automatic dev/prod mode switching.
 *
 * USAGE:
 * 1. Development (laptop testing with cloud API):
 *    - Set "mode": "dev" in config/devices.json
 *    - System uses NVIDIA Cloud API automatically
 *
 * 2. Production (Jetson AGX Orin with local RIVA):
 *    - Set "mode": "prod" in config/devices.json
 *    - System uses local RIVA server automatically
 */

#include "systemcontroller.h"
#include "rivaconfig.h"
#include "controllers/voicecommandcontroller.h"

// Placeholder classes - replace with actual implementations
class RivaAsrDevice {};
class RivaTTSClient {};

void SystemController::createVoiceSystem() {
    qInfo() << "========================================";
    qInfo() << "  Creating Voice Control System";
    qInfo() << "========================================";

    // Check if voice control is enabled
    if (!RivaConfig::enableVoiceControl()) {
        qInfo() << "  ⚠️  Voice control disabled in configuration";
        return;
    }

    qInfo() << "  Mode:" << RivaConfig::modeString();
    qInfo() << "  Server:" << RivaConfig::server().serverUrl;

    // ========================================================================
    // STEP 1: CREATE gRPC CHANNEL (AUTO-CONFIGURED)
    // ========================================================================

    const auto& serverConfig = RivaConfig::server();

    std::shared_ptr<grpc::Channel> channel;

    if (serverConfig.useSSL) {
        // SSL/TLS credentials (cloud or production with SSL)
        grpc::SslCredentialsOptions ssl_opts;
        auto creds = grpc::SslCredentials(ssl_opts);
        grpc::ChannelArguments args;
        channel = grpc::CreateCustomChannel(
            serverConfig.serverUrl.toStdString(),
            creds,
            args
        );
        qInfo() << "  ✓ gRPC channel created with SSL";
    } else {
        // Insecure channel (local development)
        channel = grpc::CreateChannel(
            serverConfig.serverUrl.toStdString(),
            grpc::InsecureChannelCredentials()
        );
        qInfo() << "  ✓ gRPC channel created (insecure)";
    }

    // ========================================================================
    // STEP 2: CREATE TTS CLIENT
    // ========================================================================

    // In dev mode, use API key and function ID
    // In prod mode, these are empty (not needed for local server)
    std::string apiKey = serverConfig.apiKey.toStdString();
    std::string ttsFunctionId = serverConfig.ttsFunctionId.toStdString();

    m_ttsClient = new RivaTTSClient(
        channel,
        QString::fromStdString(apiKey),
        this
    );
    qInfo() << "  ✓ TTS client created";

    // Configure TTS from config
    const auto& ttsConfig = RivaConfig::tts();
    qInfo() << "    Sample rate:" << ttsConfig.sampleRate << "Hz";
    qInfo() << "    Language:" << ttsConfig.languageCode;

    // ========================================================================
    // STEP 3: CREATE ASR DEVICE
    // ========================================================================

    m_rivaAsrDevice = new RivaAsrDevice(
        channel,
        QString::fromStdString(apiKey),
        this
    );
    qInfo() << "  ✓ ASR device created";

    // Configure ASR from config
    const auto& asrConfig = RivaConfig::asr();
    qInfo() << "    Sample rate:" << asrConfig.sampleRate << "Hz";
    qInfo() << "    Channels:" << asrConfig.channels;
    qInfo() << "    Language:" << asrConfig.languageCode;
    qInfo() << "    Audio device:" << asrConfig.audioDevice;

    // ========================================================================
    // STEP 4: CREATE VOICE COMMAND CONTROLLER
    // ========================================================================

    m_voiceCommandCtrl = new VoiceCommandController(this);
    qInfo() << "  ✓ Voice command controller created";

    // Configure command processor
    const auto& cmdConfig = RivaConfig::commands();
    qInfo() << "    Confirmation enabled:" << cmdConfig.enableConfirmation;
    qInfo() << "    Feedback volume:" << cmdConfig.feedbackVolume << "%";

    // ========================================================================
    // STEP 5: CONFIGURE BIOMETRIC AUTHENTICATION (OPTIONAL)
    // ========================================================================

    if (RivaConfig::enableBiometricAuth()) {
        const auto& bioConfig = RivaConfig::biometric();
        qInfo() << "  🔐 Biometric authentication enabled";
        qInfo() << "    Voiceprint database:" << bioConfig.voiceprintPath;
        qInfo() << "    Similarity threshold:" << bioConfig.similarityThreshold;
        qInfo() << "    Continuous verification:" << bioConfig.enableContinuousVerification;

        // TODO: Initialize biometric device
        // m_biometricDevice = new RivaBiometricDevice(channel, this);
        // m_biometricDevice->loadVoiceprints(bioConfig.voiceprintPath);
    } else {
        qInfo() << "  ℹ️  Biometric authentication disabled (dev mode)";
    }

    // ========================================================================
    // STEP 6: DISPLAY MODE-SPECIFIC INFORMATION
    // ========================================================================

    if (RivaConfig::isDevMode()) {
        qInfo() << "";
        qInfo() << "  🧪 DEVELOPMENT MODE";
        qInfo() << "  ================================";
        qInfo() << "  Using NVIDIA Cloud API";
        qInfo() << "  Server:" << serverConfig.serverUrl;
        qInfo() << "  API Key:" << (apiKey.empty() ? "MISSING!" : "Configured");
        qInfo() << "  ASR Function:" << serverConfig.asrFunctionId;
        qInfo() << "  TTS Function:" << serverConfig.ttsFunctionId;
        qInfo() << "  Timeout:" << serverConfig.timeoutMs << "ms";
        qInfo() << "  ⚠️  Requires internet connection";
        qInfo() << "  ⚠️  Higher latency (300-500ms)";
        qInfo() << "";
    } else {
        qInfo() << "";
        qInfo() << "  🚀 PRODUCTION MODE";
        qInfo() << "  ================================";
        qInfo() << "  Using local RIVA server";
        qInfo() << "  Server:" << serverConfig.serverUrl;
        qInfo() << "  Timeout:" << serverConfig.timeoutMs << "ms";
        qInfo() << "  ✅ No internet required";
        qInfo() << "  ✅ Low latency (50-100ms)";
        qInfo() << "  ✅ Secure on-premises deployment";
        qInfo() << "";
    }

    qInfo() << "========================================";
    qInfo() << "  Voice Control System Ready";
    qInfo() << "  Wake word: '" << RivaConfig::wakeWord() << "'";
    qInfo() << "========================================\n";
}

/**
 * @brief Quick reference for switching modes
 *
 * DEVELOPMENT MODE (laptop testing):
 * ----------------------------------
 * Edit config/devices.json:
 * {
 *   "riva": {
 *     "mode": "dev",
 *     "enableVoiceControl": true,
 *     ...
 *   }
 * }
 *
 * PRODUCTION MODE (Jetson deployment):
 * ------------------------------------
 * 1. Start RIVA server on Jetson:
 *    ./riva_start.sh
 *
 * 2. Edit config/devices.json:
 * {
 *   "riva": {
 *     "mode": "prod",
 *     "enableVoiceControl": true,
 *     ...
 *   }
 * }
 *
 * 3. Rebuild and run:
 *    qmake && make -j8
 *    ./rcws_app
 */

// ============================================================================
// EXAMPLE: VOICE SYSTEM STARTUP SEQUENCE
// ============================================================================

void SystemController::startSystem() {
    qInfo() << "=== PHASE 3: System Startup ===";

    // ... existing startup code ...

    // Start voice system (if enabled)
    if (RivaConfig::enableVoiceControl() && m_rivaAsrDevice) {
        qInfo() << "  Starting voice recognition system...";

        // Start ASR device
        m_rivaAsrDevice->start();

        // Start voice command controller
        if (m_voiceCommandCtrl) {
            m_voiceCommandCtrl->startVoiceSystem();
        }

        qInfo() << "  ✓ Voice system started";
        qInfo() << "  📣 Say '" << RivaConfig::wakeWord() << "' to activate";
    }

    qInfo() << "=== PHASE 3 COMPLETE - SYSTEM RUNNING ===\n";
}

// ============================================================================
// EXAMPLE: RUNTIME MODE CHECKING
// ============================================================================

void someFunction() {
    // Check if we're in development or production
    if (RivaConfig::isDevMode()) {
        qDebug() << "Running in DEVELOPMENT mode - using cloud API";
        // Maybe show a warning or enable extra debugging
    }

    if (RivaConfig::isProdMode()) {
        qDebug() << "Running in PRODUCTION mode - using local RIVA";
        // Production-specific optimizations
    }

    // Access configuration
    const auto& asrConf = RivaConfig::asr();
    qDebug() << "ASR sample rate:" << asrConf.sampleRate;

    // Check authentication requirements
    if (RivaConfig::requiresAuthentication()) {
        qDebug() << "API key authentication required";
    }
}
