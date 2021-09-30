#pragma once

#include <memory>

#include <macros/macros.hpp>

#include <window/window.hpp>

namespace window {

class WindowImpl final : public Window {
public:
    WindowImpl();
    WindowImpl(VideoMode mode, const std::string& title);
    virtual ~WindowImpl();

    void create();
    void create(VideoMode mode, const std::string& title);
    void close();

    bool opened() const;
    const VideoMode& videoMode() const;

    BitMap& bitMap();
    void closeBitmap();

private:
    $pimpl_decl;
};

}
