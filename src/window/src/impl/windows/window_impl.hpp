#pragma once

#include <memory>

#include <window/window.hpp>
#include <window/video_mode.hpp>

#include <macros/macros.hpp>

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

private:
    $pimpl_decl
};

}
