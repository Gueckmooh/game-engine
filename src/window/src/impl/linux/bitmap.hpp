#pragma once

#include <cstdlib>

#include <window/video_mode.hpp>
#include <window/bitmap.hpp>

#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xcb/xcb_image.h>

#include <sys/shm.h>
#include <sys/ipc.h>

#include <xcb/shm.h>
#include "window_impl.hpp"

namespace window {
class BitMapImpl final : public BitMap {
public:
    friend class WindowImpl;

    BitMapImpl(VideoMode mode, xcb_connection_t* conn, xcb_window_t win);
    ~BitMapImpl();

    uint32_t* data();
    void flush();
    const VideoMode& mode() const;
private:
    void resize(uint32_t width, uint32_t height);

    $pimpl_decl;
};
}
