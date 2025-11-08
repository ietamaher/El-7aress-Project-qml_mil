#include <grpcpp/grpcpp.h>
#include "riva/proto/riva_asr.grpc.pb.h"
#include "riva/proto/riva_tts.grpc.pb.h"  // ← TTS ADDED
#include <iostream>
#include <memory>
#include <cstring>
#include <csignal>  
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <alsa/asoundlib.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <map>

const std::string API_KEY = "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";
const std::string RIVA_URL = "grpc.nvcf.nvidia.com:443";

// System state
enum class SystemState {
    WAITING_FOR_WAKE_WORD,
    LISTENING_FOR_COMMAND,
    PROCESSING_COMMAND,
    SPEAKING_FEEDBACK  // ← NEW STATE for TTS
};

 

// ============= TTS CLIENT =============
class RivaTTSClient {
private:
    std::unique_ptr<nvidia::riva::tts::RivaSpeechSynthesis::Stub> tts_stub_;
    std::string api_key_;
    int feedback_counter_ = 0;
    
    void save_audio_to_file(const std::string& audio_data, const std::string& filename, int sample_rate) {
        std::ofstream out(filename, std::ios::binary);
        
        uint32_t data_size = audio_data.size();
        uint32_t file_size = data_size + 36;
        uint16_t channels = 1;
        uint16_t bits_per_sample = 16;
        uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
        uint16_t block_align = channels * bits_per_sample / 8;
        
        out.write("RIFF", 4);
        out.write(reinterpret_cast<const char*>(&file_size), 4);
        out.write("WAVE", 4);
        out.write("fmt ", 4);
        uint32_t fmt_size = 16;
        out.write(reinterpret_cast<const char*>(&fmt_size), 4);
        uint16_t audio_format = 1;
        out.write(reinterpret_cast<const char*>(&audio_format), 2);
        out.write(reinterpret_cast<const char*>(&channels), 2);
        out.write(reinterpret_cast<const char*>(&sample_rate), 4);
        out.write(reinterpret_cast<const char*>(&byte_rate), 4);
        out.write(reinterpret_cast<const char*>(&block_align), 2);
        out.write(reinterpret_cast<const char*>(&bits_per_sample), 2);
        out.write("data", 4);
        out.write(reinterpret_cast<const char*>(&data_size), 4);
        out.write(audio_data.data(), data_size);
    }
    
public:
    RivaTTSClient(std::shared_ptr<grpc::Channel> channel, const std::string& api_key) 
        : tts_stub_(nvidia::riva::tts::RivaSpeechSynthesis::NewStub(channel)),
          api_key_(api_key) {}
    
	bool speak(const std::string& text) {
	    std::cout << "\n🔊 [TTS] \"" << text << "\"" << std::endl;
	    
	    grpc::ClientContext context;
	    context.AddMetadata("authorization", "Bearer " + api_key_);
	    context.AddMetadata("function-id", "55cf67bf-600f-4b04-8eac-12ed39537a08");  // ← ai-radtts-hifigan-riva
	    
	    nvidia::riva::tts::SynthesizeSpeechRequest request;
	    request.set_text(text);
	    request.set_language_code("en-US");
	    request.set_encoding(nvidia::riva::LINEAR_PCM);
	    request.set_sample_rate_hz(22050);
	    // ❌ REMOVE THIS LINE - voice name doesn't exist!
	    // request.set_voice_name("English-US.Female-1");
	    
	    nvidia::riva::tts::SynthesizeSpeechResponse response;
	    grpc::Status status = tts_stub_->Synthesize(&context, request, &response);
	    
	    if (!status.ok()) {
		std::cerr << "❌ [TTS] Failed: " << status.error_message() << std::endl;
		return false;
	    }
	    
	    // Save to file (avoids audio feedback loop with microphone)
	    std::string filename = "tts_feedback_" + std::to_string(feedback_counter_++) + ".wav";
	    save_audio_to_file(response.audio(), filename, 22050);
	    std::cout << "💾 [TTS] Saved to: " << filename << std::endl;
	    
	    // Play the audio using aplay
	    std::string play_command = "aplay -q " + filename + " 2>/dev/null &";
	    system(play_command.c_str());
	    
	    // Simulate speaking time (so microphone doesn't pick up command echo)
	    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	    
	    return true;
	}
};

