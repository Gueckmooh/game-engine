#pragma once

#include <audio/audio_source.hpp>

#include "audio_engine_backend.hpp"

namespace audio {
class AudioSource::Backend {
  public:
    Backend(AudioEngine::Backend& engine);
    virtual ~Backend();

    virtual void update() = 0;

    virtual void restart() = 0;

    void playing(bool);
    bool playing() const;

    void looping(bool);
    bool looping() const;

    void removeOnFinish(bool);
    bool removeOnFinish() const;

  protected:
    void finish();

  private:
    $pimpl_decl;
};
}   // namespace audio
