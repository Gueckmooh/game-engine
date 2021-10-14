#include "window_backend.hpp"

#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <thread>
#include <xcb/shm.h>
#include <xcb/xcb_image.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon-x11.h>

#include <macros/macros.hpp>
#include <window/input.hpp>
#include <window/window_system_events.hpp>

#include "../../genericManip.hpp"
#include "bitmap_backend.hpp"
#include "extent.hpp"

// @note These horizontal mouse button indices
// are not always defined...
#if !defined(XCB_BUTTON_INDEX_6)
#    define XCB_BUTTON_INDEX_6 6
#endif
#if !defined(XCB_BUTTON_INDEX_7)
#    define XCB_BUTTON_INDEX_7 7
#endif

namespace {

// @todo Move this elsewhere
uint32_t utf8Codepoint(const uint8_t* u, uint8_t& bytesLength) {
    uint8_t u0 = u[0];
    if (u0 <= 127) {
        bytesLength = 1;
        return u0;
    }

    uint8_t u1 = u[1];
    if (u0 >= 192 && u0 <= 223) {
        bytesLength = 2;
        return (u0 - 192) * 64 + (u1 - 128);
    }

    uint8_t u2 = u[2];
    if (u0 >= 224 && u0 <= 239) {
        bytesLength = 3;
        return (u0 - 224) * 4096 + (u1 - 128) * 64 + (u2 - 128);
    }

    uint8_t u3  = u[3];
    bytesLength = 4;
    return (u0 - 240) * 262144 + (u1 - 128) * 4096 + (u2 - 128) * 64 + (u3 - 128);
}

inline uint32_t utf8Codepoint(const char* u, uint8_t& bytesLength) {
    return utf8Codepoint(reinterpret_cast<const uint8_t*>(u), bytesLength);
}

constexpr uint32_t EVENT_MASK_FLAGS =
    XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_EXPOSURE
    | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_POINTER_MOTION
    | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;

inline xcb_intern_atom_reply_t* internAtomHelper(xcb_connection_t* conn,
                                                 bool only_if_exists, const char* str) {
    auto cookie = xcb_intern_atom(conn, only_if_exists, strlen(str), str);
    return xcb_intern_atom_reply(conn, cookie, nullptr);
}
}   // namespace

namespace window {
class WindowBackend::Impl {
  private:
    Window* super;

    VideoMode fVideoMode;
    std::string fTitle;

    bool fRunning = false;

    void* fpBmMemory;

    xcb_window_t fpWindow                        = -1u;
    xcb_connection_t* fpConnection               = nullptr;
    xcb_screen_t* fpScreen                       = nullptr;
    xcb_intern_atom_reply_t* fpHintsReply        = nullptr;
    xcb_intern_atom_reply_t* fpProtocolsReply    = nullptr;
    xcb_intern_atom_reply_t* fpDeleteWindowReply = nullptr;
    xkb_context* fpXkbContext                    = nullptr;
    xkb_keymap* fpXkbKeymap                      = nullptr;
    xkb_state* fpXkbState                        = nullptr;
    xkb_compose_table* fpXkbComposeTable         = nullptr;
    xkb_compose_state* fpXkbComposeState         = nullptr;

    Extent2d fExtent;
    Extent2d fExtentBeforeFullscreen;
    WsEvent::MouseMoveData fMousePosition;
    bool fMousePositionToReset = true;

    bool fMouseCurrentlyCentered = false;
    WsEvent::MouseMoveData
        m_mouseMoveAccumulator;   // Used in centerCursor @fixme Rename infinite mode

    std::unique_ptr<BitMap> fpBitMap;

    WsEventDispatcher fEventDispatcher;

    input::KeyboardInput fKeyboardInput;
    input::InputManager fInputManager;

    void pushEvent(WsEvent event) { fEventDispatcher.pushEvent(event); }

  public:
    Impl(Window* super)
        : super(super), fVideoMode(), fTitle(""), fInputManager(fKeyboardInput) {
        init();
        // run();
    }

    Impl(Window* super, VideoMode mode, const std::string name)
        : super(super)
        , fVideoMode(std::move(mode))
        , fTitle(name)
        , fInputManager(fKeyboardInput) {
        init();
        // run();
    }

    ~Impl() = default;

    void create(VideoMode mode, const std::string& title) {
        assert(false && "Not implemented");
        fVideoMode = std::move(mode);
        fTitle     = title;
        init();
        run();
    }
    void create() {
        assert(false && "Not implemented");
        init();
        run();
    }
    void close() {
        // @todo
        assert(false && "Not implemented");
    }

