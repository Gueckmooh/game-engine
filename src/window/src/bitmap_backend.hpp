#pragma once

#include <window/bitmap.hpp>

namespace window {

class BitMapBackend;

class BitMap::Backend {
  public:
    // Backend(BitMap*, Window::Backend&);

    virtual uint32_t* data()                             = 0;
    virtual void flush()                                 = 0;
    virtual const VideoMode& mode() const                = 0;
    virtual void resize(uint32_t width, uint32_t height) = 0;

    BitMapBackend& backend() { return reinterpret_cast<BitMapBackend&>(*this); }
    const BitMapBackend& backend() const {
        return reinterpret_cast<const BitMapBackend&>(*this);
    }
};
}   // namespace window
