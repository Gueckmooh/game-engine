#pragma once

#include <windows.h>
#include <iostream>

namespace window {

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
