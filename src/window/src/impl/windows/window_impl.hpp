#pragma once

#include <memory>
#include <window/window.hpp>
#include <macros/macros.hpp>

namespace window {

class WindowImpl : public Window {
public:
    WindowImpl();
    virtual ~WindowImpl();
    void run() final;
private:
    $pimpl_decl
};

}
