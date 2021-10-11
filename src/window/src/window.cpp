#include <iostream>
#include <memory>

#include <window/window.hpp>

#ifdef __USE_WINDOWS__
#    include "impl/windows/window_impl.hpp"
#else
#    include "backends/linux/window_backend.hpp"
#endif

namespace window {

Window::Window() { fpBackend = std::make_unique<WindowBackend>(this); }

Window::Window(VideoMode mode, const std::string& title) {
    fpBackend = std::make_unique<WindowBackend>(this, mode, title);
}

Window::~Window() {}

void Window::create() { fpBackend->create(); }
void Window::create(VideoMode mode, const std::string& title) {
    fpBackend->create(mode, title);
}
void Window::close() { fpBackend->close(); }

bool Window::opened() const { return fpBackend->opened(); }
const VideoMode& Window::videoMode() const { return fpBackend->videoMode(); }

BitMap& Window::bitMap() { return fpBackend->bitMap(); }
void Window::closeBitmap() { fpBackend->closeBitmap(); }

// std::shared_ptr<Window> WindowBuilder::build() {
//   return std::make_shared<WindowImpl>();
// }

// void test() {
//     //   window::WindowBuilder wb;
//     // auto win = wb.build();
//     // win->run();
//     WindowImpl win{ { 1280, 720 }, "My awesome title" };
// }

}   // namespace window
