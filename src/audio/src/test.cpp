#include <cmath>
#include <memory>
#include <pulse/stream.h>

#include <audio/audio_engine.hpp>
#include <audio/audio_source.hpp>
#include <audio/audio_test.hpp>
#include <audio/sample_format.hpp>
#include <audio/sound.hpp>
#include <audio/sound_data.hpp>

namespace audio {

std::shared_ptr<RawSoundData> sine(uint32_t toneHZ, int16_t volume = 16000) {

    uint32_t rate             = 48000;
    uint16_t squareWavePeriod = rate / toneHZ;
    std::cout << squareWavePeriod << std::endl;

    auto rawSoundData = std::make_shared<RawSoundData>(SampleFormat::Int16LE, 2u, rate,
                                                       squareWavePeriod * toneHZ);
    char* buffer      = (char*)rawSoundData->data();
    float tSine       = 0;
    int16_t* cursor   = (int16_t*)buffer;

    int16_t* baseSine = new int16_t[squareWavePeriod];
    for (size_t index = 0; index < squareWavePeriod; ++index) {
        float SineValue     = sinf(tSine);
        int16_t sampleValue = (int16_t)(SineValue * volume);

        baseSine[index] = sampleValue;
        tSine += 2.0f * M_PIf32 * 1.0f / squareWavePeriod;
    }

    for (ssize_t index = 0; index < rawSoundData->sampleNumber(); ++index) {
        auto sampleValue = baseSine[index % squareWavePeriod];
        *cursor++        = sampleValue;
        *cursor++        = sampleValue;
    }

    delete[] baseSine;

    return rawSoundData;
}

void test() {
    AudioEngine en;
    std::shared_ptr<RawSoundData> sd = sine(512);
    auto& s                          = en.make<Sound>(sd);
    s.looping(true);
    s.play();

    for (;;) en.update();
}
}   // namespace audio
