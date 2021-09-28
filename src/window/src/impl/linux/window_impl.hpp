#pragma once

#include <memory>
#include "window/window.hpp"

namespace window {

class WindowImpl : public Window {
public:
  WindowImpl() = default;
  virtual ~WindowImpl() = default;
  void run() final;
};

}
