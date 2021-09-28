#include "window_impl.hpp"
#include <iostream>
#include <windows.h>
#include <functional>

#include <macros/macros.hpp>

namespace window {

class WindowImpl::Impl {
private:
    HWND fpWindowHandle;
    bool fRunning;
public:
    Impl() : fRunning(false) {
        init();
    }

    ~Impl() = default;

private:
    bool processEvent(HWND Window,
                      UINT Message,
                      WPARAM WParam,
                      LPARAM LParam) {
        switch (Message) {
            case WM_SIZE:
                OutputDebugString("WM_SIZE\n");
                break;
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
                HDC DeviceContext = BeginPaint(Window, &Paint);
                auto X = Paint.rcPaint.left;
                auto Y = Paint.rcPaint.top;
                auto Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
                auto Widtg = Paint.rcPaint.right - Paint.rcPaint.left;
                PatBlt(DeviceContext, X, Y, Widtg, Height, WHITENESS);
                EndPaint(Window, &Paint);
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
        return window->processEvent(Window, Message, WParam, LParam) ? 0 : DefWindowProc(Window, Message, WParam, LParam);
    }

    void init() {
        WNDCLASS WindowClass = {};

        WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
        WindowClass.lpfnWndProc = windowCallback;
        auto Instance = GetModuleHandle(nullptr);
        WindowClass.hInstance = Instance;
        WindowClass.lpszClassName = "Toto";

        if (RegisterClass(&WindowClass)) {
            fpWindowHandle = CreateWindowEx(
                0,
                "Toto",
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
            while (fRunning) {
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                if (MessageResult > 0) {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                } else {
                    break;
                }
            }
        } else {
            // @todo
        }
    }



};

$pimpl_class(WindowImpl);
$pimpl_method(WindowImpl, void, run);

}
