#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>

#include <audio/sound_data.hpp>
#include <logging/logger.hpp>

namespace audio {

namespace {
inline size_t formatToByteCount(SampleFormat format) {
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

// @todo move these elsewhere
inline int16_t readInt16LE(const uint8_t* buffer, uint32_t offset) {
    int16_t value = (buffer[offset + 1u] << 8u) + buffer[offset + 0u];
    return value;
}

inline uint16_t readUint16LE(const uint8_t* buffer, uint32_t offset) {
    uint16_t value = (buffer[offset + 1u] << 8u) + buffer[offset + 0u];
    return value;
}

inline uint32_t readUint32LE(const uint8_t* buffer, uint32_t offset) {
    uint32_t value = (buffer[offset + 3u] << 24u) + (buffer[offset + 2u] << 16u)
                     + (buffer[offset + 1u] << 8u) + buffer[offset + 0u];
    return value;
}

float readAsFloat(const uint8_t* buffer, uint32_t offset, SampleFormat sampleFormat) {
    switch (sampleFormat) {
    case SampleFormat::Int16LE: return readInt16LE(buffer, offset) / 32768.f;
    default: {
        logging::logger.error("audio.sound_data")
            << "Unhandled sample format: " << sampleFormat << std::endl;
    }
    }

    return 0.f;
}

uint32_t sampleFormatBytesCount(SampleFormat sampleFormat) {
    switch (sampleFormat) {
    case SampleFormat::Int16LE: return sizeof(int16_t); break;
    default: {
        logging::logger.error("audio.sound_data")
            << "Unhandled sample format: " << sampleFormat << std::endl;
    }
    }

    return 0u;
}

}   // namespace

class SoundData::Impl {
  protected:
    SampleFormat fSampleFormat;
    uint8_t fChannels;
    uint32_t fRate;

    uint32_t fSampleCount;
    uint8_t fBytesPerSample;
    size_t fBufferSize;
    uint8_t* fpData;

    std::vector<float> fNormalizedData;

  public:
    Impl()          = default;
    virtual ~Impl() = default;

    uint8_t* data() { return fpData; }
    const uint8_t* data() const { return fpData; }

    /// The size of the raw data (in bytes).
    uint32_t size() const { return fBufferSize; }
    uint32_t sampleNumber() const { return fBufferSize / (fBytesPerSample); }

    /// Sample rate (in Hz).
    uint32_t rate() const { return fRate; }

    /// Sample channels (1 for mono, 2 for stereo and more if needed).
    uint8_t channels() const { return fChannels; }

    uint8_t bytesPerSample() const {
        return formatToByteCount(fSampleFormat) * fChannels;
    }

    /// Sample format.
    SampleFormat sampleFormat() const { return fSampleFormat; }

    /// The raw data converted to single channel, 44100Hz and 32-bit float PCM.
    const float* normalizedData() const {
        assert(false && "Not implemented yet");
        return nullptr;   // @fixit
    }

    /// The size of normalizedData (floats count).
    uint32_t normalizedSize() const {
        assert(false && "Not implemented yet");
        return 0;   // @fixit
    }

    void normalize() {
        if (fBufferSize == 0u || fChannels == 0u) {
            fNormalizedData.resize(0u);
            return;
        }

        const auto sampleFormatSize  = sampleFormatBytesCount(fSampleFormat);
        const auto samplesPerChannel = fBufferSize / sampleFormatSize / fChannels;
        fNormalizedData.resize(samplesPerChannel);

        // ----- Convert to float and average to mono

        // @note Averaging is the usual method,
        // even if it may cause problems in very specific cases
        // (L and R signals in opposite phases), it is good enough.
        for (auto s = 0u; s < samplesPerChannel; ++s) {
            // Compute average on interleaved channels
            auto averageValue = 0.f;
            for (auto c = 0u; c < fChannels; ++c) {
                averageValue += readAsFloat(
                    fpData, (fChannels * s + c) * sampleFormatSize, fSampleFormat);
            }
            averageValue /= fChannels;

            // Set it to the normalized data
            fNormalizedData[s] = averageValue;
        }

        // ----- @fixme Resample to 44100

        if (fRate != 44100) {
            logging::logger.error("flow.sound-data")
                << "Currently, only sounds with a rate of 44100Hz are handled."
                << std::endl;
        }
    }
};

class FileSoundData::Impl : public SoundData::Impl {
  private:
    std::vector<uint8_t> fFileData;

