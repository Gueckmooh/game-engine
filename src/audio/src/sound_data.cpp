#include <cstring>

#include <audio/sound_data.hpp>

namespace audio {

class RawSoundData::Impl {
  private:
    SampleFormat fFormat;
    uint8_t fChannels;
    uint32_t fRate;

    uint32_t fSampleCount;
    uint8_t fBytesPerSample;
    size_t fBufferSize;
    uint8_t* fpData;

  public:
    Impl(SampleFormat format, uint8_t channels, uint32_t rate, uint32_t nSample)
        : fFormat(format), fChannels(channels), fRate(rate), fSampleCount(nSample) {
        if (fSampleCount == 0) fSampleCount = rate;
        fBytesPerSample = formatToByteCount(fFormat) * fChannels;
        fBufferSize     = fBytesPerSample * fSampleCount;
        fpData          = new uint8_t[fBufferSize];
    }

    ~Impl() { delete[] fpData; }

    /// The raw data (encoded interlaced according to sampleFormat).
    uint8_t* data() { return fpData; }
    const uint8_t* data() const { return fpData; }

    /// The size of the raw data (in bytes).
    uint32_t size() const { return fBufferSize; }
    uint32_t sampleNumber() const { return fBufferSize / (fBytesPerSample); }

    /// Sample rate (in Hz).
    uint32_t rate() const { return fRate; }

    /// Sample channels (1 for mono, 2 for stereo and more if needed).
    uint8_t channels() const { return fChannels; }

    uint8_t bytesPerSample() const { return formatToByteCount(fFormat) * fChannels; }

    /// Sample format.
    SampleFormat sampleFormat() const { return fFormat; }

    /// The raw data converted to single channel, 44100Hz and 32-bit float PCM.
    const float* normalizedData() const {
        return nullptr;   // @fixit
    }

    /// The size of normalizedData (floats count).
    uint32_t normalizedSize() const {
        return 0;   // @fixit
    }

    void resize(uint32_t newSampleCount) {
        // @todo make this shit work
        uint32_t newBufferSize = fBytesPerSample * newSampleCount;
        uint8_t* newData       = new uint8_t[newBufferSize];
        memcpy(newData, fpData,
               newBufferSize < fBufferSize ? newBufferSize : fBufferSize);
        fBufferSize = newBufferSize;
        delete[] fpData;
        fpData = newData;
    }

  private:
    inline size_t formatToByteCount(SampleFormat format) const {
        switch (format) {
        case SampleFormat::Uint8: return sizeof(uint8_t);
        case SampleFormat::Int16BE: return sizeof(int16_t);
        case SampleFormat::Int16LE: return sizeof(int16_t);
        case SampleFormat::Int32BE: return sizeof(int32_t);
        case SampleFormat::Int32LE: return sizeof(int32_t);
        case SampleFormat::Float32: return sizeof(float);
        case SampleFormat::Unknown: return 0;
        }
        return 0;
    }
};

// // Pimpl declarations
$pimpl_class(RawSoundData, SampleFormat, format, uint8_t, channels, uint32_t, rate,
             uint32_t, length);
$pimpl_class_delete(RawSoundData);

$pimpl_method(RawSoundData, uint8_t*, data);
$pimpl_method_const(RawSoundData, const uint8_t*, data);
$pimpl_method_const(RawSoundData, uint32_t, sampleNumber);
$pimpl_method_const(RawSoundData, uint32_t, size);
$pimpl_method_const(RawSoundData, uint32_t, rate);
$pimpl_method_const(RawSoundData, uint8_t, channels);
$pimpl_method_const(RawSoundData, uint8_t, bytesPerSample);
$pimpl_method_const(RawSoundData, SampleFormat, sampleFormat);
$pimpl_method_const(RawSoundData, const float*, normalizedData);
$pimpl_method_const(RawSoundData, uint32_t, normalizedSize);
$pimpl_method(RawSoundData, void, resize, uint32_t, newSampleCount);

}   // namespace audio
