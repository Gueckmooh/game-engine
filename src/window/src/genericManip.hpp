#pragma once

#include <window/bitmap.hpp>
#include <window/video_mode.hpp>
#include <cmath>

namespace test {
void renderWeirdGradient(window::BitMap& bm, int xOffset, int yOffset) {

    // auto pitch = bm.mode().pitch();
    // uint8_t* row = (uint8_t*) bm.data();
    // uint32_t width = bm.mode().width();
    // uint32_t height = bm.mode().height();
    // for (int y = 0; y < height; ++y) {
    //     uint32_t* pixel = (uint32_t*)row;
    //     for (int x = 0; x < width; ++x) {
    //         uint8_t blue = (x + sin(xOffset/31.)*1000);
    //         uint8_t green = (y + yOffset);
    //         *pixel++ = (green << 8) | blue;

    //     }
    //     row += pitch;
    // }

    uint32_t* pixel = bm.data();
    uint8_t red = ((sin(xOffset/31.)+1.)*100);
    for (int y = 0; y < bm.mode().height(); ++y) {
        for (int x = 0; x < bm.mode().width(); ++x) {

            uint8_t blue = (x + xOffset);
            uint8_t green = (y + sin(yOffset/31.)*100);
            *pixel = (red << 16) | (green << 8) | blue;
            ++pixel;
        }
    }
}

}