    void update() {
        processEvents();
        fEventDispatcher.dispatchEvents();
        updateWindow();
        updateGemoetry();
    }

    bool opened() const { return fRunning; }

    const VideoMode& videoMode() const { return fVideoMode; }
    input::InputManager& inputManager() { return fInputManager; }
    xcb_connection_t* connection() const { return fpConnection; }
    xcb_window_t window() const { return fpWindow; }

    BitMap& bitMap() {
        if (!fpBitMap) { fpBitMap = std::make_unique<BitMap>(*super); }
        return *fpBitMap;
    }

    void closeBitmap() { fpBitMap.reset(nullptr); }

  private:
    void initXcbConnection() {
        const xcb_setup_t* setup;
        xcb_screen_iterator_t iter;

        int scr;
        fpConnection = xcb_connect(nullptr, &scr);
        if (fpConnection == nullptr) {
            std::cerr << "Could not find a XCB connection.\n" << std::endl;
            exit(1);
        }

        setup = xcb_get_setup(fpConnection);
        iter  = xcb_setup_roots_iterator(setup);
        while (scr-- > 0) xcb_screen_next(&iter);
        fpScreen = iter.data;
    }

    void setupWindow(VideoMode mode, const std::string& title) {
        uint32_t value_mask, value_list[3];

        fpWindow = xcb_generate_id(fpConnection);

        value_mask    = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        value_list[0] = fpScreen->black_pixel;
        value_list[1] = EVENT_MASK_FLAGS;
        value_list[2] = 0;

        xcb_create_window(fpConnection, XCB_COPY_FROM_PARENT, fpWindow, fpScreen->root, 0,
                          0, mode.width(), mode.height(), 0,
                          XCB_WINDOW_CLASS_INPUT_OUTPUT, fpScreen->root_visual,
                          value_mask, value_list);

        // Stored replies
        fpHintsReply        = internAtomHelper(fpConnection, true, "_MOTIF_WM_HINTS");
        fpProtocolsReply    = internAtomHelper(fpConnection, true, "WM_PROTOCOLS");
        fpDeleteWindowReply = internAtomHelper(fpConnection, false, "WM_DELETE_WINDOW");

        // Enable window destroyed notifications
        xcb_change_property(fpConnection, XCB_PROP_MODE_REPLACE, fpWindow,
                            fpProtocolsReply->atom, XCB_ATOM_ATOM, 32, 1,
                            &fpDeleteWindowReply->atom);

        // Set title
        xcb_change_property(fpConnection, XCB_PROP_MODE_REPLACE, fpWindow,
                            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(),
                            title.c_str());

        xcb_map_window(fpConnection, fpWindow);
        xcb_flush(fpConnection);
    }

    void init() {
        fKeyboardInput.registerToEventDispatcher(fEventDispatcher);
        initXcbConnection();
        setupWindow(fVideoMode, fTitle);
        setupXkb();
    }

    void setupXkb() {
        // @note All that knowledge for correct setup comes from
        // - https://github.com/xkbcommon/libxkbcommon/blob/master/doc/quick-guide.md
        // - https://xkbcommon.org/doc/current/group__x11.html
        xkb_x11_setup_xkb_extension(
            fpConnection, XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION,
            XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, nullptr, nullptr);

        // Context
        fpXkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
        if (fpXkbContext == nullptr) {
            std::cerr << "Could not setup XKB context." << std::endl;
        }

        // Keymap
        auto deviceId = xkb_x11_get_core_keyboard_device_id(fpConnection);
        if (deviceId == -1) { std::cerr << "Could not get XKB device ID." << std::endl; }

        fpXkbKeymap = xkb_x11_keymap_new_from_device(fpXkbContext, fpConnection, deviceId,
                                                     XKB_KEYMAP_COMPILE_NO_FLAGS);
        if (fpXkbKeymap == nullptr) {
            std::cerr << "Could not setup XKB keymap." << std::endl;
        }

        // State
        fpXkbState = xkb_x11_state_new_from_device(fpXkbKeymap, fpConnection, deviceId);
        if (fpXkbState == nullptr) {
            std::cerr << "Could not setup XKB state." << std::endl;
        }

        // Compose
        fpXkbComposeTable = xkb_compose_table_new_from_locale(
            fpXkbContext, getenv("LANG"), XKB_COMPOSE_COMPILE_NO_FLAGS);
        if (fpXkbComposeTable == nullptr) {
            std::cerr
                << "Could not setup XKB compose table (wrong LANG environment variable?)."
                << std::endl;
        }

        fpXkbComposeState =
            xkb_compose_state_new(fpXkbComposeTable, XKB_COMPOSE_STATE_NO_FLAGS);
        if (fpXkbComposeState == nullptr) {
            std::cerr
                << "Could not setup XKB compose state (wrong LANG environment variable?)."
                << std::endl;
        }
    }

