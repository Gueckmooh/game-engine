#pragma once

#include <audio/audio_source.hpp>
#include <audio/sound.hpp>
#include <audio/sound_data.hpp>

#include "../../audio_source_backend.hpp"
#include "../../sound_backend.hpp"
#include "audio_engine_backend.hpp"
namespace audio {
/**
 * PulseAudio implementation of sound class.
 */
class SoundBackend : public AudioSource::Backend {
  public:
    SoundBackend(AudioEngine::Backend& engine,
                 const std::shared_ptr<SoundData>& soundData);
    ~SoundBackend();

    // ----- AudioSource

    void update();
    void restart();

  private:
    $pimpl_decl;   // @todo
};
}   // namespace audio
