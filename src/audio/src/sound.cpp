#include <memory>

#include <audio/audio_engine.hpp>
#include <audio/audio_source.hpp>
#include <audio/sound.hpp>
#include <audio/sound_data.hpp>

#include "audio_source_backend.hpp"
#include "backends/pulse/sound_backend.hpp"

namespace audio {

Sound::Sound(AudioEngine& engine, const std::shared_ptr<SoundData>& soundData) {
    fpBackend = std::make_unique<SoundBackend>(engine.backend(), soundData);
}

// Sound::Sound(AudioEngine& engine, const std::string& fileName)
//     : Sound(engine, engine.share<SoundData>(fileName)) {}

Sound::~Sound() {}

// $pimpl_method_cast(Sound, SoundBaseImpl, void, position, const glm::vec3&, position);
// $pimpl_property_cast_v(Sound, SoundBaseImpl, float, cutOffDistance);
// $pimpl_property_cast_v(Sound, SoundBaseImpl, float, spatializationHalfDistance);

}   // namespace audio

   // namespace audio
