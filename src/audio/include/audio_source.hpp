#pragma once

// @todo implement this

#include <memory>

namespace audio {
class AudioEngine;

/**
 * Base for playable audio.
 */
class AudioSource {
  public:
    // AudioSource(AudioEngine& engine);
    // ~AudioSource() = default;
    // AudioSource(AudioEngine& engine);
    // ~AudioSource();
    /// Plays the current sound data from the start.
    void play();

    /// Stops anything that is playing.
    void stop();

    /// Plays the current sound data once and remove this sound.
    // void playOnce();

    /**
     * Whether the sound should be replayed at the end.
     * Not effective if playOnce is called.
     */
    void looping(bool looping);
    bool looping() const;

  public:
    class Backend;
    Backend& backend() { return *fpBackend; }

  protected:
    Backend* fpBackend = nullptr;
};
}   // namespace audio
