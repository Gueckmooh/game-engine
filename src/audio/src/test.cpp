#include <audio/audio_engine.hpp>
#include <audio/examples/examples.hpp>
#include <audio/sound_data.hpp>

namespace audio {

void test() {
    AudioEngine en;
    std::shared_ptr<RawSoundData> sd = examples::sine(en, 256);
    auto& s                          = en.make<Sound>(sd);
    s.looping(true);
    s.play();

    for (;;) en.update();
}
}   // namespace audio
