#pragma once

#include <memory>
#include <string>

#include <window/video_mode.hpp>

namespace window {

class Window {
public:
    Window() = default;
    Window(VideoMode mode, const std::string& title);
    virtual ~Window() = default;

    virtual void create() = 0;
    virtual void create(VideoMode mode, const std::string& title) = 0;
    virtual void close() = 0;

    virtual bool opened() const = 0;
    virtual const VideoMode& videoMode() const = 0;
};

}
