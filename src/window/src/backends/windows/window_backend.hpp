#pragma once

#include <memory>
#include <windows.h>

#include <macros/macros.hpp>
#include <window/video_mode.hpp>
#include <window/window.hpp>

#include "../../window_backend.hpp"

namespace window {

class WindowBackend final : public Window::Backend {
  public:
    WindowBackend(Window*);   // @todo give super ?
    WindowBackend(Window*, VideoMode mode, const std::string& title);
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
    HWND window();
    const WNDCLASS* windowClass();

  private:
    $pimpl_decl
};

}   // namespace window
