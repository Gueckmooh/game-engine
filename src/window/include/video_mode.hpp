#pragma once

#include <cstdint>
#include <iostream>

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
    uint32_t width() const { return fWidth; }
    uint32_t& width() { return fWidth; }
    uint32_t height() const { return fHeight; }
    uint32_t& height() { return fHeight; }
    uint32_t bitsPerPixel() const { return fBitsPerPixel; }
    uint32_t& bitsPerPixel() { return fBitsPerPixel; }

    uint32_t pitch() const { return fWidth * bytesPerPixel(); }
    uint32_t pixelBytes() { return fWidth * fHeight * bytesPerPixel(); }

    uint32_t bytesPerPixel() const { return fBitsPerPixel / 8; }
    void setBytesPerPixel(const uint32_t bps) { fBitsPerPixel = bps * 8; }

    friend std::ostream& operator<< (std::ostream& stream, const VideoMode& dims) {
        stream << "{ " << dims.width() << " x " << dims.height() << " }";
        return stream;
    }
};

}
