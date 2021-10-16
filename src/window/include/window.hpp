#pragma once

#include <memory>
#include <string>

#include <window/bitmap.hpp>
#include <window/input.hpp>
#include <window/video_mode.hpp>

namespace window {

class BitMap;

class Window {
  public:
    // @todo make this an enum class
    enum Flag {
        NO_OPTION = 0,
        NO_RESIZE = 1,
        DEFAULT   = NO_OPTION,
    };

    Window();
    Window(VideoMode mode, const std::string& title, Flag options = DEFAULT);
    virtual ~Window();

    virtual void create();
    virtual void create(VideoMode mode, const std::string& title);
    virtual void close();

    virtual bool opened() const;
    virtual const VideoMode& videoMode() const;

    virtual BitMap& bitMap();
    virtual void closeBitmap();
    virtual input::InputManager& inputManager();

    virtual void update();

  public:
    class Backend;
    Backend& backend() { return *fpBackend; }

  private:
    std::unique_ptr<Backend> fpBackend;
};
}   // namespace window
