#pragma once

#include <window/input.hpp>
#include <window/window.hpp>

namespace window {

class WindowBackend;

class Window::Backend {
  public:
    // Backend();
    // Backend(VideoMode mode, const std::string& title);
    // ~Backend();

    virtual void create()                                         = 0;
    virtual void create(VideoMode mode, const std::string& title) = 0;
    virtual void close()                                          = 0;

    virtual bool opened() const                 = 0;
    virtual const VideoMode& videoMode() const  = 0;
    virtual input::InputManager& inputManager() = 0;

    virtual BitMap& bitMap()   = 0;
    virtual void closeBitmap() = 0;

    virtual void update() = 0;

    WindowBackend& backend() { return reinterpret_cast<WindowBackend&>(*this); }
    const WindowBackend& backend() const {
        return reinterpret_cast<const WindowBackend&>(*this);
    }
};
}   // namespace window
