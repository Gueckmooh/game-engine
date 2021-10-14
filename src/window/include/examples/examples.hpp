#pragma once

#include <cmath>

#include <window/bitmap.hpp>
#include <window/input.hpp>
#include <window/video_mode.hpp>
#include <window/window.hpp>

namespace window {
namespace examples {

void renderWeirdGradient(window::BitMap& bm, int xOffset, int yOffset) {
    uint32_t* pixel = bm.data();
    uint32_t height = bm.mode().height();
    uint32_t width  = bm.mode().width();
    uint8_t blue    = 0;
    uint8_t green   = 0;
    uint8_t red     = 0;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            blue   = (x + xOffset);
            red    = (y + sin(yOffset / 31.) * 100);
            *pixel = (red << 16) | (green << 8) | blue;
            ++pixel;
        }
    }
}

}   // namespace examples
}   // namespace window
