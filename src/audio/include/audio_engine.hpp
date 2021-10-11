#pragma once

#include <memory>

// #include <audio/audio_backend.hpp>
#include <audio/audio_source.hpp>
#include <macros/macros.hpp>

namespace audio {

class AudioEngine {
  public:
    // AudioEngine() = delete;
    // AudioEngine(AudioBackend&);
    AudioEngine();
    ~AudioEngine();

    void start();
    void stop();

    void update();

    void add(std::unique_ptr<AudioSource>&& source);

    template<class T, class... Arguments>
    T& make(Arguments&&... arguments);

    template<class T, class... Arguments>
    std::shared_ptr<T> share(Arguments&&... arguments);

  public:
    class Backend;
    Backend& backend() { return *fpBackend; }

  private:
    std::unique_ptr<Backend> fpBackend;
};

}   // namespace audio

#include "audio_engine.tpp"
