#pragma once

#include <memory>

namespace window {

class Window {
public:
  Window() = default;
  virtual ~Window() = default;
  virtual void run() = 0;
};

class WindowBuilder {
public:
  std::shared_ptr<Window> build();
};

}
