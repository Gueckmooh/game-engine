#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <audio/sample_format.hpp>
#include <macros/macros.hpp>

namespace audio {

class SoundData {
  public:
    virtual ~SoundData();

    /// The raw data (encoded interlaced according to sampleFormat).
    virtual const uint8_t* data() const;
    virtual uint8_t* data();

    /// The size of the raw data (in bytes).
    virtual uint32_t size() const;
    virtual uint32_t sampleNumber() const;

    /// Sample rate (in Hz).
    virtual uint32_t rate() const;

    /// Sample channels (1 for mono, 2 for stereo and more if needed).
    virtual uint8_t channels() const;

    virtual uint8_t bytesPerSample() const;

    /// Sample format.
    virtual SampleFormat sampleFormat() const;

    /// The raw data converted to single channel, 44100Hz and 32-bit float PCM.
    virtual const float* normalizedData() const;

    /// The size of normalizedData (floats count).
    virtual uint32_t normalizedSize() const;

  protected:
    SoundData();
    class Impl;

    SoundData(std::shared_ptr<Impl>);
    std::shared_ptr<Impl> fpImpl;
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

//   private:
//     $pimpl_decl;
// };

class RawSoundData : public SoundData {
  public:
    RawSoundData(SampleFormat format, uint8_t channels, uint32_t rate,
                 uint32_t sampleCound = 0);
    ~RawSoundData();

    void resize(uint32_t);

  private:
    class Impl;
};

}   // namespace audio
