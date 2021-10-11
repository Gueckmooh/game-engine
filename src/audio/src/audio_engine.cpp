#include <memory>

#include <audio/audio_engine.hpp>

#include "backends/pulse/audio_engine_backend.hpp"

namespace audio {

AudioEngine::AudioEngine() { fpBackend = std::make_unique<AudioEngineBackend>(); }

AudioEngine::~AudioEngine() = default;

void AudioEngine::start() { fpBackend->start(); }

void AudioEngine::stop() { fpBackend->stop(); }

void AudioEngine::update() { fpBackend->update(); }

void AudioEngine::add(std::unique_ptr<AudioSource>&& source) {
    fpBackend->add(std::move(source));
}

}   // namespace audio
