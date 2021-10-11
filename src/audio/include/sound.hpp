#pragma once

#include <memory>
#include <string>

// #include <audio/audio_engine.hpp>
#include <audio/audio_source.hpp>
// #include <audio/sound_data.hpp>

// namespace lava::flow {
//     class AudioEngine;
//     class SoundData;
// }

namespace audio {
class AudioEngine;
class SoundData;

class Sound : public AudioSource {
  public:
    /// Create a new sound based on preexisting SoundData.
    Sound(AudioEngine& engine, const std::shared_ptr<SoundData>& soundData);

    /// Create a new sound and associated SoundData, the data won't be sharable.
    // Sound(AudioEngine& engine, const std::string& fileName);

    ~Sound();

    /**
     * @name Spatialization
     */
    /// @{
    // void position(const glm::vec3& position);

    /// The distance to the listener above which the sound is pure silence. (Default:
    /// 100.f)
    // float cutOffDistance() const;
    // void cutOffDistance(float cutOffDistance);

    /// The distance at which left or right output is exactly half. (Default: 10.f)
    // float spatializationHalfDistance() const;
    // void spatializationHalfDistance(float spatializationHalfDistance);
    /// @}
    // private:
    //   $pimpl_decl;   // @todo
    // public:
    //   class Backend;
    //   Backend& backend() { return *fpBackend; }

    // private:
    //   std::unique_ptr<Backend> fpBackend;
};
}   // namespace audio
