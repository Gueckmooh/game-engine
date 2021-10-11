#pragma once

#include <audio/audio_engine.hpp>
#include <audio/audio_source.hpp>

namespace audio {

class AudioEngineBackend;

class AudioEngine::Backend {
  public:
    Backend();
    ~Backend();

    virtual void start() = 0;
    virtual void stop()  = 0;

    void update();

    void add(std::unique_ptr<AudioSource>&& source);

    AudioEngineBackend& backend() { return reinterpret_cast<AudioEngineBackend&>(*this); }
    const AudioEngineBackend& backend() const {
        return reinterpret_cast<const AudioEngineBackend&>(*this);
    }

  protected:
    virtual void internalUpdate() = 0;

  private:
    $pimpl_decl;
};
}   // namespace audio
