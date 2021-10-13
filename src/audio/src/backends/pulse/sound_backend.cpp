
#include "sound_backend.hpp"

#include <logging/logger.hpp>
#include <pulse/stream.h>

#include <audio/sound_data.hpp>

#include "audio_engine_backend.hpp"
#include "utils.hpp"

// namespace {

// static int latency    = 0;   // start latency in micro seconds
// static int sampleoffs = 0;
// static short sampledata[300000];
// static pa_buffer_attr bufattr;
// static int underflows = 0;
// static pa_sample_spec ss;
// static void stream_underflow_cb(pa_stream* s, void* userdata) {
//     // We increase the latency by 50% if we get 6 underflows and latency is under 2s
//     // This is very useful for over the network playback that can't handle low
//     latencies printf("underflow\n"); underflows++; if (underflows >= 6 && latency <
//     2000000) {
//         latency           = (latency * 3) / 2;
//         bufattr.maxlength = pa_usec_to_bytes(latency, &ss);
//         bufattr.tlength   = pa_usec_to_bytes(latency, &ss);
//         pa_stream_set_buffer_attr(s, &bufattr, NULL, NULL);
//         underflows = 0;
//         printf("latency increased to %d\n", latency);
//     }
// }
// }   // namespace

namespace audio {

class SoundBackend::Impl {
    SoundBackend* super;

    AudioEngineBackend& fEngineBackend;
    const std::shared_ptr<SoundData> fpSoundData;

    uint32_t fPlayingOffset = 0;

    pa_stream* fpStream;

  private:
  public:
    Impl(SoundBackend* super, AudioEngine::Backend& engine,
         const std::shared_ptr<SoundData>& soundData)
        : super(super), fEngineBackend(engine.backend()), fpSoundData(soundData) {
        pa_sample_spec sampleSpec;
        sampleSpec.format   = utils::formatToByteCount(fpSoundData->sampleFormat());
        sampleSpec.channels = fpSoundData->channels();
        sampleSpec.rate     = fpSoundData->rate();

        if (!pa_sample_spec_valid(&sampleSpec)) {
            logging::logger.error("sound.backend.init")
                << "pa_sample_spec is not valid" << std::endl;
        }

        fpStream =
            pa_stream_new(fEngineBackend.context(), "audio.sound", &sampleSpec, nullptr);

        int latency            = 32000;   // start latency in micro seconds
        pa_buffer_attr bufattr = {
            .maxlength = (uint32_t)-1,
            .tlength   = (uint32_t)pa_usec_to_bytes(latency, &sampleSpec),
            .prebuf    = (uint32_t)-1,
            .minreq    = (uint32_t)-1,
            .fragsize  = (uint32_t)-1,
        };
        pa_stream_flags flags = PA_STREAM_ADJUST_LATENCY;
        pa_stream_connect_playback(fpStream, nullptr, &bufattr, flags, nullptr, nullptr);

        logging::logger.info("sound.backend.init")
            << "stream created successfully with latency = " << latency / 1000 << "ms"
            << std::endl;
    }

    ~Impl() {
        pa_stream_disconnect(fpStream);
        pa_stream_unref(fpStream);
    }

    void finish() { super->finish(); }

    void update() {
        if (pa_stream_get_state(fpStream) != PA_STREAM_READY) { return; }
        logging::logger.info("sound.backend.update")
            << "sound backend update" << std::endl;
        logging::logger.log().tab(1);

        const auto writableSize = pa_stream_writable_size(fpStream);
        auto remainingSize      = (fpSoundData->size() - fPlayingOffset);
        uint32_t playingSize =
            (remainingSize < writableSize) ? remainingSize : writableSize;

        if (remainingSize == 0u   // && pa_stream_get_underflow_index(fpStream) >= 0
        ) {
            finish();
            remainingSize = (fpSoundData->size() - fPlayingOffset);
            playingSize   = (remainingSize < writableSize) ? remainingSize : writableSize;
        }

        logging::logger.log() << "writable size: " << writableSize << std::endl;
        logging::logger.log() << "remaining size: " << remainingSize << std::endl;
        logging::logger.log() << "playing size: " << playingSize << std::endl;

        // @todo find the right value
        if (playingSize > 4000) {
            playingSize = 4000;
            logging::logger.log()
                << "playing size is forced to: " << playingSize << std::endl;
        }

        logging::logger.log().tab(1);
        logging::logger.log() << "playing offset is: " << fPlayingOffset << std::endl;

        if (playingSize >= 512u || (playingSize > 0u && playingSize == remainingSize)) {
            const auto soundPointer = fpSoundData->data() + fPlayingOffset;
            const auto size         = playingSize;

            pa_stream_write(fpStream, soundPointer, size, nullptr, 0, PA_SEEK_RELATIVE);
            fPlayingOffset += size;
            logging::logger.log()
                << "playing offset now is: " << fPlayingOffset << std::endl;
        }

        logging::logger.log().tab(-2);
    }

    void restart() { fPlayingOffset = 0u; }
};

// $pimpl_class(SoundBackend, AudioEngine::Backend&, engine,
//              const std::shared_ptr<SoundData>&, soundData);

SoundBackend::SoundBackend(AudioEngine::Backend& engine,
                           const std::shared_ptr<SoundData>& soundData)
    : Backend(engine), fpImpl(std::make_unique<Impl>(this, engine, soundData)) {}
$pimpl_class_delete(SoundBackend);

$pimpl_method(SoundBackend, void, update);
$pimpl_method(SoundBackend, void, restart);

}   // namespace audio
