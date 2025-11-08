# RIVA Voice Control Configuration Guide

## 🎯 Quick Start

### Development Mode (Laptop Testing)
```json
// config/devices.json
{
  "riva": {
    "mode": "dev",              // ← Development mode
    "enableVoiceControl": true,
    ...
  }
}
```

**Features:**
- ✅ Uses NVIDIA Cloud API (no local setup required)
- ✅ Works on any laptop with internet
- ⚠️ Requires internet connection
- ⚠️ Higher latency (300-500ms)
- ⚠️ Sends audio to cloud (not suitable for classified operations)

### Production Mode (Jetson Deployment)
```json
// config/devices.json
{
  "riva": {
    "mode": "prod",             // ← Production mode
    "enableVoiceControl": true,
    ...
  }
}
```

**Features:**
- ✅ Uses local RIVA server on Jetson AGX Orin
- ✅ No internet required
- ✅ Low latency (50-100ms)
- ✅ Secure on-premises deployment
- ✅ Suitable for military operations

---

## 📋 Configuration Structure

### Global Settings

```json
"riva": {
  "mode": "dev",                    // "dev" or "prod"
  "enableVoiceControl": true,       // Master enable/disable
  "enableBiometricAuth": false,     // Voice biometric authentication
  "wakeWord": "hi harres",          // Wake word phrase
  ...
}
```

### Dev Server Configuration

```json
"dev": {
  "serverUrl": "grpc.nvcf.nvidia.com:443",
  "useSSL": true,
  "apiKey": "nvapi-YOUR_API_KEY_HERE",
  "asrFunctionId": "1598d209-5e27-4d3c-8079-4751568b1081",
  "ttsFunctionId": "55cf67bf-600f-4b04-8eac-12ed39537a08",
  "timeout": 30000
}
```

### Prod Server Configuration

```json
"prod": {
  "serverUrl": "localhost:50051",   // Local RIVA server
  "useSSL": false,
  "apiKey": "",                     // Not needed for local
  "asrFunctionId": "",
  "ttsFunctionId": "",
  "timeout": 5000
}
```

### ASR (Speech Recognition) Settings

```json
"asr": {
  "sampleRate": 16000,              // Hz (16000 recommended)
  "channels": 2,                    // 1 (mono) or 2 (stereo)
  "encoding": "LINEAR_PCM",
  "languageCode": "en-US",          // "en-US", "ar-SA", "fr-FR"
  "interimResults": true,           // Show partial results
  "enableAutoPunctuation": true,
  "profanityFilter": false,
  "chunkDurationMs": 100,           // Audio chunk size
  "audioDevice": "default"          // ALSA device name
}
```

### TTS (Text-to-Speech) Settings

```json
"tts": {
  "sampleRate": 22050,
  "encoding": "LINEAR_PCM",
  "languageCode": "en-US",
  "voiceName": "",                  // Optional voice selection
  "speakingRate": 1.0,              // 0.5 - 2.0
  "pitch": 0.0,                     // -20.0 to 20.0
  "volumeGainDb": 0.0,
  "saveAudioFiles": true,           // Save for debugging
  "outputPath": "./data/tts_audio/"
}
```

### NLU (Intent Recognition) Settings

```json
"nlu": {
  "confidenceThreshold": 0.5,       // Minimum confidence (0.0 - 1.0)
  "enableSlotExtraction": true,     // Extract command parameters
  "enableContextTracking": false    // Track conversation context
}
```

### Voice Biometric Settings

```json
"biometric": {
  "voiceprintPath": "./config/voiceprints.json",
  "similarityThreshold": 0.75,      // Match threshold
  "enrollmentSamplesRequired": 5,   // Samples for enrollment
  "enableContinuousVerification": true,
  "verificationIntervalSec": 30,
  "sessionTimeoutMin": 10
}
```

### Command Execution Settings

```json
"commands": {
  "enableConfirmation": true,       // Confirm critical commands
  "confirmationTimeout": 10,        // Seconds
  "feedbackVolume": 80,             // TTS volume (0-100)
  "muteMicDuringTTS": true,         // Prevent echo
  "logAllCommands": true
}
```

---

## 🔄 Switching Between Modes

### Method 1: Edit Config File

```bash
# For development (laptop)
nano config/devices.json
# Change: "mode": "prod" → "mode": "dev"

# For production (Jetson)
nano config/devices.json
# Change: "mode": "dev" → "mode": "prod"
```

### Method 2: Environment-Specific Configs

```bash
# Create separate config files
cp config/devices.json config/devices.dev.json
cp config/devices.json config/devices.prod.json

# Edit each file with appropriate mode

# Load appropriate config at runtime
./rcws_app --config=config/devices.dev.json   # Development
./rcws_app --config=config/devices.prod.json  # Production
```

---

## 🚀 Production Deployment

### Step 1: Install RIVA on Jetson AGX Orin

```bash
# Download RIVA Quick Start
wget https://ngc.nvidia.com/setup/installers/cli

# Extract and run
tar -xf riva_quickstart_arm64_v2.14.0.tar
cd riva_quickstart_arm64_v2.14.0

# Configure models
./riva_init.sh

# Start RIVA server
./riva_start.sh
```

### Step 2: Verify RIVA Server

