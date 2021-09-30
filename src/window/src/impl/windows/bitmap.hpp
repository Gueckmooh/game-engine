#pragma once

#include <window/bitmap.hpp>
#include <window/video_mode.hpp>

#include <windows.h>

#include "utils.hpp"
#include "window_impl.hpp"

namespace window {

class BitMapImpl final : public BitMap {
// private:
public:
    VideoMode fMode;
    uint32_t* fpBmMemory;

    HWND fpWindowHandle;
    BITMAPINFO fBmInfo;
    const WNDCLASS* fpWindowClass;
    // PAINTSTRUCT fPaint;
public:
    friend class WindowImpl;
    BitMapImpl(VideoMode mode, HWND windowHandle, const WNDCLASS* pWindowClass);
    ~BitMapImpl();

    uint32_t* data();
    void flush();
    void paint();
    const VideoMode& mode() const;
private:
    void resize(uint32_t width, uint32_t height);

    $pimpl_decl;
};

}
