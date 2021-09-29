#pragma once

#include <cstdint>

namespace window {

class VideoMode {
private:
    uint32_t fWidth;
    uint32_t fHeight;
    uint32_t fBitsPerPixel;
public:
    VideoMode(uint32_t width = 0u, uint32_t height = 0u, uint32_t bitsPerPixel = 32u)
        : fWidth(width)
        , fHeight(height)
        , fBitsPerPixel(bitsPerPixel)
        {}

    /// Getters
    const uint32_t width() const { return fWidth; }
    uint32_t& width() { return fWidth; }
    const uint32_t height() const { return fHeight; }
    uint32_t& height() { return fHeight; }
    const uint32_t bitsPerPixel() const { return fBitsPerPixel; }
    uint32_t& bitsPerPixel() { return fBitsPerPixel; }

    const uint32_t pitch() { return fWidth * bytesPerPixel(); }
    const uint32_t pixelBytes() { return fWidth * fHeight * bytesPerPixel(); }

    const uint32_t bytesPerPixel() const { return fBitsPerPixel / 8; }
    void setBytesPerPixel(const uint32_t bps) { fBitsPerPixel = bps * 8; }
};

}
