#include <iostream>
#include <memory>

#include <window/input.hpp>
#include <window/window.hpp>

#ifdef __USE_WINDOWS__
#    include "backends/windows/window_backend.hpp"
#else
#    include "backends/linux/window_backend.hpp"
#endif

namespace window {

Window::Window() { fpBackend = std::make_unique<WindowBackend>(this); }

Window::Window(VideoMode mode, const std::string& title, Flag options) {
    fpBackend = std::make_unique<WindowBackend>(this, mode, title, options);
}

Window::~Window() {}

void Window::create() { fpBackend->create(); }
void Window::create(VideoMode mode, const std::string& title) {
    fpBackend->create(mode, title);
}
void Window::close() { fpBackend->close(); }

bool Window::opened() const { return fpBackend->opened(); }
const VideoMode& Window::videoMode() const { return fpBackend->videoMode(); }
input::InputManager& Window::inputManager() { return fpBackend->inputManager(); }

BitMap& Window::bitMap() { return fpBackend->bitMap(); }
void Window::closeBitmap() { fpBackend->closeBitmap(); }

void Window::update() { fpBackend->update(); }

}   // namespace window
