#include <grpcpp/grpcpp.h>
#include "riva/proto/riva_asr.grpc.pb.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <cstring>

const std::string API_KEY = "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";
const std::string RIVA_URL = "grpc.nvcf.nvidia.com:443";

// Simple WAV header parser
struct WavHeader {
    uint32_t sample_rate;
    uint16_t channels;
    uint16_t bits_per_sample;
    uint32_t data_offset;
    uint32_t data_size;
};

bool ParseWavHeader(const std::string& wav_data, WavHeader& header) {
    if (wav_data.size() < 44) {
        std::cerr << "File too small to be a valid WAV" << std::endl;
        return false;
    }
    
    if (strncmp(wav_data.c_str(), "RIFF", 4) != 0) {
        std::cerr << "Not a valid WAV file (missing RIFF)" << std::endl;
        return false;
    }
    
    if (strncmp(wav_data.c_str() + 8, "WAVE", 4) != 0) {
        std::cerr << "Not a valid WAV file (missing WAVE)" << std::endl;
        return false;
    }
    
    if (strncmp(wav_data.c_str() + 12, "fmt ", 4) != 0) {
        std::cerr << "Missing fmt chunk" << std::endl;
        return false;
    }
    
    memcpy(&header.channels, wav_data.c_str() + 22, 2);
    memcpy(&header.sample_rate, wav_data.c_str() + 24, 4);
    memcpy(&header.bits_per_sample, wav_data.c_str() + 34, 2);
    
    // Find data chunk
    size_t pos = 12;
    while (pos < wav_data.size() - 8) {
        if (strncmp(wav_data.c_str() + pos, "data", 4) == 0) {
            memcpy(&header.data_size, wav_data.c_str() + pos + 4, 4);
            header.data_offset = pos + 8;
            return true;
        }
        uint32_t chunk_size;
        memcpy(&chunk_size, wav_data.c_str() + pos + 4, 4);
        pos += 8 + chunk_size;
    }
    
    std::cerr << "Could not find data chunk" << std::endl;
    return false;
}

int main() {
    grpc::SslCredentialsOptions ssl_opts;
    auto creds = grpc::SslCredentials(ssl_opts);
    grpc::ChannelArguments args;
    auto channel = grpc::CreateCustomChannel(RIVA_URL, creds, args);
    
    std::unique_ptr<nvidia::riva::asr::RivaSpeechRecognition::Stub> stub =
        nvidia::riva::asr::RivaSpeechRecognition::NewStub(channel);
    
    // Read entire WAV file
    std::ifstream input("/home/rapit/Desktop/Projet_RCWS/riva_test/python-clients/data/examples/en-US_sample.wav", 
                        std::ios::binary);
    if (!input) {
        std::cerr << "Failed to open audio file" << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string wav_data = buffer.str();
    
    // Parse WAV header
    WavHeader wav_header;
    if (!ParseWavHeader(wav_data, wav_header)) {
        return 1;
    }
    
    std::cout << "WAV file loaded:" << std::endl;
    std::cout << "  Sample rate: " << wav_header.sample_rate << " Hz" << std::endl;
    std::cout << "  Channels: " << wav_header.channels << std::endl;
    std::cout << "  Bits per sample: " << wav_header.bits_per_sample << std::endl;
    std::cout << "  Data offset: " << wav_header.data_offset << " bytes" << std::endl;
    std::cout << "  Audio data size: " << wav_header.data_size << " bytes" << std::endl;
    
    // Use StreamingRecognize (same as working client)
    grpc::ClientContext context;
    context.AddMetadata("authorization", "Bearer " + API_KEY);
    context.AddMetadata("function-id", "1598d209-5e27-4d3c-8079-4751568b1081");
    
    auto streamer = stub->StreamingRecognize(&context);
    
    // Send first request with config (same as working client - see line 177-200 in document 1)
    nvidia::riva::asr::StreamingRecognizeRequest request;
    auto* streaming_config = request.mutable_streaming_config();
    streaming_config->set_interim_results(false);
    
    auto* config = streaming_config->mutable_config();
    config->set_sample_rate_hertz(wav_header.sample_rate);
    config->set_language_code("en-US");
    config->set_encoding(nvidia::riva::LINEAR_PCM);
    config->set_max_alternatives(1);
    config->set_profanity_filter(false);
    config->set_audio_channel_count(wav_header.channels);
    config->set_enable_word_time_offsets(false);
    config->set_enable_automatic_punctuation(true);
    
    streamer->Write(request);
    std::cout << "\nSent configuration request" << std::endl;
    
    // Send audio data in chunks (same as working client - see line 213-240 in document 1)
    const size_t chunk_duration_ms = 100;
    const size_t chunk_size = (wav_header.sample_rate * chunk_duration_ms / 1000) * sizeof(int16_t);
    
    size_t offset = wav_header.data_offset;
    size_t audio_end = wav_header.data_offset + wav_header.data_size;
    int chunk_count = 0;
    
    while (offset < audio_end) {
        nvidia::riva::asr::StreamingRecognizeRequest audio_request;
        size_t bytes_to_send = std::min(chunk_size, audio_end - offset);
        audio_request.set_audio_content(wav_data.data() + offset, bytes_to_send);
        offset += bytes_to_send;
        
        streamer->Write(audio_request);
        chunk_count++;
    }
    
    std::cout << "Sent " << chunk_count << " audio chunks" << std::endl;
    streamer->WritesDone();
    
    // Receive responses (same as working client - see line 398-412 in document 1)
    nvidia::riva::asr::StreamingRecognizeResponse response;
    std::cout << "\nReceiving responses:" << std::endl;
    
    while (streamer->Read(&response)) {
        for (int r = 0; r < response.results_size(); ++r) {
            const auto& result = response.results(r);
            std::cout << "Result " << r << " (final: " << result.is_final() << ")" << std::endl;
            
            for (int a = 0; a < result.alternatives_size(); ++a) {
                const auto& alternative = result.alternatives(a);
                std::cout << "  Alternative " << a << ":" << std::endl;
                std::cout << "    Transcript: " << alternative.transcript() << std::endl;
                std::cout << "    Confidence: " << alternative.confidence() << std::endl;
            }
        }
    }
    
    grpc::Status status = streamer->Finish();
    if (!status.ok()) {
        std::cerr << "\nRecognition failed: " << status.error_message() << std::endl;
        std::cerr << "Error code: " << status.error_code() << std::endl;
        return 1;
    }
    
    std::cout << "\nRecognition completed successfully!" << std::endl;
    return 0;
}
