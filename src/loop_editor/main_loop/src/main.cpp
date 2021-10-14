#include <cmath>
// #include <ctime>
#include <chrono>
#include <file_watcher/file_watcher.hpp>
#include <iostream>
#include <main_loop/game_data.hpp>
#include <memory>
#include <thread>

#include <audio/audio_engine.hpp>
#include <audio/examples/examples.hpp>
#include <audio/sound_data.hpp>
#include <logging/logger.hpp>
#include <window/bitmap.hpp>
#include <window/examples/examples.hpp>
#include <window/input.hpp>
#include <window/video_mode.hpp>
#include <window/window.hpp>

using namespace std::chrono_literals;

extern "C" void initInputManager(window::input::InputManager& inputManager) {
    inputManager.addMapping("up",
                            window::input::Input(window::input::key::KeyboardKey::Up));
    inputManager.addMapping("down",
                            window::input::Input(window::input::key::KeyboardKey::Down));
    inputManager.addMapping("left",
                            window::input::Input(window::input::key::KeyboardKey::Left));
    inputManager.addMapping("right",
                            window::input::Input(window::input::key::KeyboardKey::Right));
    inputManager.addMapping("jump",
                            window::input::Input(window::input::key::KeyboardKey::Space));
}

extern "C" void processInputs(window::input::InputManager& inputManager,
                              game_data::GameData& gd) {
    gd.backgroundX += 1;
    if (inputManager.isActive("up")) {
        gd.charY -= 2;
    } else if (inputManager.isActive("down")) {
        gd.charY += 2;
    } else if (inputManager.isActive("left")) {
        gd.charX -= 2;
    } else if (inputManager.isActive("right")) {
        gd.charX += 2;
    }

    if (gd.jump > 0) { gd.charY += (int)(10.0f * sinf(0.5f * M_PIf32 * gd.jump)); }
    if (inputManager.isActive("jump")) { gd.jump = 4.0; }
    gd.jump -= 0.033f;
}

void renderPlayer(window::BitMap& bitmap, game_data::GameData& gd) {
    int PlayerX = gd.charX;
    int PlayerY = gd.charY;
    uint8_t* EndOfBuffer =
        (uint8_t*)bitmap.data() + bitmap.mode().pitch() * bitmap.mode().height();

    uint32_t Color     = 0xFFFF00FF;
    int Top            = PlayerY;
    int Bottom         = PlayerY + 10;
    uint8_t* buffer    = (uint8_t*)bitmap.data();
    auto bytesPerPixel = bitmap.mode().bytesPerPixel();
    auto pitch         = bitmap.mode().pitch();
    for (int X = PlayerX; X < PlayerX + 10; ++X) {
        uint8_t* Pixel = ((uint8_t*)buffer + X * bytesPerPixel + Top * pitch);
        for (int Y = Top; Y < Bottom; ++Y) {
            if ((Pixel >= buffer) && ((Pixel + 4) <= EndOfBuffer)) {
                *(uint32_t*)Pixel = Color;
            }

            Pixel += pitch;
        }
    }
}

extern "C" void renderBitmap(window::BitMap& bitmap, game_data::GameData& gd) {
    window::examples::renderWeirdGradient(bitmap, gd.backgroundX, gd.backgroundY);
    renderPlayer(bitmap, gd);
}