  public:
    Impl(const std::string& fileName) {
        logging::logger.info("audio.sound_data").tab(1)
            << "Reading '" << fileName << "'." << std::endl;
        logging::logger.log().tab(1);

        std::ifstream file(fileName, std::ifstream::binary);

        if (!file.is_open()) {
            logging::logger.error("audio.sound_data") << "Cannot open file." << std::endl;
        }

        fFileData.insert(fFileData.begin(), std::istreambuf_iterator<char>(file),
                         std::istreambuf_iterator<char>());

        // Check header
        if ((fFileData.size() < 44u)
            || (fFileData[0] != 'R' || fFileData[1] != 'I' || fFileData[2] != 'F'
                || fFileData[3] != 'F')
            || (fFileData[8] != 'W' || fFileData[9] != 'A' || fFileData[10] != 'V'
                || fFileData[11] != 'E')) {
            logging::logger.error("audio.sound_data")
                << "File is not a Wave one." << std::endl;
        }

        // Check format
        auto audioFormat = readUint16LE(fFileData.data(), 20);
        if (audioFormat != 1u) {
            logging::logger.error("audio.sound_data")
                << "Unknown audio format " << audioFormat << "." << std::endl;
        }

        // Sample specification
        fChannels = readUint16LE(fFileData.data(), 22);
        fRate     = readUint32LE(fFileData.data(), 24);

        auto bitsPerSample = readUint16LE(fFileData.data(), 34);
        switch (bitsPerSample) {
        case 8u: fSampleFormat = SampleFormat::Uint8; break;
        case 16u: fSampleFormat = SampleFormat::Int16LE; break;
        case 32u: fSampleFormat = SampleFormat::Int32LE; break;
        default:
            logging::logger.error("audio.sound_data")
                << "Unknown bitsPerSample value." << std::endl;
        }

        logging::logger.log() << "Channels: " << static_cast<uint32_t>(fChannels)
                              << std::endl;
        logging::logger.log() << "Rate: " << fRate << std::endl;
        logging::logger.log() << "Sample format: " << fSampleFormat << std::endl;

        // Set-up data region
        // @note This 44 is the size of the header of Wave format.
        fpData      = fFileData.data() + 44u;
        fBufferSize = fFileData.size() - 44u;

        logging::logger.log().tab(-2);

        normalize();
    }
};

class RawSoundData::Impl : public SoundData::Impl {
  private:
  public:
    Impl(SampleFormat format, uint8_t channels, uint32_t rate, uint32_t sampleCound)
        : SoundData::Impl() {
        fSampleFormat = format;
        fChannels     = channels;
        fRate         = rate;
        fSampleCount  = sampleCound;
        if (fSampleCount == 0) fSampleCount = rate;
        fBytesPerSample = formatToByteCount(fSampleFormat) * fChannels;
        fBufferSize     = fBytesPerSample * fSampleCount;
        fpData          = new uint8_t[fBufferSize];
    }

    ~Impl() { delete[] fpData; }

    /// The raw data (encoded interlaced according to sampleFormat).

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
};

// // Pimpl declarations

SoundData::SoundData() : fpImpl(std::make_shared<Impl>()) {}
SoundData::SoundData(std::shared_ptr<Impl> pImpl) : fpImpl(pImpl) {}
$pimpl_class_delete(SoundData);

$pimpl_method(SoundData, uint8_t*, data);
$pimpl_method_const(SoundData, const uint8_t*, data);
$pimpl_method_const(SoundData, uint32_t, sampleNumber);
$pimpl_method_const(SoundData, uint32_t, size);
$pimpl_method_const(SoundData, uint32_t, rate);
$pimpl_method_const(SoundData, uint8_t, channels);
$pimpl_method_const(SoundData, uint8_t, bytesPerSample);
$pimpl_method_const(SoundData, SampleFormat, sampleFormat);
$pimpl_method_const(SoundData, const float*, normalizedData);
$pimpl_method_const(SoundData, uint32_t, normalizedSize);

RawSoundData::RawSoundData(SampleFormat format, uint8_t channels, uint32_t rate,
                           uint32_t length)
    : SoundData(std::make_shared<Impl>(format, channels, rate, length)) {}

$pimpl_class_delete(RawSoundData);

// $pimpl_method(RawSoundData, void, resize, uint32_t, newSampleCount);
void RawSoundData::resize(uint32_t newSampleCount) {
    dynamic_cast<Impl*>(fpImpl.get())->resize(newSampleCount);
}

}   // namespace audio
