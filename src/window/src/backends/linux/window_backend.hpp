#pragma once

#include <memory>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon-x11.h>

#include <macros/macros.hpp>
#include <window/input.hpp>
#include <window/window.hpp>

#include "../../window_backend.hpp"

namespace window {

class WindowBackend final : public Window::Backend {
  public:
    WindowBackend(Window*);
    WindowBackend(Window*, VideoMode mode, const std::string& title,
                  Window::Flag option = Window::DEFAULT);
    virtual ~WindowBackend();

    void create();
    void create(VideoMode mode, const std::string& title);
    void close();

    bool opened() const;
    const VideoMode& videoMode() const;
    input::InputManager& inputManager();

    void update();

    BitMap& bitMap();
    void closeBitmap();

    // ----- Plateform specific

    xcb_connection_t* connection() const;
    xcb_window_t window() const;

  private:
    $pimpl_decl;
};
}   // namespace window