    input::key::KeyboardKey keysymToKey(xkb_keysym_t keysym) {
        switch (keysym) {
        case XKB_KEY_a:
        case XKB_KEY_A: return input::key::KeyboardKey::A;
        case XKB_KEY_b:
        case XKB_KEY_B: return input::key::KeyboardKey::B;
        case XKB_KEY_c:
        case XKB_KEY_C: return input::key::KeyboardKey::C;
        case XKB_KEY_d:
        case XKB_KEY_D: return input::key::KeyboardKey::D;
        case XKB_KEY_e:
        case XKB_KEY_E: return input::key::KeyboardKey::E;
        case XKB_KEY_f:
        case XKB_KEY_F: return input::key::KeyboardKey::F;
        case XKB_KEY_g:
        case XKB_KEY_G: return input::key::KeyboardKey::G;
        case XKB_KEY_h:
        case XKB_KEY_H: return input::key::KeyboardKey::H;
        case XKB_KEY_i:
        case XKB_KEY_I: return input::key::KeyboardKey::I;
        case XKB_KEY_j:
        case XKB_KEY_J: return input::key::KeyboardKey::J;
        case XKB_KEY_k:
        case XKB_KEY_K: return input::key::KeyboardKey::K;
        case XKB_KEY_l:
        case XKB_KEY_L: return input::key::KeyboardKey::L;
        case XKB_KEY_m:
        case XKB_KEY_M: return input::key::KeyboardKey::M;
        case XKB_KEY_n:
        case XKB_KEY_N: return input::key::KeyboardKey::N;
        case XKB_KEY_o:
        case XKB_KEY_O: return input::key::KeyboardKey::O;
        case XKB_KEY_p:
        case XKB_KEY_P: return input::key::KeyboardKey::P;
        case XKB_KEY_q:
        case XKB_KEY_Q: return input::key::KeyboardKey::Q;
        case XKB_KEY_r:
        case XKB_KEY_R: return input::key::KeyboardKey::R;
        case XKB_KEY_s:
        case XKB_KEY_S: return input::key::KeyboardKey::S;
        case XKB_KEY_t:
        case XKB_KEY_T: return input::key::KeyboardKey::T;
        case XKB_KEY_u:
        case XKB_KEY_U: return input::key::KeyboardKey::U;
        case XKB_KEY_v:
        case XKB_KEY_V: return input::key::KeyboardKey::V;
        case XKB_KEY_w:
        case XKB_KEY_W: return input::key::KeyboardKey::W;
        case XKB_KEY_x:
        case XKB_KEY_X: return input::key::KeyboardKey::X;
        case XKB_KEY_y:
        case XKB_KEY_Y: return input::key::KeyboardKey::Y;
        case XKB_KEY_z:
        case XKB_KEY_Z: return input::key::KeyboardKey::Z;
        case XKB_KEY_BackSpace: return input::key::KeyboardKey::Backspace;
        case XKB_KEY_Escape: return input::key::KeyboardKey::Escape;
        case XKB_KEY_space: return input::key::KeyboardKey::Space;
        case XKB_KEY_F1: return input::key::KeyboardKey::F1;
        case XKB_KEY_F2: return input::key::KeyboardKey::F2;
        case XKB_KEY_F3: return input::key::KeyboardKey::F3;
        case XKB_KEY_F4: return input::key::KeyboardKey::F4;
        case XKB_KEY_F5: return input::key::KeyboardKey::F5;
        case XKB_KEY_F6: return input::key::KeyboardKey::F6;
        case XKB_KEY_F7: return input::key::KeyboardKey::F7;
        case XKB_KEY_F8: return input::key::KeyboardKey::F8;
        case XKB_KEY_F9: return input::key::KeyboardKey::F9;
        case XKB_KEY_F10: return input::key::KeyboardKey::F10;
        case XKB_KEY_F11: return input::key::KeyboardKey::F11;
        case XKB_KEY_F12: return input::key::KeyboardKey::F12;
        case XKB_KEY_Left: return input::key::KeyboardKey::Left;
        case XKB_KEY_Up: return input::key::KeyboardKey::Up;
        case XKB_KEY_Right: return input::key::KeyboardKey::Right;
        case XKB_KEY_Down: return input::key::KeyboardKey::Down;
        case XKB_KEY_Shift_L: return input::key::KeyboardKey::LeftShift;
        case XKB_KEY_Shift_R: return input::key::KeyboardKey::RightShift;
        case XKB_KEY_Control_L: return input::key::KeyboardKey::LeftControl;
        case XKB_KEY_Control_R: return input::key::KeyboardKey::RightControl;
        case XKB_KEY_Alt_L: return input::key::KeyboardKey::LeftAlt;
        case XKB_KEY_Alt_R: return input::key::KeyboardKey::RightAlt;
        case XKB_KEY_Delete: return input::key::KeyboardKey::Delete;
        default: {
            // @note Debug to understand unhandled keys
            // char keysym_name[64];
            // xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));
            // std::cout << keysym_name << ": 0x" << std::hex << keysym << std::endl;
        }
        }

        return input::key::KeyboardKey::Unknown;
    }