// ============= SIMPLIFIED BMW VEHICLE CONTROL (3 COMMANDS ONLY) =============
class BMWVehicleControl {
public:
    void openWindows() {
        std::cout << "🚗 [ACTION] Opening windows..." << std::endl;
        // TODO: Add actual CAN bus or vehicle control logic
    }
    
    void closeWindows() {
        std::cout << "🚗 [ACTION] Closing windows..." << std::endl;
        // TODO: Add actual CAN bus or vehicle control logic
    }
    
    void turnOnAirConditioning() {
        std::cout << "🚗 [ACTION] Turning on air conditioning..." << std::endl;
        // TODO: Add actual CAN bus or vehicle control logic
    }
};


// ============= NLU INTENT & SLOT DEFINITIONS ============= 

enum class Intent {
    OPEN_WINDOWS,
    CLOSE_WINDOWS,
    CLIMATE_CONTROL,
    MEDIA_CONTROL,
    UNKNOWN
};

struct Slot {
    std::string name;
    std::string value;
};

struct NLUResult {
    Intent intent;
    std::vector<Slot> slots;
    float confidence;
};

// ============= SIMPLE NLU ENGINE =============
class SimpleNLUEngine {
private:
    std::map<std::string, std::vector<std::string>> intent_patterns_;
    
