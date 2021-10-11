#include <iostream>
#include <memory>

#include <window/bitmap.hpp>

#ifdef __USE_WINDOWS__
#    include "backends/windows/bitmap_backend.hpp"
#else
#    include "backends/linux/bitmap_backend.hpp"
#endif

namespace window {

BitMap::BitMap(Window& window) {
    fpBackend = std::make_unique<BitMapBackend>(window.backend());
}
BitMap::~BitMap() {}

uint32_t* BitMap::data() { return fpBackend->data(); }
void BitMap::flush() { fpBackend->flush(); }
const VideoMode& BitMap::mode() const { return fpBackend->mode(); }

}   // namespace window