```bash
# Check server status
curl -X POST http://localhost:50051/v1/health

# Test ASR
./riva_quickstart_asr_test.sh

# Test TTS
./riva_quickstart_tts_test.sh
```

### Step 3: Configure RCWS

```bash
# Edit devices.json
nano config/devices.json

# Set production mode
{
  "riva": {
    "mode": "prod",
    "enableVoiceControl": true,
    "prod": {
      "serverUrl": "localhost:50051",
      ...
    }
  }
}
```

### Step 4: Build and Run

```bash
# Build RCWS
qmake QT6-gstreamer-example.pro
make -j8

# Run
./rcws_app
```

---

## 🧪 Testing Workflow

### Local Development (Laptop)

1. **Set dev mode** in `config/devices.json`
2. **Ensure internet connection** is available
3. **Run application:**
   ```bash
   ./rcws_app
   ```
4. **Test voice commands:**
   - Say "hi harres" (wake word)
   - Say "slew left" / "zoom in" / etc.
   - Verify TTS feedback

### Pre-Deployment Testing (Jetson)

1. **Start RIVA server locally:**
   ```bash
   ./riva_start.sh
   ```
2. **Set prod mode** in config
3. **Test voice system:**
   ```bash
   ./rcws_app
   ```
4. **Verify latency** is < 100ms
5. **Disconnect internet** and test (should still work)

---

## 📊 Performance Comparison

| Metric | Dev Mode (Cloud) | Prod Mode (Local) |
|--------|------------------|-------------------|
| **Latency (ASR)** | 300-500ms | 50-80ms |
| **Latency (TTS)** | 200-400ms | 30-50ms |
| **Internet Required** | ✅ Yes | ❌ No |
| **Data Privacy** | ⚠️ Cloud | ✅ On-prem |
| **Reliability** | ⚠️ Network-dependent | ✅ Local |
| **Setup Complexity** | ✅ Easy | ⚠️ Moderate |

---

## 🐛 Troubleshooting

### Dev Mode Issues

**Problem:** "Failed to connect to RIVA server"
```bash
# Check internet connection
ping grpc.nvcf.nvidia.com

# Verify API key is valid
# Edit config/devices.json and update "apiKey"
```

**Problem:** "Authentication failed"
```bash
# Verify API key in config/devices.json
# Get new key from: https://build.nvidia.com/
```

### Prod Mode Issues

**Problem:** "Connection refused to localhost:50051"
```bash
# Check if RIVA server is running
ps aux | grep riva

# Start RIVA server
cd riva_quickstart_arm64_v2.14.0
./riva_start.sh

# Check server logs
tail -f riva_server.log
```

**Problem:** "High latency even in prod mode"
```bash
# Check CPU/GPU usage
tegrastats

# Verify RIVA is using GPU
nvidia-smi

# Ensure models are on GPU, not CPU
```

---

## 🔐 Security Best Practices

### Development Mode
- ✅ Use only for non-classified testing
- ✅ Rotate API keys regularly
- ✅ Never commit API keys to git
- ✅ Use environment variables for keys

### Production Mode
- ✅ Enable biometric authentication
- ✅ Use TLS/SSL for gRPC (optional)
- ✅ Restrict network access to RIVA server
- ✅ Enable audit logging
- ✅ Regular voiceprint updates

---

## 📚 Code Usage Examples

### Check Current Mode

```cpp
#include "rivaconfig.h"

void myFunction() {
    if (RivaConfig::isDevMode()) {
        qDebug() << "Running in development mode";
    }

    if (RivaConfig::isProdMode()) {
        qDebug() << "Running in production mode";
    }

    qDebug() << "Mode:" << RivaConfig::modeString();
}
```

### Access Configuration

```cpp
// Get server configuration
const auto& server = RivaConfig::server();
qDebug() << "Server URL:" << server.serverUrl;
qDebug() << "Uses SSL:" << server.useSSL;

// Get ASR configuration
const auto& asr = RivaConfig::asr();
qDebug() << "Sample rate:" << asr.sampleRate;
qDebug() << "Language:" << asr.languageCode;
```

### Create gRPC Channel

```cpp
const auto& serverConfig = RivaConfig::server();

std::shared_ptr<grpc::Channel> channel;

if (serverConfig.useSSL) {
    grpc::SslCredentialsOptions ssl_opts;
    auto creds = grpc::SslCredentials(ssl_opts);
    channel = grpc::CreateCustomChannel(
        serverConfig.serverUrl.toStdString(),
        creds,
        grpc::ChannelArguments()
    );
} else {
    channel = grpc::CreateChannel(
        serverConfig.serverUrl.toStdString(),
        grpc::InsecureChannelCredentials()
    );
}
```

---

## 🎓 Additional Resources

- **NVIDIA RIVA Docs:** https://docs.nvidia.com/deeplearning/riva/
- **Jetson Setup:** https://developer.nvidia.com/embedded/learn/tutorials
- **gRPC C++ Guide:** https://grpc.io/docs/languages/cpp/

---

## 📞 Support

For issues or questions:
1. Check logs: `./logs/rcws.log`
2. Review RIVA server logs
3. Contact development team

---

**Last Updated:** January 2025
**Version:** 1.0
