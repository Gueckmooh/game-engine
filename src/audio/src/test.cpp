#include <cmath>
#include <memory>
#include <pulse/stream.h>

#include <audio/audio_engine.hpp>
#include <audio/audio_source.hpp>
#include <audio/audio_test.hpp>
#include <audio/sample_format.hpp>
#include <audio/sound.hpp>
#include <audio/sound_data.hpp>

#include "audio_source_backend.hpp"
namespace audio {

std::shared_ptr<RawSoundData> sine(uint32_t toneHZ, int16_t volume = 16000) {

    auto rawSoundData = std::make_shared<RawSoundData>(SampleFormat::Int16LE, 2u, 48000);
    float squareWavePeriod = rawSoundData->rate() / toneHZ;
    char* buffer           = (char*)rawSoundData->data();
    float tSine            = 0;
    int16_t* cursor        = (int16_t*)buffer;

    std::cout << "rawSoundData->sampleNumber(): " << rawSoundData->sampleNumber()
              << std::endl;

    for (ssize_t index = 0; index < rawSoundData->sampleNumber(); ++index) {
        float SineValue     = sinf(tSine);
        int16_t sampleValue = (int16_t)(SineValue * volume);

        *cursor++ = sampleValue;
        *cursor++ = sampleValue;

        tSine += 2.0f * M_PIf32 * 1.0f / squareWavePeriod;
    }

    size_t newSampleCount = rawSoundData->sampleNumber();
    uint16_t firstValue   = ((int16_t*)buffer)[0];
    std::cout << "firstValue: " << firstValue << std::endl;
    for (ssize_t index = rawSoundData->sampleNumber(); index >= 0; --index) {
        std::cout << "index: " << index << std::endl;
        std::cout << "((uint16_t*)buffer)[2 * index]: " << ((uint16_t*)buffer)[2 * index]
                  << std::endl;
        if (((uint16_t*)buffer)[2 * index] == firstValue) {
            newSampleCount = index;
            break;
        }
    }
    rawSoundData->resize(newSampleCount);

    std::cout << "rawSoundData->sampleNumber(): " << rawSoundData->sampleNumber()
              << std::endl;

    return rawSoundData;
}

void test() {
    AudioEngine en;
    std::shared_ptr<RawSoundData> sd = sine(250);
    auto& s                          = en.make<Sound>(sd);
    s.looping(true);
    s.play();

    for (;;) en.update();
}
}   // namespace audio
