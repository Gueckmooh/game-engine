#include "window_impl.hpp"
#include <iostream>
#include <windows.h>
#include <functional>
#include <cstdint>
#include <cmath>

#include <macros/macros.hpp>

namespace {
struct Pixel{
    uint8_t* fpWord;
    uint8_t fBytesPerPixel;
    Pixel(uint8_t* word, uint8_t bytesPerPixel = 4)
        : fpWord(word)
        , fBytesPerPixel(bytesPerPixel)
        {}

    uint8_t& red() { return fpWord[2]; }
    uint8_t& green() { return fpWord[1]; }
    uint8_t& blue() { return fpWord[0]; }

    void setRGB(uint8_t red, uint8_t green, uint8_t blue) {
        this->red() = red;
        this->green() = green;
        this->blue() = blue;
    }

    void operator++(int) {
        fpWord += fBytesPerPixel;
    }
};


struct WindowDimension {
    uint32_t width;
    uint32_t height;

    WindowDimension(HWND Window) {
        RECT rect;
        GetClientRect(Window, &rect);
        height = rect.bottom - rect.top;
        width = rect.right - rect.left;
    }

    friend std::ostream& operator<< (std::ostream& stream, const WindowDimension& dims) {
        stream << "{ " << dims.width << " x " << dims.height << " }";
        return stream;
    }

};



}

namespace window {

class WindowImpl::Impl {
private:
    VideoMode fVideoMode;
    std::string fTitle;

    bool fRunning = false;

    void* fpBmMemory;

    HWND fpWindowHandle;
    BITMAPINFO fBmInfo;
    WNDCLASS fWindowClass;

public:
    Impl()
        : fVideoMode()
        , fTitle("") {
        // init();
    }

    Impl(VideoMode mode, const std::string name)
        : fVideoMode(std::move(mode))
        , fTitle(name) {
        init();
        run();
    }

    ~Impl() = default;

    void create(VideoMode mode, const std::string& title) {
        fVideoMode = std::move(mode);
        fTitle = title;
        init();
        run();
    }
    void create() {
        init();
        run();
    }
    void close() {
        // @todo
    }

    bool opened() const {
        return fRunning;
    }

    const VideoMode& videoMode() const {
        return fVideoMode;
    }

private:
    void renderWeirdGradient(int xOffset, int yOffset) {
        int pitch = fVideoMode.pitch();
        uint8_t* row = (uint8_t*) fpBmMemory;
        for (int y = 0; y < fVideoMode.height(); ++y) {
            uint32_t* pixel = (uint32_t*)row;
            for (int x = 0; x < fVideoMode.width(); ++x) {
                uint8_t blue = (x + sin(xOffset/31.)*1000);
                uint8_t green = (y + yOffset);
                *pixel++ = (green << 8) | blue;

            }
            row += pitch;
        }

    }

    void resizeDIBSection(unsigned width, unsigned height) {
        // @todo bullet proof this

        if (fpBmMemory) {
            // @note we could use MEM_UNCOMMIT
            VirtualFree(fpBmMemory, 0, MEM_RELEASE);
        }

        fVideoMode.width() = width;
        fVideoMode.height() = height;
                fVideoMode.setBytesPerPixel(4);


        fBmInfo.bmiHeader.biSize = sizeof(fBmInfo.bmiHeader);
        fBmInfo.bmiHeader.biWidth = fVideoMode.width();
        fBmInfo.bmiHeader.biHeight = -fVideoMode.height(); // Get a top down window
        fBmInfo.bmiHeader.biPlanes = 1;
        fBmInfo.bmiHeader.biBitCount = 32;
        fBmInfo.bmiHeader.biCompression = BI_RGB;
        fBmInfo.bmiHeader.biSizeImage = 0;
        fBmInfo.bmiHeader.biXPelsPerMeter = 0;
        fBmInfo.bmiHeader.biYPelsPerMeter = 0;
        fBmInfo.bmiHeader.biClrUsed = 0;
        fBmInfo.bmiHeader.biClrImportant = 0;

        fpBmMemory = VirtualAlloc(0, fVideoMode.pixelBytes(), MEM_COMMIT, PAGE_READWRITE);

        renderWeirdGradient(0, 0);
    }

