#include <cmath>
// #include <ctime>
#include <chrono>
#include <iostream>
#include <logging/logger.hpp>
#include <memory>

#include <audio/audio_engine.hpp>
#include <audio/examples/examples.hpp>
#include <audio/sound_data.hpp>
#include <window/bitmap.hpp>
#include <window/examples/examples.hpp>
#include <window/input.hpp>
#include <window/video_mode.hpp>
#include <window/window.hpp>

namespace {
void waitToBeOnTime(std::chrono::duration<double> targetSecondsPerFrame) {
    static auto lastCounter = std::chrono::high_resolution_clock::now();
    auto workCounter        = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> workSecondsElapsed = workCounter - lastCounter;
    std::chrono::duration<double> secondsElapsed     = workSecondsElapsed;
    if (secondsElapsed < targetSecondsPerFrame) {
        while (secondsElapsed < targetSecondsPerFrame) {
            secondsElapsed = std::chrono::high_resolution_clock::now() - lastCounter;
        }
    }
    lastCounter = std::chrono::high_resolution_clock::now();
}

void initLogger() {
    logging::logger.enable("sound");
    // logging::logger.enable("main");
}

void computeFPS() {
    static auto begin                     = std::chrono::high_resolution_clock::now();
    auto end                              = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - begin;
    double msPerFrame                     = elapsed.count() * 1000;   // / 1'000'000;
    double fps                            = 1000 / msPerFrame;
    logging::logger.info("main.loop.fps")
        << (int)round(fps) << "fps " << (msPerFrame) << "mspf" << std::endl;
    begin = std::chrono::high_resolution_clock::now();
}
}   // namespace

int main() {
    initLogger();

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
    inputManager.addMapping("toto",
                            window::input::Input(window::input::key::KeyboardKey::A));

    // struct timespec begin, end;
    // auto begin = std::chrono::system_clock::now();
    // std::chrono::time_point<std::chrono::system_clock,
    //                         std::chrono::duration<long, std::ratio<1, 1000000000>>>
    // begin, end;
    // begin = std::chrono::system_clock::now();

    int monitorRefreshHz             = 60;
    int gameUpdateHz                 = monitorRefreshHz;
    double targetMiliSecondsPerFrame = 1000.0f / (double)gameUpdateHz;

    int xo = 0, yo = 0;
    auto& bitmap = win.bitMap();
    while (true) {
        // clock_gettime(CLOCK_REALTIME, &begin);

        window::examples::renderWeirdGradient(bitmap, xo, yo);
        if (inputManager.isActive("toto")) {
            s.stop();
        } else {
            s.play();
        }

        ++xo;
        if (inputManager.isActive("fastUp")) {
            yo += 3;
        } else if (inputManager.isActive("fastDown")) {
            yo -= 3;
        } else if (inputManager.isActive("up")) {
            yo += 1;
        } else if (inputManager.isActive("down")) {
            yo -= 1;
        }
        en.update();

        waitToBeOnTime(
            std::chrono::duration<double, std::milli>(targetMiliSecondsPerFrame));
        win.update();

        computeFPS();
    }
    return 0;
}
