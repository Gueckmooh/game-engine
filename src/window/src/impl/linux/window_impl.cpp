#include "window_impl.hpp"

#include <macros/macros.hpp>

#include "bitmap.hpp"

#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xcb/xcb_image.h>

#include <sys/shm.h>
#include <sys/ipc.h>

#include <xcb/shm.h>

#include <thread>
#include <chrono>

#include <iostream>
#include <cstring>
#include <cmath>


#include "../../genericManip.hpp"

namespace {
constexpr uint32_t EVENT_MASK_FLAGS = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_KEY_PRESS |
    XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
    XCB_EVENT_MASK_POINTER_MOTION |
    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;//  |
    // XCB_EVENT_MASK_RESIZE_REDIRECT;

inline xcb_intern_atom_reply_t* internAtomHelper(xcb_connection_t* conn, bool only_if_exists, const char* str)
    {
        auto cookie = xcb_intern_atom(conn, only_if_exists, strlen(str), str);
        return xcb_intern_atom_reply(conn, cookie, nullptr);
    }
}

namespace window {
class WindowImpl::Impl {
private:
    VideoMode fVideoMode;
    std::string fTitle;

    bool fRunning = false;

    void* fpBmMemory;

    xcb_window_t fpWindow = -1u;
    xcb_connection_t* fpConnection = nullptr;
    xcb_screen_t* fpScreen = nullptr;
    xcb_intern_atom_reply_t* fpHintsReply = nullptr;
    xcb_intern_atom_reply_t* fpProtocolsReply = nullptr;
    xcb_intern_atom_reply_t* fpDeleteWindowReply = nullptr;
    xkb_context* fpXkbContext = nullptr;
    xkb_keymap* fpXkbKeymap = nullptr;
    xkb_state* fpXkbState = nullptr;
    xkb_compose_table* fpXkbComposeTable = nullptr;
    xkb_compose_state* fpXkbComposeState = nullptr;

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
        std::cout << "Called too early ?" << std::endl;
        if (!fpBitMap) {
            fpBitMap = std::make_unique<BitMapImpl>(fVideoMode, fpConnection, fpWindow);
        }
        return *fpBitMap;
    }

    void closeBitmap() {
        fpBitMap.reset(nullptr);
    }

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
        iter = xcb_setup_roots_iterator(setup);
        while (scr-- > 0) xcb_screen_next(&iter);
        fpScreen = iter.data;
    }

    void setupWindow(VideoMode mode, const std::string& title) {
        uint32_t value_mask, value_list[3];

        fpWindow = xcb_generate_id(fpConnection);

        value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        value_list[0] = fpScreen->black_pixel;
        value_list[1] = EVENT_MASK_FLAGS;
        value_list[2] = 0;

        xcb_create_window(fpConnection, XCB_COPY_FROM_PARENT, fpWindow, fpScreen->root, 0, 0, mode.width(), mode.height(), 0,
                          XCB_WINDOW_CLASS_INPUT_OUTPUT, fpScreen->root_visual, value_mask, value_list);

        // Stored replies
        fpHintsReply = internAtomHelper(fpConnection, true, "_MOTIF_WM_HINTS");
        fpProtocolsReply = internAtomHelper(fpConnection, true, "WM_PROTOCOLS");
        fpDeleteWindowReply = internAtomHelper(fpConnection, false, "WM_DELETE_WINDOW");

        // Enable window destroyed notifications
        xcb_change_property(fpConnection, XCB_PROP_MODE_REPLACE, fpWindow, fpProtocolsReply->atom, XCB_ATOM_ATOM, 32, 1,
                            &fpDeleteWindowReply->atom);

        // Set title
        xcb_change_property(fpConnection, XCB_PROP_MODE_REPLACE, fpWindow, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(),
                            title.c_str());

        xcb_map_window(fpConnection, fpWindow);
        xcb_flush(fpConnection);
    }



    void init() {
        initXcbConnection();
        setupWindow(fVideoMode, fTitle);
        setupXkb();
    }

    void setupXkb() {
        // @note All that knowledge for correct setup comes from
        // - https://github.com/xkbcommon/libxkbcommon/blob/master/doc/quick-guide.md
        // - https://xkbcommon.org/doc/current/group__x11.html
        xkb_x11_setup_xkb_extension(fpConnection, XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION,
                                    XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, nullptr, nullptr);

        // Context
        fpXkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
        if (fpXkbContext == nullptr) {
            std::cerr << "Could not setup XKB context." << std::endl;
        }

        // Keymap
        auto deviceId = xkb_x11_get_core_keyboard_device_id(fpConnection);
        if (deviceId == -1) {
            std::cerr << "Could not get XKB device ID." << std::endl;
        }

        fpXkbKeymap = xkb_x11_keymap_new_from_device(fpXkbContext, fpConnection, deviceId, XKB_KEYMAP_COMPILE_NO_FLAGS);
        if (fpXkbKeymap == nullptr) {
            std::cerr << "Could not setup XKB keymap." << std::endl;
        }

        // State
        fpXkbState = xkb_x11_state_new_from_device(fpXkbKeymap, fpConnection, deviceId);
        if (fpXkbState == nullptr) {
            std::cerr << "Could not setup XKB state." << std::endl;
        }

        // Compose
        fpXkbComposeTable = xkb_compose_table_new_from_locale(fpXkbContext, getenv("LANG"), XKB_COMPOSE_COMPILE_NO_FLAGS);
        if (fpXkbComposeTable == nullptr) {
            std::cerr << "Could not setup XKB compose table (wrong LANG environment variable?)." << std::endl;
        }

        fpXkbComposeState = xkb_compose_state_new(fpXkbComposeTable, XKB_COMPOSE_STATE_NO_FLAGS);
        if (fpXkbComposeState == nullptr) {
            std::cerr << "Could not setup XKB compose state (wrong LANG environment variable?)." << std::endl;
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
        static_cast<BitMapImpl&>(bitMap()).resize(width, height);
    }

    void updateWindow() {
        bitMap().flush();
    }

    void resizeWindow(uint32_t width, uint32_t height) {
                    fVideoMode.width() = width;
            fVideoMode.height() = height;
            resizeDrawable(fVideoMode.width(), fVideoMode.height());
    }

    void run() {

      // BitMapImpl bm{fVideoMode, fpConnection, fpWindow};
        // auto& bm = bitMap();
      xcb_generic_event_t *event;
      int xo = 0, yo = 0;
      while (true) {
        {
            auto geom = xcb_get_geometry_reply(
                fpConnection, xcb_get_geometry(fpConnection, fpWindow), nullptr);
            resizeWindow(geom->width, geom->height);
        }

        test::renderWeirdGradient(bitMap(), xo, yo);
        updateWindow();

        ++xo;
        ++yo;

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
