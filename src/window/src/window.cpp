#include <iostream>
#include "window/window.hpp"

#ifdef __USE_WINDOWS__
#include "impl/windows/window_impl.hpp"
#else
#include "impl/linux/window_impl.hpp"
#endif

namespace window {

std::shared_ptr<Window> WindowBuilder::build() {
  return std::make_shared<WindowImpl>();
}

void test() {
  window::WindowBuilder wb;
  auto win = wb.build();
  win->run();
}

}
