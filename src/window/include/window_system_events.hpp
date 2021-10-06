#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <queue>

#include <macros/macros.hpp>
#include <window/keys.hpp>

$enum_class(window, WsEventType,
            WindowClosed,          // Window is being closed
            WindowResized,         // Window is being resized
            MouseButtonPressed,    // Mouse button has been pressed
            MouseButtonReleased,   // Mouse button has been released
            MouseWheelScrolled,    // Mouse wheel has been scrolled
            MouseMoved,            // Mouse moved
            KeyPressed,            // Keyboard key has been pressed
            KeyReleased,           // Keyboard key has been released
            TextEntered,           // Keyboard has generated a unicode codepoint
);

namespace window {

// @todo find a better way

struct WsEvent {
  public:
    struct WindowSizeData {
        uint16_t width;
        uint16_t height;
    };

    struct MouseButtonData {
        int16_t x;
        int16_t y;
        input::key::MouseKey which;
    };

    struct MouseMoveData {
        int16_t x;
        int16_t y;
        int16_t dx;
        int16_t dy;
    };

    struct MouseWheelData {
        int16_t x;
        int16_t y;
        input::key::WheelKey which;

        // Standard delta is 1.f, but hight precision mice (ou touchpads)
        // might be non-integers.
        float delta;
    };

    struct KeyData {
        input::key::KeyboardKey which;
    };

    struct TextData {
        uint32_t codepoint;   // Unicode codepoint
    };

    // -----

    WsEventType type;
    union {
        WindowSizeData windowSize;
        MouseButtonData mouseButton;
        MouseMoveData mouseMove;
        MouseWheelData mouseWheel;
        KeyData key;
        TextData text;
    };
};

class WsEventDispatcher {
    std::queue<WsEvent> fEventQueue;
    std::map<WsEventType, std::function<void(WsEvent)>> fHandlerMap;

    bool dispatchEvent() {
        if (!fEventQueue.empty()) {
            auto& event = fEventQueue.front();
            auto it     = fHandlerMap.find(event.type);
            if (it == fHandlerMap.end()) {
                fEventQueue.pop();
                return true;
            }
            auto& handler = it->second;
            handler(event);
            fEventQueue.pop();
            return true;
        } else {
            return false;
        }
    }

  public:
    void pushEvent(WsEvent event) { fEventQueue.push(event); }
    void registerHandler(WsEventType eventType, std::function<void(WsEvent)> handler) {
        fHandlerMap[eventType] = handler;
    }

    void dispatchEvents(size_t nEvents = 0) {
        if (nEvents == 0) {
            while (dispatchEvent()) {}
        } else {
            for (int i = 0; i < nEvents; i++) {
                if (!dispatchEvent()) break;
            }
        }
    }
};

}   // namespace window
