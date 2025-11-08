# RIVA Voice Control - Build Guide

## 📦 Prerequisites

### 1. Install System Dependencies

```bash
# Update package list
sudo apt update

# Install build essentials
sudo apt install -y build-essential cmake git

# Install gRPC and Protobuf
sudo apt install -y \
    protobuf-compiler \
    libprotobuf-dev \
    libgrpc++-dev \
    libgrpc-dev

# Install ALSA development libraries (audio)
sudo apt install -y libasound2-dev

# Install Qt6 (if not already installed)
sudo apt install -y \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-serialbus-dev \
    qt6-serialport-dev
```

### 2. Verify Installation

```bash
# Check protoc
protoc --version
# Should show: libprotoc 3.x.x or higher

# Check grpc_cpp_plugin
which grpc_cpp_plugin
# Should show: /usr/bin/grpc_cpp_plugin

# Check ALSA
pkg-config --modversion alsa
# Should show: 1.x.x
```

---

## 🔨 Build Steps

### Step 1: Compile Proto Files

The proto files are already in your project at:
`Features/riva_test/riva_ASR_TTS_NLU/riva/proto/`

**Option A: Use the automated script (recommended):**

```bash
cd /path/to/El-7aress-Project-qml_mil

# Run the proto build script
./build_riva_protos.sh
```

**Option B: Manual compilation:**

```bash
cd Features/riva_test/riva_ASR_TTS_NLU/riva/proto

# Compile common proto (no dependencies)
protoc --cpp_out=. --grpc_out=. \
       --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
       riva_common.proto

# Compile audio proto
protoc --cpp_out=. --grpc_out=. \
       --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
       riva_audio.proto

# Compile ASR proto
protoc --cpp_out=. --grpc_out=. \
       --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
       riva_asr.proto

# Compile TTS proto
protoc --cpp_out=. --grpc_out=. \
       --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
       riva_tts.proto
```

**Expected output:**
```
riva_common.pb.h
riva_common.pb.cc
riva_common.grpc.pb.h
riva_common.grpc.pb.cc
riva_audio.pb.h
riva_audio.pb.cc
riva_asr.pb.h
riva_asr.pb.cc
riva_asr.grpc.pb.h
riva_asr.grpc.pb.cc
riva_tts.pb.h
riva_tts.pb.cc
riva_tts.grpc.pb.h
riva_tts.grpc.pb.cc
```

### Step 2: Update Your .pro File

Add this line to `QT6-gstreamer-example.pro`:

```qmake
# Include RIVA voice control
include(riva_voice.pri)
```

The `riva_voice.pri` file includes all necessary:
- Headers and sources for RIVA devices
- Proto generated files
- Library linkage (gRPC, protobuf, ALSA)
- Include paths

### Step 3: Build the Project

```bash
# Clean previous build
make clean
rm -rf build
mkdir build
cd build

# Run qmake
qmake ../QT6-gstreamer-example.pro

# Build (use all cores)
make -j$(nproc)
```

### Step 4: Run the Application

```bash
# From build directory
./QT6-gstreamer-example

# Or from project root
./build/QT6-gstreamer-example
```

---

## 🧪 Testing RIVA Integration

### Test 1: Check Configuration Loading

Look for these messages in the console output:

```
✅ [RIVA CONFIG] Loaded successfully
   Mode: DEVELOPMENT
   Server: grpc.nvcf.nvidia.com:443
   Voice Control: ENABLED
   Biometric Auth: DISABLED
```

### Test 2: Check ASR Device

If voice control is enabled, you should see:

```
🎤 [ASR] RivaAsrDevice created
   Audio device: default
   Sample rate: 16000 Hz
   Channels: 2
   Wake word: hi harres

🎤 [ASR] Starting device...
  [ASR] Opening audio device: default
  ✓ Audio device opened successfully
  [ASR] Initializing gRPC stream...
  ✓ gRPC stream initialized
  [ASR] Sending configuration...
  ✓ Configuration sent
✅ [ASR] Device started successfully
```

### Test 3: Test Voice Recognition

1. **Say the wake word:** "Hi Harres"
2. **Expected output:**
   ```
   ✅ [ASR] Wake word detected: hi harres
   🎤 [VOICE] Wake word detected
   ```

3. **Say a command:** "Slew left"
4. **Expected output:**
   ```
   🎤 [VOICE] Transcript: slew left
   🧠 [NLU] Analyzing: "slew left"
      Intent: GIMBAL_SLEW_LEFT (confidence: 0.95)
   🔊 [TTS] Speaking: "Slewing left"
   💾 [TTS] Saved to: ./data/tts_audio/tts_feedback_0.wav
   ```

---

## 🐛 Troubleshooting

### Problem: Proto compilation fails

**Error:** `protoc: command not found`

**Solution:**
```bash
sudo apt install protobuf-compiler
```

