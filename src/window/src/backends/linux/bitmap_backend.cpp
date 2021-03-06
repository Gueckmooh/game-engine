#include "bitmap_backend.hpp"

#include <cstring>

namespace window {
class BitMapBackend::Impl {
  private:
    VideoMode fMode;
    xcb_connection_t* fpConnection;
    xcb_window_t fpWindow;
    xcb_shm_segment_info_t fInfo;
    xcb_pixmap_t fPix;
    xcb_gcontext_t fGcontext;

    uint32_t* fpData;
    // @XXX: this is a workaround to not have screen to do crazy stuff
    // @note it looks like it is the good solution
    uint32_t* fpUserData;

  public:
    friend class BitMapBackend;

    Impl(Window::Backend& window) {
        WindowBackend& windowBackend = window.backend();
        fMode                        = windowBackend.videoMode();
        fpConnection                 = windowBackend.connection();
        fpWindow                     = windowBackend.window();
        init();
    }

    Impl(VideoMode mode, xcb_connection_t* conn, xcb_window_t win)
        : fMode(mode), fpConnection(conn), fpWindow(win) {
        init();
    }

    ~Impl() {
        xcb_shm_detach(fpConnection, fInfo.shmseg);
        shmdt(fInfo.shmaddr);

        xcb_free_pixmap(fpConnection, fPix);
        fpData = nullptr;

        delete (uint8_t*)fpUserData;
    }

    uint32_t* data() { return fpUserData; }
    const VideoMode& mode() const { return fMode; };

    void flush() {
        memcpy(fpData, fpUserData, fMode.pixelBytes());

        xcb_copy_area(fpConnection, fPix, fpWindow, fGcontext, 0, 0, 0, 0, fMode.width(),
                      fMode.height());

        xcb_flush(fpConnection);
    }

  private:
    void resize(uint32_t width, uint32_t height) {
        fMode.width()  = width;
        fMode.height() = height;

        xcb_shm_detach(fpConnection, fInfo.shmseg);
        shmdt(fInfo.shmaddr);

        xcb_free_pixmap(fpConnection, fPix);

        xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(fpConnection)).data;

        fInfo.shmid   = shmget(IPC_PRIVATE, fMode.pixelBytes(), IPC_CREAT | 0777);
        fInfo.shmaddr = (uint8_t*)shmat(fInfo.shmid, nullptr, 0);

        fInfo.shmseg = xcb_generate_id(fpConnection);

        xcb_shm_attach(fpConnection, fInfo.shmseg, fInfo.shmid, 0);
        shmctl(fInfo.shmid, IPC_RMID, 0);

        fpData = (uint32_t*)fInfo.shmaddr;

        fPix = xcb_generate_id(fpConnection);
        xcb_shm_create_pixmap(fpConnection, fPix, fpWindow, fMode.width(), fMode.height(),
                              screen->root_depth, fInfo.shmseg, 0);
    }

    void init() {
        xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(fpConnection)).data;
        uint32_t value_mask;
        uint32_t value_list[2];
        value_mask    = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
        value_list[0] = screen->black_pixel;
        value_list[1] = 0;

        fGcontext = xcb_generate_id(fpConnection);
        xcb_create_gc(fpConnection, fGcontext, fpWindow, value_mask, value_list);

        // xcb_map_window(fpConnection, fpWindow);
        xcb_flush(fpConnection);

        xcb_shm_query_version_reply_t* reply;

        reply = xcb_shm_query_version_reply(fpConnection,
                                            xcb_shm_query_version(fpConnection), nullptr);

        if (!reply || !reply->shared_pixmaps) {
            printf("Shm error...\n");
            exit(0);   // @todo do something here
        }

        fInfo.shmid   = shmget(IPC_PRIVATE, fMode.pixelBytes(), IPC_CREAT | 0777);
        fInfo.shmaddr = (uint8_t*)shmat(fInfo.shmid, nullptr, 0);

        fInfo.shmseg = xcb_generate_id(fpConnection);

        xcb_shm_attach(fpConnection, fInfo.shmseg, fInfo.shmid, 0);
        shmctl(fInfo.shmid, IPC_RMID, 0);

        fpData     = (uint32_t*)fInfo.shmaddr;
        fpUserData = (uint32_t*)new uint8_t[fMode.pixelBytes()];

        fPix = xcb_generate_id(fpConnection);
        xcb_shm_create_pixmap(fpConnection, fPix, fpWindow, fMode.width(), fMode.height(),
                              screen->root_depth, fInfo.shmseg, 0);
    }
};

$pimpl_class(BitMapBackend, Window::Backend&, window);
$pimpl_class(BitMapBackend, VideoMode, mode, xcb_connection_t*, conn, xcb_window_t, win);
$pimpl_class_delete(BitMapBackend);

$pimpl_method(BitMapBackend, uint32_t*, data);
$pimpl_method(BitMapBackend, void, flush);
$pimpl_method_const(BitMapBackend, const VideoMode&, mode);
$pimpl_method(BitMapBackend, void, resize, uint32_t, width, uint32_t, height);
}   // namespace window
