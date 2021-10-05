#include "window_impl.hpp"

#include "../../genericManip.hpp"
#include "bitmap_impl.hpp"

#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <macros/macros.hpp>
#include <window/input.hpp>
#include <windows.h>
#include <xinput.h>

namespace {
// @todo Do I need this?
struct Pixel {
    uint8_t* fpWord;
    uint8_t fBytesPerPixel;
    Pixel(uint8_t* word, uint8_t bytesPerPixel = 4)
        : fpWord(word), fBytesPerPixel(bytesPerPixel) {}

    uint8_t& red() { return fpWord[2]; }
    uint8_t& green() { return fpWord[1]; }
    uint8_t& blue() { return fpWord[0]; }

    void setRGB(uint8_t red, uint8_t green, uint8_t blue) {
        this->red()   = red;
        this->green() = green;
        this->blue()  = blue;
    }

    void operator++(int) { fpWord += fBytesPerPixel; }
};

}   // namespace

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
    Impl() : fVideoMode(), fTitle("") {
        init();
        run();
    }

    Impl(VideoMode mode, const std::string name)
        : fVideoMode(std::move(mode)), fTitle(name) {
        init();
        run();
    }

    ~Impl() = default;

    void create(VideoMode mode, const std::string& title) {
        fVideoMode = std::move(mode);
        fTitle     = title;
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

    bool opened() const { return fRunning; }

    const VideoMode& videoMode() const { return fVideoMode; }

    BitMap& bitMap() {
        if (!fpBitMap) {
            fpBitMap =
                std::make_unique<BitMapImpl>(fVideoMode, fpWindowHandle, &fWindowClass);
        }
        return *fpBitMap;
    }

    void closeBitmap() { fpBitMap.reset(nullptr); }

  private:
    void resize(unsigned width, unsigned height) {
        fVideoMode.width()  = width;
        fVideoMode.height() = height;
    }

    void resizeBitMap(unsigned width, unsigned height) {
        // @todo bullet proof this
        resize(width, height);   // @todo do I need this?
        static_cast<BitMapImpl&>(bitMap()).resize(width, height);
    }

    // @todo remove useless params
    void updateWindow() { bitMap().flush(); }

    void paintWindow() { static_cast<BitMapImpl&>(bitMap()).paint(); }

    void processEvents() {
        MSG msg;
        while (PeekMessage(&msg, fpWindowHandle, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { fRunning = false; }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    bool processEvent(UINT Message, WPARAM WParam, LPARAM LParam) {
        switch (Message) {
        case WM_SIZE: {
            WindowDimension dims{ fpWindowHandle };
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
        case WM_ACTIVATEAPP: OutputDebugString("WM_ACTIVATEAPP\n"); break;
        case WM_PAINT: {
            paintWindow();
        } break;
        default: return false;
        }
        return true;
    }

    static LRESULT CALLBACK windowCallback(HWND Window, UINT Message, WPARAM WParam,
                                           LPARAM LParam) {
        auto* window = reinterpret_cast<Impl*>(GetWindowLongPtr(Window, GWLP_USERDATA));
        if (!window) { return DefWindowProc(Window, Message, WParam, LParam); }
        return window->processEvent(Message, WParam, LParam)
                   ? 0
                   : DefWindowProc(Window, Message, WParam, LParam);
    }

    void init() {
        fWindowClass = {};

        fWindowClass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        fWindowClass.lpfnWndProc   = windowCallback;
        auto Instance              = GetModuleHandle(NULL);
        fWindowClass.hInstance     = Instance;
        fWindowClass.lpszClassName = "Window";

        if (RegisterClass(&fWindowClass)) {
            fpWindowHandle = CreateWindowExA(
                0, fWindowClass.lpszClassName, fTitle.c_str(),
                WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
                CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);
            if (fpWindowHandle) {
                SetWindowLongPtrW(fpWindowHandle, GWLP_USERDATA,
                                  reinterpret_cast<LONG_PTR>(this));

            } else {
            }
        } else {
        }
    }

  public:
    class WinControllerInput : public input::ControllerInput {
      public:
        void readInput() {
            for (int controllerIdx = 0; controllerIdx < XUSER_MAX_COUNT;
                 ++controllerIdx) {
                XINPUT_STATE controllerState;
                memset(&controllerState, 0, sizeof(XINPUT_STATE));
                if (XInputGetState(controllerIdx, &controllerState) == ERROR_SUCCESS) {
                    // @note the controller is blugged in
                    XINPUT_GAMEPAD& pad = controllerState.Gamepad;

                    bool dPadUp        = (pad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
                    bool dPadDown      = (pad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                    bool dPadLeft      = (pad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                    bool dPadRight     = (pad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                    bool start         = (pad.wButtons & XINPUT_GAMEPAD_START);
                    bool back          = (pad.wButtons & XINPUT_GAMEPAD_BACK);
                    bool leftThumb     = (pad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
                    bool rightThumb    = (pad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
                    bool leftShoulder  = (pad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                    bool rightShoulder = (pad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                    bool keyA          = (pad.wButtons & XINPUT_GAMEPAD_A);
                    bool keyB          = (pad.wButtons & XINPUT_GAMEPAD_B);
                    bool keyX          = (pad.wButtons & XINPUT_GAMEPAD_X);
                    bool keyY          = (pad.wButtons & XINPUT_GAMEPAD_Y);

                    setKey(input::key::ControllerKey::DPadUp, dPadUp);
                    setKey(input::key::ControllerKey::DPadDown, dPadDown);
                    setKey(input::key::ControllerKey::DPadLeft, dPadLeft);
                    setKey(input::key::ControllerKey::DPadRight, dPadRight);
                    setKey(input::key::ControllerKey::Start, start);
                    setKey(input::key::ControllerKey::Back, back);
                    setKey(input::key::ControllerKey::LeftThumb, leftThumb);
                    setKey(input::key::ControllerKey::RightThumb, rightThumb);
                    setKey(input::key::ControllerKey::LeftShoulder, leftShoulder);
                    setKey(input::key::ControllerKey::RightShoulder, rightShoulder);
                    setKey(input::key::ControllerKey::A, keyA);
                    setKey(input::key::ControllerKey::B, keyB);
                    setKey(input::key::ControllerKey::X, keyX);
                    setKey(input::key::ControllerKey::Y, keyY);

                    int16_t lStickX = pad.sThumbLX;
                    int16_t lStickY = pad.sThumbLY;
                    int16_t rStickX = pad.sThumbRX;
                    int16_t rStickY = pad.sThumbRY;
                } else {
                    // @note the controller is not available
                }
            }
            // forwardInput();
        }
    };

    void run() {
        std::shared_ptr<input::ControllerInput> ipt =
            std::make_shared<WinControllerInput>();
        input::InputManager inputManager{ *ipt };
        inputManager.addMapping("up", input::Input(input::key::ControllerKey::DPadUp));
        inputManager.addMapping("down",
                                input::Input(input::key::ControllerKey::DPadDown));
        inputManager.addMapping("fastUp", { input::key::ControllerKey::LeftShoulder,
                                            input::key::ControllerKey::DPadUp });
        inputManager.addMapping("fastDown", { input::key::ControllerKey::LeftShoulder,
                                              input::key::ControllerKey::DPadDown });
        fRunning = true;
        if (fpWindowHandle) {
            MSG Message;
            int xo = 0, yo = 0;
            while (fRunning) {
                processEvents();

                inputManager.readInput();

                // @todo should we poll this more frequently?
                test::renderWeirdGradient(bitMap(), xo, yo);

                updateWindow();

                ++xo;
                // @todo make fast up exclude up?
                if (inputManager.isActive("fastUp")) {
                    yo += 3;
                } else if (inputManager.isActive("fastDown")) {
                    yo -= 3;
                } else if (inputManager.isActive("up")) {
                    yo += 1;
                } else if (inputManager.isActive("down")) {
                    yo -= 1;
                }

                // if (cipt.getKey(input::key::ControllerKey::A)) {
                //     ++yo;
                // } else if (cipt.getKey(input::key::ControllerKey::B)) {
                //     --yo;
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
$pimpl_method(WindowImpl, BitMap&, bitMap);
$pimpl_method(WindowImpl, void, closeBitmap);
$pimpl_method_const(WindowImpl, bool, opened);
$pimpl_method_const(WindowImpl, const VideoMode&, videoMode);

}   // namespace window
