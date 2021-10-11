
#include "sound_backend.hpp"

#include <dsound.h>

#include <audio/sound_data.hpp>

#include "audio_engine_backend.hpp"
#include "utils.hpp"

namespace audio {

class SoundBackend::Impl {
    SoundBackend* super;

    AudioEngineBackend& fEngineBackend;
    const std::shared_ptr<SoundData> fpSoundData;

    uint32_t fPlayingOffset = 0;

    // pa_stream* fpStream;

  private:
  public:
    Impl(SoundBackend* super, AudioEngine::Backend& engine,
         const std::shared_ptr<SoundData>& soundData)
        : super(super), fEngineBackend(engine.backend()), fpSoundData(soundData) {
        WAVEFORMATEX WaveFormat   = {};
        WaveFormat.wFormatTag     = WAVE_FORMAT_PCM;
        WaveFormat.nChannels      = soundData->channels();
        WaveFormat.nSamplesPerSec = soundData->rate();
        WaveFormat.wBitsPerSample = utils::formatToBitCount(soundData->sampleFormat());
        WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
        WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
        WaveFormat.cbSize          = 0;

        HRESULT Error = fEngineBackend.primaryBuffer()->SetFormat(&WaveFormat);
        if (SUCCEEDED(Error)) {
            // NOTE(casey): We have finally set the format!
            OutputDebugStringA("Primary buffer format was set.\n");
        } else {
            // TODO(casey): Diagnostic
        }

        DSBUFFERDESC BufferDescription  = {};
        BufferDescription.dwSize        = sizeof(BufferDescription);
        BufferDescription.dwFlags       = 0;
        BufferDescription.dwBufferBytes = soundData->size();
        BufferDescription.lpwfxFormat   = &WaveFormat;
        auto secondaryBuffer            = fEngineBackend.secondaryBuffer();
        Error = fEngineBackend.directSound()->CreateSoundBuffer(&BufferDescription,
                                                                &secondaryBuffer, 0);
        if (SUCCEEDED(Error)) {
            OutputDebugStringA("Secondary buffer created successfully.\n");
        }
    }

    ~Impl() {
        // pa_stream_disconnect(fpStream);
        // pa_stream_unref(fpStream);
    }

    void finish() { super->finish(); }

    void update() {
        // if (pa_stream_get_state(fpStream) != PA_STREAM_READY) return;

        // const auto writableSize  = pa_stream_writable_size(fpStream);
        // const auto remainingSize = (fpSoundData->size() - fPlayingOffset);
        // uint32_t playingSize =
        //     (remainingSize < writableSize) ? remainingSize : writableSize;

        // if (playingSize >= 512u || (playingSize > 0u && playingSize == remainingSize))
        // {
        //     const auto soundPointer = fpSoundData->data() + fPlayingOffset;
        //     const auto size         = playingSize;

        //     pa_stream_write(fpStream, soundPointer, size, nullptr, 0,
        //     PA_SEEK_RELATIVE); fPlayingOffset += size;
        // }

        // if (remainingSize == 0u && pa_stream_get_underflow_index(fpStream) >= 0) {
        //     finish();
        // }
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
