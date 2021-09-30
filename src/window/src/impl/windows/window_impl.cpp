#include "window_impl.hpp"
#include <iostream>
#include <windows.h>
#include <functional>
#include <cstdint>
#include <cmath>

#include <macros/macros.hpp>

#include "bitmap_impl.hpp"
#include "../../genericManip.hpp"

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
    VideoMode fVideoMode;
    std::string fTitle;

    bool fRunning = false;

    void* fpBmMemory;

    HWND fpWindowHandle;
    BITMAPINFO fBmInfo;
    WNDCLASS fWindowClass;

    std::unique_ptr<BitMap> fpBitMap;

public:
    Impl()
        : fVideoMode()
        , fTitle("") {
        init();
        run();
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

    BitMap& bitMap() {
        if (!fpBitMap) {
            fpBitMap = std::make_unique<BitMapImpl>(fVideoMode, fpWindowHandle, &fWindowClass);
        }
        return *fpBitMap;
    }

    void closeBitmap() {
        fpBitMap.reset(nullptr);
    }


private:
    void resize(unsigned width, unsigned height) {
        fVideoMode.width() = width;
        fVideoMode.height() = height;
    }

    void resizeBitMap(unsigned width, unsigned height) {
        // @todo bullet proof this
        resize(width, height);  // @todo do I need this?
        static_cast<BitMapImpl&>(bitMap()).resize(width, height);
    }

    // @todo remove useless params
    void updateWindow() {
        bitMap().flush();
    }

    void paintWindow() {
        static_cast<BitMapImpl&>(bitMap()).paint();
    }

    bool processEvent(HWND Window,
                      UINT Message,
                      WPARAM WParam,
                      LPARAM LParam) {
        switch (Message) {
        case WM_SIZE: {
            WindowDimension dims{fpWindowHandle};
            resizeBitMap(dims.width, dims.height);
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
            paintWindow();
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


                test::renderWeirdGradient(bitMap(), xo, yo);

                updateWindow();

                ++xo;
                ++yo;

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
$pimpl_method(WindowImpl, BitMap&, bitMap);
$pimpl_method(WindowImpl, void, closeBitmap);
$pimpl_method_const(WindowImpl, bool, opened);
$pimpl_method_const(WindowImpl, const VideoMode&, videoMode);

}