    std::string toLowerCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    bool containsAny(const std::string& text, const std::vector<std::string>& keywords) {
        std::string lower = toLowerCase(text);
        for (const auto& keyword : keywords) {
            if (lower.find(toLowerCase(keyword)) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    std::string extractLocation(const std::string& text) {
        std::string lower = toLowerCase(text);
        if (lower.find("rear") != std::string::npos || lower.find("back") != std::string::npos) {
            return "rear";
        }
        if (lower.find("front") != std::string::npos) {
            return "front";
        }
        if (lower.find("all") != std::string::npos) {
            return "all";
        }
        return "all"; // default
    }
    
    int extractTemperature(const std::string& text) {
        std::string lower = toLowerCase(text);
        size_t pos = 0;
        while (pos < lower.length()) {
            if (isdigit(lower[pos])) {
                int temp = 0;
                while (pos < lower.length() && isdigit(lower[pos])) {
                    temp = temp * 10 + (lower[pos] - '0');
                    pos++;
                }
                if (temp >= 16 && temp <= 30) {
                    return temp;
                }
            }
            pos++;
        }
        return -1;
    }
    
public:
    SimpleNLUEngine() {
        // Define intent patterns - use string keys instead of enum
        intent_patterns_["OPEN_WINDOWS"] = {
            "open", "roll down", "lower", "open up", "wind down"
        };
        intent_patterns_["CLOSE_WINDOWS"] = {
            "close", "roll up", "raise", "shut", "wind up"
        };
        intent_patterns_["CLIMATE_CONTROL"] = {
            "air conditioning", "climate", "temperature", "ac", "heat", "cool"
        };
        intent_patterns_["MEDIA_CONTROL"] = {
            "music", "play", "pause", "next", "previous", "volume"
        };
    }
    
    NLUResult classify(const std::string& text) {
        NLUResult result;
        result.confidence = 0.0f;
        result.intent = Intent::UNKNOWN;
        
        std::cout << "🧠 [NLU] Analyzing: \"" << text << "\"" << std::endl;
        
        // Check for window commands
        if (containsAny(text, {"window", "windows"})) {
            if (containsAny(text, intent_patterns_["OPEN_WINDOWS"])) {
                result.intent = Intent::OPEN_WINDOWS;
                result.confidence = 0.95f;
                
                std::string location = extractLocation(text);
                result.slots.push_back({"location", location});
                
                std::cout << "   Intent: OPEN_WINDOWS (confidence: " << result.confidence << ")" << std::endl;
                std::cout << "   Slot: location=" << location << std::endl;
            }
            else if (containsAny(text, intent_patterns_["CLOSE_WINDOWS"])) {
                result.intent = Intent::CLOSE_WINDOWS;
                result.confidence = 0.95f;
                
                std::string location = extractLocation(text);
                result.slots.push_back({"location", location});
                
                std::cout << "   Intent: CLOSE_WINDOWS (confidence: " << result.confidence << ")" << std::endl;
                std::cout << "   Slot: location=" << location << std::endl;
            }
        }
        // Check for climate commands
        else if (containsAny(text, intent_patterns_["CLIMATE_CONTROL"])) {
            result.intent = Intent::CLIMATE_CONTROL;
            result.confidence = 0.90f;
            
            // Determine action
            if (containsAny(text, {"on", "start", "turn on"})) {
                result.slots.push_back({"action", "on"});
            } else if (containsAny(text, {"off", "stop", "turn off"})) {
                result.slots.push_back({"action", "off"});
            }
            
            // Extract temperature if present
            int temp = extractTemperature(text);
            if (temp != -1) {
                result.slots.push_back({"temperature", std::to_string(temp)});
            }
            
            std::cout << "   Intent: CLIMATE_CONTROL (confidence: " << result.confidence << ")" << std::endl;
        }
        // Check for media commands
        else if (containsAny(text, intent_patterns_["MEDIA_CONTROL"])) {
            result.intent = Intent::MEDIA_CONTROL;
            result.confidence = 0.90f;
            
            if (containsAny(text, {"play"})) {
                result.slots.push_back({"action", "play"});
            } else if (containsAny(text, {"pause", "stop"})) {
                result.slots.push_back({"action", "pause"});
            }
            
            std::cout << "   Intent: MEDIA_CONTROL (confidence: " << result.confidence << ")" << std::endl;
        }
        
        if (result.intent == Intent::UNKNOWN) {
            std::cout << "   Intent: UNKNOWN" << std::endl;
        }
        
        return result;
    }
};

// =============  COMMAND PROCESSOR (3 COMMANDS ONLY) =============
class CommandProcessor {
private:
    BMWVehicleControl& vehicle_;
    RivaTTSClient& tts_;
    SimpleNLUEngine nlu_;  // ← Add NLU engine
    
public:
    CommandProcessor(BMWVehicleControl& ctrl, RivaTTSClient& tts) 
        : vehicle_(ctrl), tts_(tts) {}
    
    bool processCommand(const std::string& command) {
        std::cout << "\n📝 [COMMAND] Processing: \"" << command << "\"" << std::endl;
        
        // Use NLU to classify
        NLUResult nlu_result = nlu_.classify(command);
        
        if (nlu_result.confidence < 0.5f) {
            std::cout << "⚠️  [WARNING] Low confidence, command not recognized" << std::endl;
            tts_.speak("Sorry, I didn't understand that command");
            return false;
        }
        
        // Execute based on intent
        switch (nlu_result.intent) {
            case Intent::OPEN_WINDOWS: {
                std::string location = "all";
                for (const auto& slot : nlu_result.slots) {
                    if (slot.name == "location") location = slot.value;
                }
                
                std::string feedback = "Opening " + location + " windows";
                tts_.speak(feedback);
                vehicle_.openWindows();
                return true;
            }
            
            case Intent::CLOSE_WINDOWS: {
                std::string location = "all";
                for (const auto& slot : nlu_result.slots) {
                    if (slot.name == "location") location = slot.value;
                }
                
                std::string feedback = "Closing " + location + " windows";
                tts_.speak(feedback);
                vehicle_.closeWindows();
                return true;
            }
            
		case Intent::CLIMATE_CONTROL: {
		    // Extract action slot (on/off)
		    std::string action = "on";  // default
		    int temperature = -1;
		    
		    for (const auto& slot : nlu_result.slots) {
			if (slot.name == "action") action = slot.value;
			if (slot.name == "temperature") temperature = std::stoi(slot.value);
		    }
		    
		    // Execute based on action
		    if (action == "on") {
			tts_.speak("Turning on air conditioning");
			vehicle_.turnOnAirConditioning();
		    } 
		    else if (action == "off") {
			tts_.speak("Turning off air conditioning");
			//vehicle_.turnOffAirConditioning();  // ← Need to add this method!
		    }
		    else if (temperature != -1) {
			std::string feedback = "Setting temperature to " + std::to_string(temperature) + " degrees";
			tts_.speak(feedback);
			//vehicle_.setTemperature(temperature);   // ← Need to add this method!
		    }
		    
		    return true;
		}
            
            case Intent::MEDIA_CONTROL: {
                tts_.speak("Media control not yet implemented");
                return true;
            }
            
            default:
                tts_.speak("Command not recognized");
                return false;
        }
    }
};

// ============= AUDIO DEVICE MANAGEMENT =============
bool OpenAudioDevice(
    const char* device_name, snd_pcm_t** handle, snd_pcm_stream_t stream_type,
    int channels, int samplerate, int latency_us)
{
    int err;
    
    if ((err = snd_pcm_open(handle, device_name, stream_type, 0)) < 0) {
        std::cerr << "Cannot open audio device " << device_name 
                  << " (" << snd_strerror(err) << ")" << std::endl;
        return false;
    }
    
    snd_pcm_hw_params_t* hw_params;
    snd_pcm_hw_params_alloca(&hw_params);
    snd_pcm_hw_params_any(*handle, hw_params);
    snd_pcm_hw_params_set_access(*handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(*handle, hw_params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate_near(*handle, hw_params, (unsigned int*)&samplerate, 0);
    snd_pcm_hw_params_set_channels(*handle, hw_params, channels);
    
    unsigned int buffer_time = latency_us;
    snd_pcm_hw_params_set_buffer_time_near(*handle, hw_params, &buffer_time, 0);
    
    if ((err = snd_pcm_hw_params(*handle, hw_params)) < 0) {
        std::cerr << "Cannot set hardware parameters (" << snd_strerror(err) << ")" << std::endl;
        return false;
    }
    
    if ((err = snd_pcm_prepare(*handle)) < 0) {
        std::cerr << "Cannot prepare audio interface (" << snd_strerror(err) << ")" << std::endl;
        return false;
    }
    
    return true;
}

void CloseAudioDevice(snd_pcm_t** handle)
{
    if (*handle) {
        snd_pcm_close(*handle);
        *handle = nullptr;
    }
}

// ============= BMW VOICE CONTROL CLIENT WITH TTS =============
class BMWVoiceControlClient {
private:
    std::unique_ptr<nvidia::riva::asr::RivaSpeechRecognition::Stub> asr_stub_;
    std::unique_ptr<RivaTTSClient> tts_client_;
    SystemState state_;
    BMWVehicleControl vehicle_;
    std::unique_ptr<CommandProcessor> commandProcessor_;
    std::atomic<bool> should_exit_;
    std::mutex state_mutex_;
 
    using AsrStream =
        grpc::ClientReaderWriter<nvidia::riva::asr::StreamingRecognizeRequest,
                                   nvidia::riva::asr::StreamingRecognizeResponse>;   
        
        
    const std::string WAKE_WORD = "hi harris";
    const int SAMPLE_RATE = 16000;
    const int CHANNELS = 2;
    const int CHUNK_DURATION_MS = 100;
    
 
    
    std::string toLowerCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    bool containsWakeWord(const std::string& text) {
        return toLowerCase(text).find(WAKE_WORD) != std::string::npos;
    }
    

                                                 
    void MicrophoneThreadMain(std::unique_ptr<AsrStream>& streamer,  
                          snd_pcm_t* alsa_handle)                                                 
    {
        nvidia::riva::asr::StreamingRecognizeRequest request;
        
        const size_t frames_per_chunk = SAMPLE_RATE * CHUNK_DURATION_MS / 1000;
        const size_t stereo_chunk_size = frames_per_chunk * sizeof(int16_t) * CHANNELS;
        std::vector<char> stereo_chunk(stereo_chunk_size);
        
        const size_t mono_chunk_size = frames_per_chunk * sizeof(int16_t);
        std::vector<char> mono_chunk(mono_chunk_size);
        
	while (!should_exit_) {
	    // Pause mic during TTS feedback to avoid echo
	    {
		std::lock_guard<std::mutex> lock(state_mutex_);
		if (state_ == SystemState::SPEAKING_FEEDBACK) {
		    // Drain the buffer to prevent overflow
		    if (alsa_handle) {
		        snd_pcm_drop(alsa_handle);  // Drop pending frames
		        snd_pcm_prepare(alsa_handle);  // Re-prepare device
		    }
		    std::this_thread::sleep_for(std::chrono::milliseconds(50));
		    continue;
		}
	    }
	    
	    if (alsa_handle) {
		int frames_read = snd_pcm_readi(alsa_handle, &stereo_chunk[0], frames_per_chunk);
		
		if (frames_read > 0) {
		    // Convert stereo to mono
		    int16_t* stereo_samples = (int16_t*)stereo_chunk.data();
		    int16_t* mono_samples = (int16_t*)mono_chunk.data();
		    
		    for (int i = 0; i < frames_read; ++i) {
		        mono_samples[i] = stereo_samples[i * CHANNELS];
		    }
		    
		    size_t mono_bytes_to_send = frames_read * sizeof(int16_t);
		    request.set_audio_content(mono_chunk.data(), mono_bytes_to_send);
		    
		    if (!streamer->Write(request)) {
		        std::cerr << "❌ [ERROR] Failed to write audio data" << std::endl;
		        break;
		    }
		} 
		else if (frames_read == -EPIPE) {
		    // Buffer overrun/underrun - recover!
		    std::cerr << "⚠️  [ALSA] Buffer overrun, recovering..." << std::endl;
		    int err = snd_pcm_recover(alsa_handle, frames_read, 1);  // 1 = silent recovery
		    if (err < 0) {
		        std::cerr << "❌ [ALSA] Recovery failed: " << snd_strerror(err) << std::endl;
		        break;
		    }
		    // Re-prepare after recovery
		    snd_pcm_prepare(alsa_handle);
		}
		else if (frames_read < 0) {
		    // Other errors - try to recover
		    std::cerr << "❌ [ERROR] Read failed: " << snd_strerror(frames_read) << std::endl;
		    int err = snd_pcm_recover(alsa_handle, frames_read, 1);
		    if (err < 0) {
		        std::cerr << "❌ [ALSA] Cannot recover: " << snd_strerror(err) << std::endl;
		        break;
		    }
		}
	    } else {
		break;
	    }
	}

	streamer->WritesDone();
    }
    
    //void ResponseThreadMain(StreamerType& streamer)
    void ResponseThreadMain(std::unique_ptr<AsrStream>& streamer) 
    {
        nvidia::riva::asr::StreamingRecognizeResponse response;
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "🚗 BMW Voice Control System Ready" << std::endl;
        std::cout << "📣 Say 'hi harres' to start a command" << std::endl;
        std::cout << "📋 Available commands:" << std::endl;
        std::cout << "   • 'open windows'" << std::endl;
        std::cout << "   • 'close windows'" << std::endl;
        std::cout << "   • 'turn on air conditioning'" << std::endl;
        std::cout << "⌨️  Press Ctrl-C to exit" << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        while (streamer->Read(&response)) {
            for (int r = 0; r < response.results_size(); ++r) {
                const auto& result = response.results(r);
                
                if (result.alternatives_size() > 0) {
                    const auto& alternative = result.alternatives(0);
                    std::string transcript = alternative.transcript();
                    
                    if (result.is_final() && !transcript.empty()) {
                        std::lock_guard<std::mutex> lock(state_mutex_);
                        
                        if (state_ == SystemState::WAITING_FOR_WAKE_WORD) {
                            std::cout << "🎤 [LISTENING] " << transcript << std::endl;
                            
                            if (containsWakeWord(transcript)) {
                                std::cout << "\n✅ [WAKE WORD DETECTED] Ready for command!\n" << std::endl;
                                state_ = SystemState::LISTENING_FOR_COMMAND;
                            }
                        }
                        else if (state_ == SystemState::LISTENING_FOR_COMMAND) {
                            std::cout << "📢 [COMMAND RECEIVED] " << transcript << std::endl;
                            state_ = SystemState::SPEAKING_FEEDBACK;
                            
                            // Process command (TTS happens inside)
                            commandProcessor_->processCommand(transcript);
                            
                            // Return to waiting for wake word
                            std::cout << "\n✅ [READY] Say 'hi Harres' for next command\n" << std::endl;
                            state_ = SystemState::WAITING_FOR_WAKE_WORD;
                        }
                    } else if (!result.is_final()) {
                        std::lock_guard<std::mutex> lock(state_mutex_);
                        if (state_ == SystemState::LISTENING_FOR_COMMAND) {
                            std::cout << "💭 [INTERIM] " << transcript << "\r" << std::flush;
                        }
                    }
                }
            }
        }
        
        grpc::Status status = streamer->Finish();
        if (!status.ok()) {
            std::cerr << "❌ [ERROR] Stream finished with error: " << status.error_message() << std::endl;
        }
    }
    
public:
    BMWVoiceControlClient(std::shared_ptr<grpc::Channel> channel)
        : asr_stub_(nvidia::riva::asr::RivaSpeechRecognition::NewStub(channel)),
          tts_client_(std::make_unique<RivaTTSClient>(channel, API_KEY)),
          state_(SystemState::WAITING_FOR_WAKE_WORD),
          should_exit_(false)
    {
        commandProcessor_ = std::make_unique<CommandProcessor>(vehicle_, *tts_client_);
    }
    
    int Run(const std::string& audio_device) {
        snd_pcm_t* alsa_handle = nullptr;
        
        if (!OpenAudioDevice(
                audio_device.c_str(), &alsa_handle, SND_PCM_STREAM_CAPTURE,
                CHANNELS, SAMPLE_RATE, 100000)) {
            std::cerr << "Error opening capture device " << audio_device << std::endl;
            return 1;
        }
        
        std::cout << "🎙️  Using audio device: " << audio_device << std::endl;
        
        grpc::ClientContext context;
        context.AddMetadata("authorization", "Bearer " + API_KEY);
        context.AddMetadata("function-id", "1598d209-5e27-4d3c-8079-4751568b1081");
        
        auto streamer = asr_stub_->StreamingRecognize(&context);
        
        // Send initial configuration
        nvidia::riva::asr::StreamingRecognizeRequest request;
        auto* streaming_config = request.mutable_streaming_config();
        streaming_config->set_interim_results(true);
        
        auto* config = streaming_config->mutable_config();
        config->set_sample_rate_hertz(SAMPLE_RATE);
        config->set_language_code("en-US");
        config->set_encoding(nvidia::riva::LINEAR_PCM);
        config->set_max_alternatives(1);
        config->set_profanity_filter(false);
        config->set_audio_channel_count(1);
        config->set_enable_word_time_offsets(false);
        config->set_enable_automatic_punctuation(true);
        
	streamer->Write(request);

	// Start threads - BOTH share the same streamer by reference
	std::thread mic_thread(&BMWVoiceControlClient::MicrophoneThreadMain, 
		               this, std::ref(streamer), alsa_handle);
		               
	std::thread response_thread(&BMWVoiceControlClient::ResponseThreadMain, 
		                    this, std::ref(streamer));

	response_thread.join();
	should_exit_ = true;
	mic_thread.join();
        
        CloseAudioDevice(&alsa_handle);
        
        std::cout << "\n👋 Exiting BMW Voice Control System" << std::endl;
        return 0;
    }
    
    void Stop() {
        should_exit_ = true;
    }
};

// Global pointer for signal handler
BMWVoiceControlClient* g_client = nullptr;

void signal_handler(int signal_num) {
    std::cout << "\n⚠️  Received interrupt signal, stopping..." << std::endl;
    if (g_client) {
        g_client->Stop();
    }
}

int main(int argc, char** argv) {
    std::string audio_device = "default";
    
    if (argc > 1) {
        audio_device = argv[1];
    }
    
    std::signal(SIGINT, signal_handler);
    
    grpc::SslCredentialsOptions ssl_opts;
    auto creds = grpc::SslCredentials(ssl_opts);
    grpc::ChannelArguments args;
    auto channel = grpc::CreateCustomChannel(RIVA_URL, creds, args);
    
    BMWVoiceControlClient client(channel);
    g_client = &client;
    
    int result = client.Run(audio_device);
    
    g_client = nullptr;
    return result;
}


/*```

## 🎯 Key Changes - Capito!

### ✅ What's New:
1. **Riva TTS integrated** - `RivaTTSClient` class added
2. **Only 3 commands** for testing:
   - ✅ "open windows" → "Opening windows"
   - ✅ "close windows" → "Closing windows"
   - ✅ "turn on air conditioning" → "Turning on air conditioning"
3. **TTS speaks BEFORE action** - Feedback then execution
4. **Anti-feedback protection** - Mic pauses during TTS (new state: `SPEAKING_FEEDBACK`)
5. **Audio saved to files** - `tts_feedback_0.wav`, `tts_feedback_1.wav`, etc.

### 🔄 Workflow:
```
1. Say "Hi BMW" → Wake word detected
2. Say "open windows" → Command recognized
3. TTS: "Opening windows" (mic paused)
4. Execute: vehicle.openWindows()
5. Back to waiting for "Hi  harris"*/
