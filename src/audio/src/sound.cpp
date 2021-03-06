#include <memory>

#include <audio/audio_engine.hpp>
#include <audio/audio_source.hpp>
#include <audio/sound.hpp>
#include <audio/sound_data.hpp>

#include "audio_source_backend.hpp"

#ifdef __USE_WINDOWS__
#    include "backends/dsound/sound_backend.hpp"
#else
#    include "backends/pulse/sound_backend.hpp"
#endif

namespace audio {

Sound::Sound(AudioEngine& engine, const std::shared_ptr<SoundData>& soundData) {
    fpBackend = new SoundBackend(engine.backend(), soundData);
}

// Sound::Sound(AudioEngine& engine, const std::string& fileName)
//     : Sound(engine, engine.share<SoundData>(fileName)) {}

Sound::~Sound() { delete fpBackend; }

// $pimpl_method_cast(Sound, SoundBaseImpl, void, position, const glm::vec3&, position);
// $pimpl_property_cast_v(Sound, SoundBaseImpl, float, cutOffDistance);
// $pimpl_property_cast_v(Sound, SoundBaseImpl, float, spatializationHalfDistance);

}   // namespace audio

   // namespace audio
