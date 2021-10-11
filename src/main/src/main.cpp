#include <iostream>

// #include <audio/audio_test.hpp>
// #include <window/window_test.hpp>

#include <cmath>

#include <window/bitmap.hpp>
#include <window/video_mode.hpp>
#include <window/window.hpp>

void renderWeirdGradient(window::BitMap& bm, int xOffset, int yOffset) {
    uint32_t* pixel = bm.data();
    uint8_t red     = ((sin(xOffset / 31.) + 1.) * 100);
    for (size_t y = 0; y < bm.mode().height(); ++y) {
        for (size_t x = 0; x < bm.mode().width(); ++x) {

            uint8_t blue  = (x + xOffset);
            uint8_t green = (y + sin(yOffset / 31.) * 100);
            *pixel        = (red << 16) | (green << 8) | blue;
            ++pixel;
        }
    }
}

int main() {
    // audio::test();
    // window::test();
    window::Window win{ { 1280, 720 }, "toto" };

    // input::KeyboardInput kin;
    // kin.registerToEventDispatcher(fEventDispatcher);
    // input::InputManager inputManager(kin);

    // inputManager.addMapping("up", input::Input(input::key::KeyboardKey::Up));
    // inputManager.addMapping("down", input::Input(input::key::KeyboardKey::Down));
    // inputManager.addMapping("fastUp", { input::key::KeyboardKey::LeftShift,
    //                                     input::key::KeyboardKey::Up });
    // inputManager.addMapping("fastDown", { input::key::KeyboardKey::LeftShift,
    //                                       input::key::KeyboardKey::Down });

    int xo = 0, yo = 0;
    while (true) {
        // processEvents();
        // fEventDispatcher.dispatchEvents();

        // {
        //     auto geom = xcb_get_geometry_reply(
        //         fpConnection, xcb_get_geometry(fpConnection, fpWindow), nullptr);
        //     resizeWindow(geom->width, geom->height);
        // }

        auto& bitmap = win.bitMap();
        renderWeirdGradient(bitmap, xo, yo);
        bitmap.flush();

        ++xo;
        //     if (inputManager.isActive("fastUp")) {
        //         std::cout << "fastUp" << std::endl;
        //         yo += 3;
        //     } else if (inputManager.isActive("fastDown")) {
        //         std::cout << "fastDown" << std::endl;
        //         yo -= 3;
        //     } else if (inputManager.isActive("up")) {
        //         std::cout << "up" << std::endl;
        //         yo += 1;
        //     } else if (inputManager.isActive("down")) {
        //         std::cout << "down" << std::endl;
        //         yo -= 1;
        //     }
        // }
    }
    return 0;
}
