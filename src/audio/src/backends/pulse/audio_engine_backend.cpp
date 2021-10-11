#include "audio_engine_backend.hpp"

#include <cstdlib>
#include <pulse/context.h>
#include <pulse/mainloop-api.h>
#include <pulse/mainloop.h>
#include <pulse/stream.h>
#include <thread>

#include <macros/macros.hpp>

namespace audio {

class AudioEngineBackend::Impl {
  private:
    pa_mainloop* fpMainloop;
    pa_context* fpContext;
    pa_stream* fpStream;
    std::thread* fpRunThread;

    bool fStarted = false;

    // void connect() {
    //     pa_sample_spec sampleSpec;
    //     sampleSpec.format   = PA_SAMPLE_S16LE;   // @todo understand this shit
    //     sampleSpec.channels = 2u;
    //     sampleSpec.rate     = 48000;

    //     fpStream = pa_stream_new(fpContext, "test.sound", &sampleSpec, nullptr);
    //     pa_stream_connect_playback(fpStream, nullptr, nullptr,
    //                                static_cast<pa_stream_flags_t>(0u), nullptr,
    //                                nullptr);
    // }

    // void disconnect() {
    //     pa_stream_disconnect(fpStream);
    //     pa_stream_unref(fpStream);
    // }

    void waitForContextToConnect() {
        auto timeLimit = time(nullptr) + 5;
        while (timeLimit > time(nullptr)) {
            pa_mainloop_iterate(fpMainloop, 0, nullptr);
            if (PA_CONTEXT_READY == pa_context_get_state(fpContext)) { return; }
        }
        std::exit(1);
    }

  public:
    Impl() {
        fpMainloop = pa_mainloop_new();

        fpContext = pa_context_new(pa_mainloop_get_api(fpMainloop), "audio blabla");
        pa_context_connect(fpContext, nullptr, static_cast<pa_context_flags_t>(0u),
                           nullptr);

        waitForContextToConnect();

        // connect();
    }

    ~Impl() {
        // disconnect();

        pa_context_disconnect(fpContext);
        pa_context_unref(fpContext);
        fpContext = nullptr;

        stop();

        pa_mainloop_free(fpMainloop);
        fpMainloop = nullptr;
    }

    void start() {
        if (!fStarted) {
            fpRunThread = new std::thread(pa_mainloop_run, fpMainloop, nullptr);
            fStarted    = true;
        }
    }

    void stop() {
        if (fStarted) {
            pa_mainloop_quit(fpMainloop, PA_OK);

            fpRunThread->join();
            delete fpRunThread;
            fpRunThread = nullptr;
            fStarted    = false;
        }
    }

    void internalUpdate() { pa_mainloop_iterate(fpMainloop, 0, nullptr); }

    pa_mainloop* mainloop() { return fpMainloop; }

    pa_context* context() { return fpContext; }
};

$pimpl_class(AudioEngineBackend);
$pimpl_class_delete(AudioEngineBackend);

$pimpl_method(AudioEngineBackend, void, start);
$pimpl_method(AudioEngineBackend, void, stop);

$pimpl_method(AudioEngineBackend, pa_mainloop*, mainloop);
$pimpl_method(AudioEngineBackend, pa_context*, context);

$pimpl_method(AudioEngineBackend, void, internalUpdate);

}   // namespace audio
