#pragma once

#include <cstdint>

#include <window/video_mode.hpp>
#include <window/window.hpp>

namespace window {

class BitMap {
public:
    friend class Window;

    virtual uint32_t* data() = 0;
    virtual void flush() = 0;
    virtual const VideoMode& mode() const = 0;
private:
    virtual void resize(uint32_t width, uint32_t height) = 0;
};

}
