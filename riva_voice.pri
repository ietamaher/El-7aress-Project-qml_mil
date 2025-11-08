# ==============================================================================
# RIVA Voice Control Integration
# ==============================================================================
# Include this file in your main .pro file:
# include(riva_voice.pri)
#
# Prerequisites:
# 1. gRPC and Protobuf installed
# 2. ALSA development libraries installed
# 3. Proto files compiled to C++ (see instructions below)

# ==============================================================================
# CONFIGURATION
# ==============================================================================

# Enable RIVA voice control
CONFIG += riva_voice

# Proto files location (use existing proto files from Features folder)
RIVA_PROTO_DIR = $$PWD/Features/riva_test/riva_ASR_TTS_NLU/riva/proto

# gRPC/Protobuf paths (adjust for your system)
# Option 1: System-installed gRPC/Protobuf
# (usually in /usr/local or /usr)

# Option 2: Custom installation path
# GRPC_PREFIX = $$HOME/riva_deps
# INCLUDEPATH += $$GRPC_PREFIX/include

# ==============================================================================
# HEADERS
# ==============================================================================

HEADERS += \
    $$PWD/src/controllers/rivaconfig.h \
    $$PWD/src/hardware/devices/rivaasrdevice.h \
    $$PWD/src/hardware/devices/rivattsclient.h

# ==============================================================================
# SOURCES
# ==============================================================================

SOURCES += \
    $$PWD/src/controllers/rivaconfig.cpp \
    $$PWD/src/hardware/devices/rivaasrdevice.cpp \
    $$PWD/src/hardware/devices/rivattsclient.cpp

# ==============================================================================
# PROTO GENERATED FILES
# ==============================================================================
# These are generated from .proto files
# See "Building Proto Files" section below

HEADERS += \
    $$RIVA_PROTO_DIR/riva_asr.pb.h \
    $$RIVA_PROTO_DIR/riva_asr.grpc.pb.h \
    $$RIVA_PROTO_DIR/riva_tts.pb.h \
    $$RIVA_PROTO_DIR/riva_tts.grpc.pb.h \
    $$RIVA_PROTO_DIR/riva_audio.pb.h \
    $$RIVA_PROTO_DIR/riva_common.pb.h

SOURCES += \
    $$RIVA_PROTO_DIR/riva_asr.pb.cc \
    $$RIVA_PROTO_DIR/riva_asr.grpc.pb.cc \
    $$RIVA_PROTO_DIR/riva_tts.pb.cc \
    $$RIVA_PROTO_DIR/riva_tts.grpc.pb.cc \
    $$RIVA_PROTO_DIR/riva_audio.pb.cc \
    $$RIVA_PROTO_DIR/riva_common.pb.cc

# ==============================================================================
# INCLUDE PATHS
# ==============================================================================

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/Features/riva_test/riva_ASR_TTS_NLU

# ==============================================================================
# LIBRARIES
# ==============================================================================

# gRPC and Protobuf libraries
LIBS += -lgrpc++ -lgrpc -lprotobuf

# ALSA audio library
LIBS += -lasound

# pthread (required by gRPC)
LIBS += -lpthread

# ==============================================================================
# COMPILER FLAGS
# ==============================================================================

# C++17 required
CONFIG += c++17

# Suppress warnings from generated proto files
QMAKE_CXXFLAGS += -Wno-unused-parameter

# ==============================================================================
# BUILDING PROTO FILES
# ==============================================================================
#
# Before building, you need to compile the .proto files to C++:
#
# cd Features/riva_test/riva_ASR_TTS_NLU/riva/proto
#
# # Compile proto files
# protoc --cpp_out=. --grpc_out=. \
#        --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
#        riva_common.proto
#
# protoc --cpp_out=. --grpc_out=. \
#        --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
#        riva_audio.proto
#
# protoc --cpp_out=. --grpc_out=. \
#        --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
#        riva_asr.proto
#
# protoc --cpp_out=. --grpc_out=. \
#        --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
#        riva_tts.proto
#
# Or use the build_protos.sh script (see below)
#
# ==============================================================================

message("RIVA Voice Control enabled")
message("  Proto dir: $$RIVA_PROTO_DIR")
