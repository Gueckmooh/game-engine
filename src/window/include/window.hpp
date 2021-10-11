#pragma once

#include <memory>
#include <string>

#include <window/video_mode.hpp>
#include <window/bitmap.hpp>

namespace window {

class BitMap;

class Window {
  public:
    Window();
    Window(VideoMode mode, const std::string& title);
    virtual ~Window();

    virtual void create();
    virtual void create(VideoMode mode, const std::string& title);
    virtual void close();

    virtual bool opened() const;
    virtual const VideoMode& videoMode() const;

    virtual BitMap& bitMap();
    virtual void closeBitmap();

  public:
    class Backend;
    Backend& backend() { return *fpBackend; }

  private:
    std::unique_ptr<Backend> fpBackend;
};
}