    // @todo remove useless params
    void updateWindow(HDC deviceContext, WindowDimension dims, unsigned x, unsigned y, unsigned width, unsigned height) {
        std::cout << "-> " << dims << std::endl;
        StretchDIBits(deviceContext,
                      // x, y, width, height,
                      // x, y, width, height,
                      0, 0, dims.width, dims.height,
                      0, 0, fVideoMode.width(), fVideoMode.height(),
                      fpBmMemory,
                      &fBmInfo,
                      DIB_RGB_COLORS,
                      SRCCOPY);
    }

    bool processEvent(HWND Window,
                      UINT Message,
                      WPARAM WParam,
                      LPARAM LParam) {
        switch (Message) {
        case WM_SIZE: {
            // WindowDimension dims{fpWindowHandle};
            // resizeDIBSection(dims.width, dims.height);
            // OutputDebugString("WM_SIZE\n");
            break;
        }
        case WM_DESTROY:
            // @todo
            fRunning = false;
            break;
        case WM_CLOSE:
            // @todo
            fRunning = false;
            break;
        case WM_ACTIVATEAPP:
            OutputDebugString("WM_ACTIVATEAPP\n");
            break;
        case WM_PAINT: {
            PAINTSTRUCT Paint;
            HDC deviceContext = BeginPaint(Window, &Paint);
            auto x = Paint.rcPaint.left;
            auto y = Paint.rcPaint.top;
            auto height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            auto width = Paint.rcPaint.right - Paint.rcPaint.left;

            WindowDimension dims{fpWindowHandle};
            updateWindow(deviceContext, dims, x, y, width, height);
        } break;
        default:
            return false;
        }
        return true;
    }

    static LRESULT CALLBACK
    windowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
        auto* window = reinterpret_cast<Impl*>(GetWindowLongPtr(Window, GWLP_USERDATA));
        if (!window) {
            return DefWindowProc(Window, Message, WParam, LParam);
        }
        return window->processEvent(Window, Message, WParam, LParam) ? 0 :
            DefWindowProc(Window, Message, WParam, LParam);
    }

    void init() {
        fWindowClass = {};

        resizeDIBSection(fVideoMode.width(), fVideoMode.height());

        fWindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
        fWindowClass.lpfnWndProc = windowCallback;
        auto Instance = GetModuleHandle(NULL);
        fWindowClass.hInstance = Instance;
        fWindowClass.lpszClassName = "Window";

        if (RegisterClass(&fWindowClass)) {
            fpWindowHandle = CreateWindowExA(
                0,
                fWindowClass.lpszClassName,
                fTitle.c_str(),
                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);
            if (fpWindowHandle) {
                SetWindowLongPtrW(fpWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

            } else {

            }
        } else {

        }

    }
public:
    void run() {
        fRunning = true;
        if (fpWindowHandle) {
            MSG Message;
            int xo = 0, yo = 0;
            while (fRunning) {
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
                    if (Message.message == WM_QUIT) {
                        fRunning = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }


                renderWeirdGradient(xo, yo);

                HDC deviceContext = GetDC(fpWindowHandle);
                RECT rect;
                WindowDimension dims {fpWindowHandle};
                updateWindow(deviceContext, dims, 0, 0, dims.width, dims.height);
                ReleaseDC(fpWindowHandle, deviceContext);

                ++xo;

                // BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                // if (MessageResult > 0) {
                //     TranslateMessage(&Message);
                //     DispatchMessage(&Message);
                // } else {
                //     break;
                // }
            }
        } else {
            // @todo
        }
    }



};


// Pimpl declarations
$pimpl_class(WindowImpl);
$pimpl_class(WindowImpl, VideoMode, mode, const std::string&, title);
$pimpl_class_delete(WindowImpl);

$pimpl_method(WindowImpl, void, create);
$pimpl_method(WindowImpl, void, create, VideoMode, mode, const std::string&, title);
$pimpl_method(WindowImpl, void, close);
$pimpl_method_const(WindowImpl, bool, opened);
$pimpl_method_const(WindowImpl, const VideoMode&, videoMode);

}
