#include <memory>

#include <audio/audio_source.hpp>

#include "audio_source_backend.hpp"

namespace audio {

// AudioSource::AudioSource(AudioEngine& engine) {
//     fpBackend = std::make_unique<AudioSource::Backend>(engine.backend());
// }

// AudioSource::~AudioSource() = default;

// AudioSource::AudioSource(AudioEngine& engine) {
//     fpBackend = std::make_unique<AudioSource::Backend>(engine.backend());
// }

// AudioSource::~AudioSource() = default;

void AudioSource::play() { fpBackend->playing(true); }

void AudioSource::stop() { fpBackend->playing(false); }

void AudioSource::looping(bool looping) { fpBackend->looping(looping); }
bool AudioSource::looping() const { return fpBackend->looping(); }

// void AudioSource::playOnce()
// {
//     m_impl->removeOnFinish(true);
//     m_impl->playing(true);
// }

}   // namespace audio
