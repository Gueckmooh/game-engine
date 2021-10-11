#pragma once

#include <windows.h>

#include <window/bitmap.hpp>
#include <window/video_mode.hpp>

#include "../../bitmap_backend.hpp"
#include "utils.hpp"
#include "window_backend.hpp"

namespace window {

class BitMapBackend final : public BitMap::Backend {
    // private:
  public:
    // @todo remove this ?
    VideoMode fMode;
    uint32_t* fpBmMemory;

    HWND fpWindowHandle;
    BITMAPINFO fBmInfo;
    const WNDCLASS* fpWindowClass;
    // PAINTSTRUCT fPaint;
  public:
    friend class WindowImpl;
    BitMapBackend(Window::Backend&);
    BitMapBackend(VideoMode mode, HWND windowHandle, const WNDCLASS* pWindowClass);
    ~BitMapBackend();

    uint32_t* data();
    void flush();
    void paint();
    const VideoMode& mode() const;

  private:
    void resize(uint32_t width, uint32_t height);

    $pimpl_decl;
};

}   // namespace window
