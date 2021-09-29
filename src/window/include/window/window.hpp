#pragma once

#include <memory>
#include <window/video_mode.hpp>

namespace window {

class Window {
public:
    Window() = default;
    // Window(const VideoMode&);
    virtual ~Window() = default;
    virtual void run() = 0;
};

class WindowBuilder {
public:
    std::shared_ptr<Window> build();
};

}
