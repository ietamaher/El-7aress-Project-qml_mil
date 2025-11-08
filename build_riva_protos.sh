#!/bin/bash

# ==============================================================================
# RIVA Proto Files Build Script
# ==============================================================================
# This script compiles NVIDIA RIVA .proto files to C++ code
#
# Prerequisites:
# - protoc (Protocol Buffers compiler)
# - grpc_cpp_plugin (gRPC C++ plugin)
#
# Install on Ubuntu:
#   sudo apt install -y protobuf-compiler libgrpc++-dev

set -e

echo "========================================"
echo "Building RIVA Proto Files"
echo "========================================"

# Proto directory
PROTO_DIR="Features/riva_test/riva_ASR_TTS_NLU/riva/proto"

if [ ! -d "$PROTO_DIR" ]; then
    echo "Error: Proto directory not found: $PROTO_DIR"
    exit 1
fi

cd "$PROTO_DIR"
echo "Working directory: $(pwd)"

# Check if protoc is installed
if ! command -v protoc &> /dev/null; then
    echo "Error: protoc not found. Install with:"
    echo "  sudo apt install protobuf-compiler"
    exit 1
fi

# Check if grpc_cpp_plugin is installed
if ! command -v grpc_cpp_plugin &> /dev/null; then
    echo "Error: grpc_cpp_plugin not found. Install with:"
    echo "  sudo apt install libgrpc++-dev"
    exit 1
fi

echo "protoc version: $(protoc --version)"
echo "grpc_cpp_plugin: $(which grpc_cpp_plugin)"
echo ""

# Clean previous generated files
echo "Cleaning previous generated files..."
rm -f *.pb.h *.pb.cc *.grpc.pb.h *.grpc.pb.cc

# Function to compile a proto file
compile_proto() {
    local proto_file=$1
    echo "Compiling $proto_file..."

    protoc --cpp_out=. --grpc_out=. \
           --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
           "$proto_file"

    if [ $? -eq 0 ]; then
        echo "  ✓ $proto_file compiled successfully"
    else
        echo "  ✗ Failed to compile $proto_file"
        exit 1
    fi
}

# Compile proto files in dependency order
# (dependencies must be compiled first)

echo ""
echo "Compiling proto files..."
echo "----------------------------------------"

# 1. Common (no dependencies)
compile_proto "riva_common.proto"

# 2. Audio (depends on common)
compile_proto "riva_audio.proto"

# 3. ASR (depends on audio, common)
compile_proto "riva_asr.proto"

# 4. TTS (depends on audio, common)
compile_proto "riva_tts.proto"

# 5. NLP (optional, if you need NLU)
if [ -f "riva_nlp.proto" ]; then
    compile_proto "riva_nlp.proto"
fi

echo ""
echo "========================================"
echo "Proto compilation complete!"
echo "========================================"
echo ""
echo "Generated files:"
ls -lh *.pb.h *.pb.cc 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}'
echo ""
echo "You can now build your Qt project:"
echo "  qmake"
echo "  make -j\$(nproc)"
echo ""
