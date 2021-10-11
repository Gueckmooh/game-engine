
#include "sound_backend.hpp"

#include <pulse/stream.h>

#include <audio/sound_data.hpp>

#include "audio_engine_backend.hpp"
#include "utils.hpp"

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

        fpStream =
            pa_stream_new(fEngineBackend.context(), "audio.sound", &sampleSpec, nullptr);
        pa_stream_connect_playback(fpStream, nullptr, nullptr,
                                   static_cast<pa_stream_flags_t>(0u), nullptr, nullptr);
    }

    ~Impl() {
        pa_stream_disconnect(fpStream);
        pa_stream_unref(fpStream);
    }

    void finish() { super->finish(); }

    void update() {
        if (pa_stream_get_state(fpStream) != PA_STREAM_READY) return;

        const auto writableSize  = pa_stream_writable_size(fpStream);
        const auto remainingSize = (fpSoundData->size() - fPlayingOffset);
        uint32_t playingSize =
            (remainingSize < writableSize) ? remainingSize : writableSize;

        if (playingSize >= 512u || (playingSize > 0u && playingSize == remainingSize)) {
            const auto soundPointer = fpSoundData->data() + fPlayingOffset;
            const auto size         = playingSize;

            pa_stream_write(fpStream, soundPointer, size, nullptr, 0, PA_SEEK_RELATIVE);
            fPlayingOffset += size;
        }

        if (remainingSize == 0u && pa_stream_get_underflow_index(fpStream) >= 0) {
            finish();
        }
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
