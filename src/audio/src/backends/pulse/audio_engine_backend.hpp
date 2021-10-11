#pragma once

#include <pulse/context.h>
#include <pulse/mainloop.h>

#include "../../audio_engine_backend.hpp"

namespace audio {

class AudioEngineBackend : public AudioEngine::Backend {
  public:
    AudioEngineBackend();
    ~AudioEngineBackend();

    void start();
    void stop();

    pa_mainloop* mainloop();
    pa_context* context();

    void internalUpdate() override final;

  private:
    $pimpl_decl;   // @todo
};

}   // namespace audio
