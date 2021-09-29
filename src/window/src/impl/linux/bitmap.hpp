#pragma once

#include <cstdlib>

#include <window/video_mode.hpp>

#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xcb/xcb_image.h>

#include <sys/shm.h>
#include <sys/ipc.h>

#include <xcb/shm.h>

namespace window {
class BitMap {
private:
    VideoMode fMode;
    xcb_connection_t* fpConnection;
    xcb_window_t fpWindow;
    xcb_shm_segment_info_t fInfo;
    xcb_pixmap_t fPix;
    xcb_gcontext_t fGcontext;

    uint32_t* fpData;
public:
    BitMap(VideoMode mode, xcb_connection_t* conn, xcb_window_t win)
        : fMode(mode)
        , fpConnection(conn)
        , fpWindow(win) {

        xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(fpConnection)).data;
        uint32_t value_mask;
        uint32_t value_list[2];
        value_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
        value_list[0] = screen->black_pixel;
        value_list[1] = 0;

        fGcontext = xcb_generate_id(fpConnection);
        xcb_create_gc(fpConnection, fGcontext, fpWindow, value_mask, value_list);

        // xcb_map_window(fpConnection, fpWindow);
        xcb_flush(fpConnection);

        xcb_shm_query_version_reply_t*  reply;

        reply = xcb_shm_query_version_reply(
            fpConnection,
            xcb_shm_query_version(fpConnection),
            nullptr);

        if(!reply || !reply->shared_pixmaps){
            printf("Shm error...\n");
            exit(0);            // @todo do something here
        }

        fInfo.shmid = shmget(IPC_PRIVATE, fMode.pixelBytes(), IPC_CREAT | 0777);
        fInfo.shmaddr = (uint8_t*)shmat(fInfo.shmid, nullptr, 0);

        fInfo.shmseg = xcb_generate_id(fpConnection);

        xcb_shm_attach(fpConnection, fInfo.shmseg, fInfo.shmid, 0);
        shmctl(fInfo.shmid, IPC_RMID, 0);

        fpData = (uint32_t*)fInfo.shmaddr;

        fPix = xcb_generate_id(fpConnection);
        xcb_shm_create_pixmap(
            fpConnection,
            fPix,
            fpWindow,
            fMode.width(), fMode.height(),
            screen->root_depth,
            fInfo.shmseg,
            0);

        // uint32_t* pixel = fpData;
        // for (int y = 0; y < fMode.height(); ++y) {
        //     for (int x = 0; x < fMode.width(); ++x) {
        //         uint8_t blue = x;
        //         uint8_t green = y;
        //         *pixel = (green << 8) | blue;
        //         ++pixel;
        //     }
        // }

        // xcb_copy_area(fpConnection, fPix, fpWindow, gcontext, 0, 0, 0, 0,
        //               fMode.width(), fMode.height());

        // xcb_flush(fpConnection);
    }
    ~BitMap() {
        xcb_shm_detach(fpConnection, fInfo.shmseg);
        shmdt(fInfo.shmaddr);

        xcb_free_pixmap(fpConnection, fPix);
        fpData = nullptr;
    }

    uint32_t* data() { return fpData; }

    void flush() {
        xcb_copy_area(fpConnection, fPix, fpWindow, fGcontext, 0, 0, 0, 0,
                      fMode.width(), fMode.height());

        xcb_flush(fpConnection);
    }

    void resize(uint32_t width, uint32_t height) {
        fMode.width() = width;
        fMode.height() = height;

        xcb_shm_detach(fpConnection, fInfo.shmseg);
        shmdt(fInfo.shmaddr);

        xcb_free_pixmap(fpConnection, fPix);

        xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(fpConnection)).data;

        fInfo.shmid = shmget(IPC_PRIVATE, fMode.pixelBytes(), IPC_CREAT | 0777);
        fInfo.shmaddr = (uint8_t*)shmat(fInfo.shmid, nullptr, 0);

        fInfo.shmseg = xcb_generate_id(fpConnection);

        xcb_shm_attach(fpConnection, fInfo.shmseg, fInfo.shmid, 0);
        shmctl(fInfo.shmid, IPC_RMID, 0);

        fpData = (uint32_t*)fInfo.shmaddr;

        fPix = xcb_generate_id(fpConnection);
        xcb_shm_create_pixmap(
            fpConnection,
            fPix,
            fpWindow,
            fMode.width(), fMode.height(),
            screen->root_depth,
            fInfo.shmseg,
            0);
    }

};
}
