#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <audio/sample_format.hpp>
#include <macros/macros.hpp>

namespace audio {

class SoundData {
  public:
    // SoundData(const std::string& fileName);
    virtual ~SoundData(){}

    /// The raw data (encoded interlaced according to sampleFormat).
    virtual const uint8_t* data() const = 0;
    virtual uint8_t* data()             = 0;

    /// The size of the raw data (in bytes).
    virtual uint32_t size() const         = 0;
    virtual uint32_t sampleNumber() const = 0;

    /// Sample rate (in Hz).
    virtual uint32_t rate() const = 0;

    /// Sample channels (1 for mono, 2 for stereo and more if needed).
    virtual uint8_t channels() const = 0;

    virtual uint8_t bytesPerSample() const = 0;

    /// Sample format.
    virtual SampleFormat sampleFormat() const = 0;

    /// The raw data converted to single channel, 44100Hz and 32-bit float PCM.
    virtual const float* normalizedData() const = 0;

    /// The size of normalizedData (floats count).
    virtual uint32_t normalizedSize() const = 0;
};

// class FileSoundData : public SoundData {
//   public:
//     FileSoundData(const std::string& fileName);
//     ~FileSoundData();

//     /// The raw data (encoded interlaced according to sampleFormat).
//     const uint8_t* data() const;

//     /// The size of the raw data (in bytes).
//     uint32_t size() const;

//     /// Sample rate (in Hz).
//     uint32_t rate() const;

//     /// Sample channels (1 for mono, 2 for stereo and more if needed).
//     uint8_t channels() const;

//     /// Sample format.
//     SampleFormat sampleFormat() const;

//     /// The raw data converted to single channel, 44100Hz and 32-bit float PCM.
//     const float* normalizedData() const;

//     /// The size of normalizedData (floats count).
//     uint32_t normalizedSize() const;

//   public:
//     class Impl;
//     Impl& impl() { return *m_impl; }

//   private:
//     Impl* m_impl = nullptr;
// };

class RawSoundData : public SoundData {
  public:
    RawSoundData(SampleFormat format, uint8_t channels, uint32_t rate,
                 uint32_t nSample = 0);
    ~RawSoundData();

    /// The raw data (encoded interlaced according to sampleFormat).
    const uint8_t* data() const;
    uint8_t* data();

    /// The size of the raw data (in bytes).
    uint32_t size() const;
    uint32_t sampleNumber() const;

    /// Sample rate (in Hz).
    uint32_t rate() const;

    /// Sample channels (1 for mono, 2 for stereo and more if needed).
    uint8_t channels() const;

    uint8_t bytesPerSample() const;

    /// Sample format.
    SampleFormat sampleFormat() const;

    /// The raw data converted to single channel, 44100Hz and 32-bit float PCM.
    const float* normalizedData() const;

    /// The size of normalizedData (floats count).
    uint32_t normalizedSize() const;

    void resize(uint32_t);

  private:
    $pimpl_decl;
};

}   // namespace audio
