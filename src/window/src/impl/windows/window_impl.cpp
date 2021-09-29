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






}

namespace window {

class WindowImpl::Impl {
private:
    HWND fpWindowHandle;
    bool fRunning;
    BITMAPINFO fBmInfo;
    void* fpBmMemory;
    WNDCLASS fWindowClass;
    unsigned fBmWidth;
    unsigned fBmHeight;
    unsigned fBytesPerPixel;
public:
    Impl() : fRunning(false) {
        init();
    }

    ~Impl() = default;

private:
    void toto(int xOffset, int yOffset) {
        int pitch = fBmWidth * fBytesPerPixel;
        uint8_t* row = (uint8_t*) fpBmMemory;
        for (int y = 0; y < fBmHeight; ++y) {
            // uint8_t *pixel = (uint8_t*) row;
#if 0
            Pixel pixel{row};
            for (int x = 0; x < fBmWidth; ++x) {
                // Pixel layout: BB GG RR xx
                // 0x xxRRGGBB
                pixel.setRGB(0u, (uint8_t)(y + yOffset), (uint8_t)(x + xOffset));
                pixel++;
            }
#else
            uint32_t* pixel = (uint32_t*)row;
            for (int x = 0; x < fBmWidth; ++x) {
                uint8_t blue = (x + sin(xOffset/31.)*100);
                uint8_t green = (y + yOffset);
                *pixel++ = (green << 8) | blue;

            }
#endif
            row += pitch;
        }

    }


    void resizeDIBSection(unsigned width, unsigned height) {
        // @todo bullet proof this

        if (fpBmMemory) {
            // @note we could use MEM_UNCOMMIT
            VirtualFree(fpBmMemory, 0, MEM_RELEASE);
        }

        fBmWidth = width;
        fBmHeight = height;

        fBmInfo.bmiHeader.biSize = sizeof(fBmInfo.bmiHeader);
        fBmInfo.bmiHeader.biWidth = fBmWidth;
        fBmInfo.bmiHeader.biHeight = -fBmHeight; // Get a top down window
        fBmInfo.bmiHeader.biPlanes = 1;
        fBmInfo.bmiHeader.biBitCount = 32;
        fBmInfo.bmiHeader.biCompression = BI_RGB;
        fBmInfo.bmiHeader.biSizeImage = 0;
        fBmInfo.bmiHeader.biXPelsPerMeter = 0;
        fBmInfo.bmiHeader.biYPelsPerMeter = 0;
        fBmInfo.bmiHeader.biClrUsed = 0;
        fBmInfo.bmiHeader.biClrImportant = 0;

        // fpBmHandle = CreateDIBSection(fpDeviceContext, &fBmInfo, DIB_RGB_COLORS, &fpBmMemory, nullptr, 0);
        fBytesPerPixel = 4;
        unsigned bmMemorySize = (width * height) * fBytesPerPixel;
        fpBmMemory = VirtualAlloc(0, bmMemorySize, MEM_COMMIT, PAGE_READWRITE);

        toto(0, 0);

    }

    void updateWindow(HDC deviceContext, RECT *pRect, unsigned x, unsigned y, unsigned width, unsigned height) {
        unsigned winWidth = pRect->right - pRect->left;
        unsigned winHeight = pRect->bottom - pRect->top;
        StretchDIBits(deviceContext,
                      // x, y, width, height,
                      // x, y, width, height,
                      0, 0, fBmWidth, fBmHeight,
                      0, 0, winWidth, winHeight,
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
            RECT rect;
            GetClientRect(fpWindowHandle, &rect);
            auto height = rect.bottom - rect.top;
            auto width = rect.right - rect.left;
            resizeDIBSection(width, height);
            OutputDebugString("WM_SIZE\n");
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

            RECT rect;
            GetClientRect(fpWindowHandle, &rect);
            updateWindow(deviceContext, &rect, x, y, width, height);
        } break;
        default:
            return false;
        }
        return true;
    }

    static LRESULT CALLBACK
    windowCallback(HWND Window,
                   UINT Message,
                   WPARAM WParam,
                   LPARAM LParam) {
        auto* window = reinterpret_cast<Impl*>(GetWindowLongPtr(Window, GWLP_USERDATA));
        if (!window) {
            return DefWindowProc(Window, Message, WParam, LParam);
        }
        return window->processEvent(Window, Message, WParam, LParam) ? 0 : DefWindowProc(Window, Message, WParam, LParam);
    }

    void init() {
        fWindowClass = {};

        fWindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
        fWindowClass.lpfnWndProc = windowCallback;
        auto Instance = GetModuleHandle(NULL);
        fWindowClass.hInstance = Instance;
        fWindowClass.lpszClassName = "Toto";

        if (RegisterClass(&fWindowClass)) {
            fpWindowHandle = CreateWindowExA(
                0,
                fWindowClass.lpszClassName,
                "titi",
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


                toto(xo, yo);

                HDC deviceContext = GetDC(fpWindowHandle);
                RECT rect;
                GetClientRect(fpWindowHandle, &rect);
                int width = rect.right - rect.left;
                int height = rect.bottom - rect.top;
                updateWindow(deviceContext, &rect, 0, 0, width, height);
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

$pimpl_class(WindowImpl);
$pimpl_method(WindowImpl, void, run);

}