    // Process events
    void processEvents() {
        xcb_flush(fpConnection);

        xcb_generic_event_t* event;
        while ((event = xcb_poll_for_event(fpConnection))) {
            processEvent(*event);
            free(event);
        }

        // @todo look at this
        // if (m_mouseKeptCentered && !m_mouseCurrentlyCentered) {
        //     mouseMoveIgnored(true);
        //     xcb_warp_pointer(fpConnection, XCB_NONE, m_window, 0, 0, 0, 0,
        //                      m_extent.width / 2, m_extent.height / 2);
        //     mouseMoveIgnored(false);

        //     m_mouseCurrentlyCentered = true;
        //     m_mousePositionToReset   = true;   // Won't emit delta for the centering
        // }
    }

    void processEvent(xcb_generic_event_t& windowEvent) {
        switch (windowEvent.response_type & 0x7f) {
        case XCB_DESTROY_NOTIFY: {
            break;
        }

        case XCB_CLIENT_MESSAGE: {
            auto messageEvent =
                reinterpret_cast<xcb_client_message_event_t&>(windowEvent);
            if (messageEvent.data.data32[0] != fpDeleteWindowReply->atom) break;

            WsEvent event;
            event.type = WsEventType::WindowClosed;
            pushEvent(event);
            break;
        }

        case XCB_CONFIGURE_NOTIFY: {
            auto configureEvent =
                reinterpret_cast<xcb_configure_notify_event_t&>(windowEvent);
            if (fExtent.width == configureEvent.width
                && fExtent.height == configureEvent.height)
                break;

            WsEvent event;
            event.type              = WsEventType::WindowResized;
            event.windowSize.width  = configureEvent.width;
            event.windowSize.height = configureEvent.height;
            pushEvent(event);

            fExtent.width  = configureEvent.width;
            fExtent.height = configureEvent.height;
            break;
        }

        case XCB_BUTTON_PRESS: {
            auto buttonEvent = reinterpret_cast<xcb_button_press_event_t&>(windowEvent);

            WsEvent event;

            // Classic buttons
            if (buttonEvent.detail <= XCB_BUTTON_INDEX_3) {
                event.type          = WsEventType::MouseButtonPressed;
                event.mouseButton.x = buttonEvent.event_x;
                event.mouseButton.y = buttonEvent.event_y;
                if (buttonEvent.detail == XCB_BUTTON_INDEX_1)
                    event.mouseButton.which = input::key::MouseKey::Left;
                else if (buttonEvent.detail == XCB_BUTTON_INDEX_2)
                    event.mouseButton.which = input::key::MouseKey::Middle;
                else if (buttonEvent.detail == XCB_BUTTON_INDEX_3)
                    event.mouseButton.which = input::key::MouseKey::Right;
            }
            // Vertical mouse wheel buttons
            else if (buttonEvent.detail <= XCB_BUTTON_INDEX_5) {
                event.type             = WsEventType::MouseWheelScrolled;
                event.mouseWheel.which = input::key::WheelKey::Vertical;
                event.mouseWheel.x     = buttonEvent.event_x;
                event.mouseWheel.y     = buttonEvent.event_y;
                event.mouseWheel.delta =
                    (buttonEvent.detail == XCB_BUTTON_INDEX_4) ? 1.f : -1.f;
            }
            // Horizontal mouse wheel buttons
            else if (buttonEvent.detail <= XCB_BUTTON_INDEX_7) {
                event.type             = WsEventType::MouseWheelScrolled;
                event.mouseWheel.which = input::key::WheelKey::Horizontal;
                event.mouseWheel.x     = buttonEvent.event_x;
                event.mouseWheel.y     = buttonEvent.event_y;
                event.mouseWheel.delta =
                    (buttonEvent.detail == XCB_BUTTON_INDEX_4) ? 1.f : -1.f;
            } else {
                // logger.warning("crater.xcb.window")
                //     << "Unknown buttonEvent.detail during BUTTON_PRESS: "
                //     << static_cast<int>(buttonEvent.detail) << std::endl;
                break;
            }
            pushEvent(event);
            break;
        }

        case XCB_BUTTON_RELEASE: {
            auto buttonEvent = reinterpret_cast<xcb_button_release_event_t&>(windowEvent);

            WsEvent event;
            event.type          = WsEventType::MouseButtonReleased;
            event.mouseButton.x = buttonEvent.event_x;
            event.mouseButton.y = buttonEvent.event_y;

            // Classic buttons
            if (buttonEvent.detail == XCB_BUTTON_INDEX_1)
                event.mouseButton.which = input::key::MouseKey::Left;
            else if (buttonEvent.detail == XCB_BUTTON_INDEX_2)
                event.mouseButton.which = input::key::MouseKey::Middle;
            else if (buttonEvent.detail == XCB_BUTTON_INDEX_3)
                event.mouseButton.which = input::key::MouseKey::Right;
            // Mouse wheel buttons
            else if (buttonEvent.detail <= XCB_BUTTON_INDEX_7) {
                // Nothing to do, scrolling is handled in BUTTON_PRESS
                break;
            } else {
                // logger.warning("crater.xcb.window")
                //     << "Unknown buttonEvent.detail during BUTTON_RELEASE: "
                //     << static_cast<int>(buttonEvent.detail) << std::endl;
                break;
            }
            pushEvent(event);
            break;
        }

        case XCB_MOTION_NOTIFY: {
            auto motionEvent = reinterpret_cast<xcb_motion_notify_event_t&>(windowEvent);

            WsEvent event;
            event.type = WsEventType::MouseMoved;

            // @note To prevent big deltas when moving
            // the mouse the first time, we use this flag.
            if (fMousePositionToReset) {
                fMousePosition.x      = motionEvent.event_x;
                fMousePosition.y      = motionEvent.event_y;
                fMousePositionToReset = false;
                return;
            }

            event.mouseMove.x  = motionEvent.event_x;
            event.mouseMove.y  = motionEvent.event_y;
            event.mouseMove.dx = event.mouseMove.x - fMousePosition.x;
            event.mouseMove.dy = event.mouseMove.y - fMousePosition.y;

            fMouseCurrentlyCentered = false;
            fMousePosition.x        = motionEvent.event_x;
            fMousePosition.y        = motionEvent.event_y;

            pushEvent(event);
            break;
        }

        case XCB_KEY_PRESS: {
            auto keyEvent         = reinterpret_cast<xcb_key_press_event_t&>(windowEvent);
            xkb_keycode_t keycode = keyEvent.detail;
            xkb_state_update_key(fpXkbState, keycode, XKB_KEY_DOWN);
            auto keysym = xkb_state_key_get_one_sym(fpXkbState, keycode);

            WsEvent event;
            event.type      = WsEventType::KeyPressed;
            event.key.which = keysymToKey(keysym);
            pushEvent(event);

            // We have two state machines running: one for dead-key composing
            // and one classical. We use the unicode codepoint of the dead-key composing
            // one if it has any result.
            uint32_t codepoint;
            auto composeFeedResult  = xkb_compose_state_feed(fpXkbComposeState, keysym);
            auto composeStateStatus = xkb_compose_state_get_status(fpXkbComposeState);
            if (composeFeedResult == XKB_COMPOSE_FEED_ACCEPTED
                && composeStateStatus == XKB_COMPOSE_COMPOSED) {
                char c[5] = { 0 };
                uint8_t bytesLength;
                bytesLength = xkb_compose_state_get_utf8(fpXkbComposeState, c, 4u);
                codepoint   = utf8Codepoint(c, bytesLength);
            } else if (composeStateStatus != XKB_COMPOSE_NOTHING) {
                // 1) XKB_COMPOSE_CANCELLED
                // Example: typing <dead_acute> <b> cancels the composition,
                // in that case, we don't want to emit <b> codepoint.
                // 2) XKB_COMPOSE_COMPOSING
                // No need to produce a codepoint in that case.
                codepoint = 0u;
            } else {
                codepoint = xkb_state_key_get_utf32(fpXkbState, keycode);
            }

            // @note ASCII below 32 (SPACE) is non-text
            // and 127 (DELETE) should be ignored too.
            if (codepoint > 31u && codepoint != 127u) {
                WsEvent event;
                event.type           = WsEventType::TextEntered;
                event.text.codepoint = codepoint;
                pushEvent(event);
            }
        } break;

        case XCB_KEY_RELEASE: {
            auto keyEvent = reinterpret_cast<xcb_key_release_event_t&>(windowEvent);
            xkb_keycode_t keycode = keyEvent.detail;
            xkb_state_update_key(fpXkbState, keycode, XKB_KEY_UP);

            auto keysym = xkb_state_key_get_one_sym(fpXkbState, keycode);

            WsEvent event;
            event.type      = WsEventType::KeyReleased;
            event.key.which = keysymToKey(keysym);
            pushEvent(event);
        } break;

        default: break;
        }
    }

