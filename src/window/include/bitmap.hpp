#pragma once

#include <cstdint>
#include <memory>

#include <window/video_mode.hpp>
#include <window/window.hpp>

namespace window {

class Window;

class BitMap {
  public:
    BitMap(Window&);
    virtual ~BitMap();

    virtual uint32_t* data();
    virtual void flush();
    virtual const VideoMode& mode() const;

    // private:
    //   virtual void resize(uint32_t width, uint32_t height) = 0;

  public:
    class Backend;
    Backend& backend() { return *fpBackend; }

  private:
    std::unique_ptr<Backend> fpBackend;
};
}   // namespace window
