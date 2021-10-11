#include <cmath>
// #include <ctime>
#include <iostream>
#include <memory>

#include <audio/audio_engine.hpp>
#include <audio/examples/examples.hpp>
#include <audio/sound_data.hpp>
#include <window/bitmap.hpp>
#include <window/examples/examples.hpp>
#include <window/input.hpp>
#include <window/video_mode.hpp>
#include <window/window.hpp>

int main() {
    window::Window win{ { 1280, 720 }, "toto" };
    audio::AudioEngine en;
    std::shared_ptr<audio::RawSoundData> sd = audio::examples::sine(en, 256);
    auto& s                                 = en.make<audio::Sound>(sd);
    s.looping(true);
    s.play();

    auto& inputManager = win.inputManager();
    inputManager.addMapping("up",
                            window::input::Input(window::input::key::KeyboardKey::Up));
    inputManager.addMapping("down",
                            window::input::Input(window::input::key::KeyboardKey::Down));
    inputManager.addMapping("fastUp", { window::input::key::KeyboardKey::LeftShift,
                                        window::input::key::KeyboardKey::Up });
    inputManager.addMapping("fastDown", { window::input::key::KeyboardKey::LeftShift,
                                          window::input::key::KeyboardKey::Down });

    // struct timespec begin, end;

    int xo = 0, yo = 0;
    while (true) {
        // clock_gettime(CLOCK_REALTIME, &begin);
        auto& bitmap = win.bitMap();
        window::examples::renderWeirdGradient(bitmap, xo, yo);
        win.update();
        en.update();

        ++xo;
        if (inputManager.isActive("fastUp")) {
            std::cout << "fastUp" << std::endl;
            yo += 3;
        } else if (inputManager.isActive("fastDown")) {
            std::cout << "fastDown" << std::endl;
            yo -= 3;
        } else if (inputManager.isActive("up")) {
            std::cout << "up" << std::endl;
            yo += 1;
        } else if (inputManager.isActive("down")) {
            std::cout << "down" << std::endl;
            yo -= 1;
        }

        // clock_gettime(CLOCK_REALTIME, &end);
        // int64_t elapsed   = end.tv_nsec - begin.tv_nsec;
        // double msPerFrame = elapsed / 1'000'000;
        // double fps        = 1000 / msPerFrame;
        // std::cout << fps << "fps " << msPerFrame << "mspf" << std::endl;
    }
    return 0;
}