    void allocateBuffer(uint8_t** buffer) {
        if (*buffer) {
            delete *buffer;
            *buffer = new uint8_t[fVideoMode.pixelBytes()];
        } else {
            *buffer = new uint8_t[fVideoMode.pixelBytes()];
        }
    }

    void resizeDrawable(uint32_t width, uint32_t height) {
        bitMap().backend().resize(width, height);
    }

    void updateWindow() { bitMap().flush(); }
    void updateGemoetry() {
        auto geom = xcb_get_geometry_reply(
            fpConnection, xcb_get_geometry(fpConnection, fpWindow), nullptr);
        resizeWindow(geom->width, geom->height);
    }

    void resizeWindow(uint32_t width, uint32_t height) {
        fVideoMode.width()  = width;
        fVideoMode.height() = height;
        resizeDrawable(fVideoMode.width(), fVideoMode.height());
    }

    void run() {

        // BitMapImpl bm{fVideoMode, fpConnection, fpWindow};
        // auto& bm = bitMap();
        // xcb_generic_event_t* event;

        // WsEventDispatcher eventDispatcher;

        fInputManager.addMapping("up", input::Input(input::key::KeyboardKey::Up));
        fInputManager.addMapping("down", input::Input(input::key::KeyboardKey::Down));
        fInputManager.addMapping("fastUp", { input::key::KeyboardKey::LeftShift,
                                             input::key::KeyboardKey::Up });
        fInputManager.addMapping("fastDown", { input::key::KeyboardKey::LeftShift,
                                               input::key::KeyboardKey::Down });

        int xo = 0, yo = 0;
        while (true) {
            processEvents();
            fEventDispatcher.dispatchEvents();

            {
                auto geom = xcb_get_geometry_reply(
                    fpConnection, xcb_get_geometry(fpConnection, fpWindow), nullptr);
                resizeWindow(geom->width, geom->height);
            }

            test::renderWeirdGradient(bitMap(), xo, yo);
            updateWindow();

            ++xo;
            if (fInputManager.isActive("fastUp")) {
                std::cout << "fastUp" << std::endl;
                yo += 3;
            } else if (fInputManager.isActive("fastDown")) {
                std::cout << "fastDown" << std::endl;
                yo -= 3;
            } else if (fInputManager.isActive("up")) {
                std::cout << "up" << std::endl;
                yo += 1;
            } else if (fInputManager.isActive("down")) {
                std::cout << "down" << std::endl;
                yo -= 1;
            }
        }
    }
};

// Pimpl declarations
$pimpl_class(WindowBackend, Window*, super);
$pimpl_class(WindowBackend, Window*, super, VideoMode, mode, const std::string&, title);
$pimpl_class_delete(WindowBackend);

$pimpl_method(WindowBackend, void, create);
$pimpl_method(WindowBackend, void, create, VideoMode, mode, const std::string&, title);
$pimpl_method(WindowBackend, void, close);
$pimpl_method(WindowBackend, void, update);
$pimpl_method(WindowBackend, BitMap&, bitMap);
$pimpl_method(WindowBackend, void, closeBitmap);
$pimpl_method_const(WindowBackend, bool, opened);
$pimpl_method_const(WindowBackend, const VideoMode&, videoMode);
$pimpl_method(WindowBackend, input::InputManager&, inputManager);
$pimpl_method_const(WindowBackend, xcb_connection_t*, connection);
$pimpl_method_const(WindowBackend, xcb_window_t, window);

}   // namespace window