**Error:** `grpc_cpp_plugin: not found`

**Solution:**
```bash
sudo apt install libgrpc++-dev
```

### Problem: Build fails with "undefined reference to grpc"

**Error:**
```
undefined reference to `grpc::Channel::Create`
```

**Solution:** Make sure `riva_voice.pri` is included in your .pro file:
```qmake
include(riva_voice.pri)
```

### Problem: Audio device not found

**Error:**
```
✗ Cannot open audio device: No such device
```

**Solution:** Check available audio devices:
```bash
# List ALSA devices
arecord -L

# Test microphone
arecord -d 3 test.wav
aplay test.wav
```

Update `config/devices.json`:
```json
"asr": {
  "audioDevice": "plughw:1,0"  // Use specific device
}
```

### Problem: Connection to RIVA server fails (dev mode)

**Error:**
```
Failed to initialize gRPC stream
```

**Solutions:**

1. **Check internet connection:**
   ```bash
   ping grpc.nvcf.nvidia.com
   ```

2. **Verify API key:**
   Edit `config/devices.json` and check `dev.apiKey` is correct

3. **Test with curl:**
   ```bash
   curl -H "Authorization: Bearer YOUR_API_KEY" \
        https://grpc.nvcf.nvidia.com
   ```

### Problem: High CPU usage

**Cause:** Audio capture running at high rate

**Solution:** Reduce chunk duration in `config/devices.json`:
```json
"asr": {
  "chunkDurationMs": 200  // Increase from 100ms to 200ms
}
```

---

## 📁 File Structure After Build

```
El-7aress-Project-qml_mil/
├── config/
│   └── devices.json                    ← RIVA configuration
│
├── src/
│   ├── controllers/
│   │   ├── rivaconfig.h                ← Configuration manager
│   │   └── rivaconfig.cpp
│   │
│   └── hardware/devices/
│       ├── rivaasrdevice.h             ← ASR device (audio → text)
│       ├── rivaasrdevice.cpp
│       ├── rivattsclient.h             ← TTS client (text → audio)
│       └── rivattsclient.cpp
│
├── Features/riva_test/riva_ASR_TTS_NLU/riva/proto/
│   ├── riva_common.proto               ← Proto definitions
│   ├── riva_asr.proto
│   ├── riva_tts.proto
│   ├── riva_common.pb.h                ← Generated C++ (after build)
│   ├── riva_common.pb.cc
│   ├── riva_asr.pb.h
│   ├── riva_asr.pb.cc
│   └── ... (more generated files)
│
├── riva_voice.pri                      ← Build configuration
├── build_riva_protos.sh                ← Proto build script
└── QT6-gstreamer-example.pro           ← Main project file
```

---

## 🚀 Quick Start Summary

```bash
# 1. Install dependencies
sudo apt install -y protobuf-compiler libgrpc++-dev libasound2-dev

# 2. Compile proto files
./build_riva_protos.sh

# 3. Add to .pro file
echo "include(riva_voice.pri)" >> QT6-gstreamer-example.pro

# 4. Build
qmake
make -j$(nproc)

# 5. Run
./QT6-gstreamer-example
```

---

## 📊 Build Time Expectations

| Step | Time (typical) |
|------|----------------|
| Proto compilation | 5-10 seconds |
| Initial Qt build | 3-5 minutes |
| Incremental build | 30-60 seconds |

---

## 🔄 Switching Modes

### Development Mode (Cloud API)
```json
// config/devices.json
{
  "riva": {
    "mode": "dev"  // ← Uses cloud
  }
}
```

No rebuild needed! Just restart the application.

### Production Mode (Local Server)
```json
// config/devices.json
{
  "riva": {
    "mode": "prod"  // ← Uses localhost:50051
  }
}
```

Make sure RIVA server is running:
```bash
./riva_start.sh
```

---

## 📚 Additional Resources

- **gRPC C++ Guide:** https://grpc.io/docs/languages/cpp/quickstart/
- **Protobuf Guide:** https://developers.google.com/protocol-buffers/docs/cpptutorial
- **ALSA Programming:** https://www.alsa-project.org/alsa-doc/alsa-lib/
- **NVIDIA RIVA Docs:** https://docs.nvidia.com/deeplearning/riva/

---

## ✅ Verification Checklist

Before running your application, verify:

- [ ] Proto files compiled successfully (`.pb.h` and `.pb.cc` files exist)
- [ ] `riva_voice.pri` included in main `.pro` file
- [ ] gRPC libraries installed (`libgrpc++-dev`)
- [ ] ALSA libraries installed (`libasound2-dev`)
- [ ] Config file has correct mode (`dev` or `prod`)
- [ ] API key configured (for dev mode)
- [ ] Audio device accessible (run `arecord -L`)

---

**Ready to build! Any issues? Check the Troubleshooting section above.** 🎤

