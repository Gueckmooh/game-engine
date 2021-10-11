#pragma once

#include <cstdlib>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <xcb/shm.h>
#include <xcb/xcb_image.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon-x11.h>

#include <window/bitmap.hpp>
#include <window/video_mode.hpp>

#include "../../bitmap_backend.hpp"
#include "window_backend.hpp"

namespace window {
class BitMapBackend final : public BitMap::Backend {
  public:
    friend class WindowImpl;

    BitMapBackend(Window::Backend&);
    BitMapBackend(VideoMode mode, xcb_connection_t* conn, xcb_window_t win);
    ~BitMapBackend();

    uint32_t* data();
    void flush();
    const VideoMode& mode() const;

  private:
    void resize(uint32_t width, uint32_t height);

    $pimpl_decl;
};
}   // namespace window
