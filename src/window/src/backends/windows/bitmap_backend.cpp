#include "bitmap_backend.hpp"

namespace window {

class BitMapBackend::Impl {
    // private:
  public:
    VideoMode fMode;
    uint32_t* fpBmMemory;

    HWND fpWindowHandle;
    BITMAPINFO fBmInfo;
    const WNDCLASS* fpWindowClass;
    // PAINTSTRUCT fPaint;
  public:
    friend class BitMapBackend;
    Impl(Window::Backend& window) {
        WindowBackend& windowBackend = window.backend();
        fMode                        = windowBackend.videoMode();
        fpWindowHandle               = windowBackend.window();
        fpWindowClass                = windowBackend.windowClass();
    }

    Impl(VideoMode mode, HWND windowHandle, const WNDCLASS* pWindowClass)
        : fMode(mode), fpWindowHandle(windowHandle), fpWindowClass(pWindowClass) {
        init();
    }

    ~Impl() { terminate(); }

    uint32_t* data() { return fpBmMemory; }
    const VideoMode& mode() const { return fMode; }

    void flush() {
        if (fpBmMemory) {
            HDC myDeviceContext = GetDC(fpWindowHandle);
            WindowDimension myDims{ fpWindowHandle };
            StretchDIBits(myDeviceContext,
                          // x, y, width, height,
                          // x, y, width, height,
                          0, 0, myDims.width, myDims.height, 0, 0, fMode.width(),
                          fMode.height(),
                          // fpBmMemory,
                          data(),
                          // &fBmInfo,
                          &fBmInfo, DIB_RGB_COLORS, SRCCOPY);
            ReleaseDC(fpWindowHandle, myDeviceContext);
        }
    }

    void paint() {
        if (fpBmMemory) {
            PAINTSTRUCT paint;
            HDC myDeviceContext = BeginPaint(fpWindowHandle, &paint);
            auto x              = paint.rcPaint.left;
            auto y              = paint.rcPaint.top;
            auto height         = paint.rcPaint.bottom - paint.rcPaint.top;
            auto width          = paint.rcPaint.right - paint.rcPaint.left;
            // HDC myDeviceContext = GetDC(fpWindowHandle);
            WindowDimension myDims{ fpWindowHandle };
            StretchDIBits(myDeviceContext,
                          // x, y, width, height,
                          // x, y, width, height,
                          0, 0, myDims.width, myDims.height, 0, 0, fMode.width(),
                          fMode.height(),
                          // fpBmMemory,
                          data(),
                          // &fBmInfo,
                          &fBmInfo, DIB_RGB_COLORS, SRCCOPY);
            EndPaint(fpWindowHandle, &paint);
        }
    }

  private:
    void resize(uint32_t width, uint32_t height) {
        fMode.width()  = width;
        fMode.height() = height;

        init();
    }

    void init() {
        if (fpBmMemory) {
            // @note we could use MEM_UNCOMMIT
            VirtualFree(fpBmMemory, 0, MEM_RELEASE);
            fpBmMemory = nullptr;
        }

        fBmInfo.bmiHeader.biSize          = sizeof(fBmInfo.bmiHeader);
        fBmInfo.bmiHeader.biWidth         = fMode.width();
        fBmInfo.bmiHeader.biHeight        = -fMode.height();   // Get a top down window
        fBmInfo.bmiHeader.biPlanes        = 1;
        fBmInfo.bmiHeader.biBitCount      = 32;
        fBmInfo.bmiHeader.biCompression   = BI_RGB;
        fBmInfo.bmiHeader.biSizeImage     = 0;
        fBmInfo.bmiHeader.biXPelsPerMeter = 0;
        fBmInfo.bmiHeader.biYPelsPerMeter = 0;
        fBmInfo.bmiHeader.biClrUsed       = 0;
        fBmInfo.bmiHeader.biClrImportant  = 0;

        fpBmMemory =
            (uint32_t*)VirtualAlloc(0, fMode.pixelBytes(), MEM_COMMIT, PAGE_READWRITE);
    }

    void terminate() {
        if (fpBmMemory) {
            // @note we could use MEM_UNCOMMIT
            VirtualFree(fpBmMemory, 0, MEM_RELEASE);
            fpBmMemory = nullptr;
        }
    }
};

$pimpl_class(BitMapBackend, Window::Backend&, window);
$pimpl_class(BitMapBackend, VideoMode, mode, HWND, windowHandle, const WNDCLASS*,
             pWindowClass);
$pimpl_class_delete(BitMapBackend);

$pimpl_method(BitMapBackend, uint32_t*, data);
$pimpl_method(BitMapBackend, void, flush);
$pimpl_method(BitMapBackend, void, paint);
$pimpl_method_const(BitMapBackend, const VideoMode&, mode);
$pimpl_method(BitMapBackend, void, resize, uint32_t, width, uint32_t, height);

}   // namespace window
