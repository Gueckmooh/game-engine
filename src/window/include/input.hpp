#pragma once

#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <macros/macros.hpp>

namespace window {
namespace input {

#define KeyboardKeys                                                                     \
    Unknown,          /* Unknown key */                                                  \
        A,            /* A key */                                                        \
        B,            /* B key */                                                        \
        C,            /* C key */                                                        \
        D,            /* D key */                                                        \
        E,            /* E key */                                                        \
        F,            /* F key */                                                        \
        G,            /* G key */                                                        \
        H,            /* H key */                                                        \
        I,            /* I key */                                                        \
        J,            /* J key */                                                        \
        K,            /* K key */                                                        \
        L,            /* L key */                                                        \
        M,            /* M key */                                                        \
        N,            /* N key */                                                        \
        O,            /* O key */                                                        \
        P,            /* P key */                                                        \
        Q,            /* Q key */                                                        \
        R,            /* R key */                                                        \
        S,            /* S key */                                                        \
        T,            /* T key */                                                        \
        U,            /* U key */                                                        \
        V,            /* V key */                                                        \
        W,            /* W key */                                                        \
        X,            /* X key */                                                        \
        Y,            /* Y key */                                                        \
        Z,            /* Z key */                                                        \
        Left,         /* Left key */                                                     \
        Right,        /* Right key */                                                    \
        Up,           /* Up key */                                                       \
        Down,         /* Down key */                                                     \
        F1,           /* F1 key */                                                       \
        F2,           /* F2 key */                                                       \
        F3,           /* F3 key */                                                       \
        F4,           /* F4 key */                                                       \
        F5,           /* F5 key */                                                       \
        F6,           /* F6 key */                                                       \
        F7,           /* F7 key */                                                       \
        F8,           /* F8 key */                                                       \
        F9,           /* F9 key */                                                       \
        F10,          /* F10 key */                                                      \
        F11,          /* F11 key */                                                      \
        F12,          /* F12 key */                                                      \
        Escape,       /* Escape key */                                                   \
        LeftAlt,      /* LeftAlt key */                                                  \
        RightAlt,     /* RightAlt key */                                                 \
        LeftControl,  /* LeftControl key */                                              \
        RightControl, /* RightControl key */                                             \
        LeftShift,    /* LeftShift key */                                                \
        RightShift,   /* RightShift key */                                               \
        Delete,       /* Delete key */                                                   \
        Backspace,    /* Backspace key */

#define ControllerKeys                                                                   \
    Unknown,           /* Unknown */                                                     \
        DPadLeft,      /* Left */                                                        \
        DPadRight,     /* Right */                                                       \
        DPadUp,        /* Up */                                                          \
        DPadDown,      /* Down */                                                        \
        Start,         /* Start */                                                       \
        Back,          /* Back */                                                        \
        LeftThumb,     /* Left Thumb */                                                  \
        RightThumb,    /* Right Thumb */                                                 \
        LeftShoulder,  /* Left Shoulder */                                               \
        RightShoulder, /* Right Shoulder */                                              \
        A,             /* A */                                                           \
        B,             /* B */                                                           \
        X,             /* C */                                                           \
        Y,             /* D */

$enum_class(key, KeyboardKey, KeyboardKeys);
$enum_class(key, ControllerKey, ControllerKeys);

class Input {
  public:
    using InputKindType = uint8_t;
    enum class InputKind : InputKindType { ControllerInput, KeyboardInput, Unknown };
    using InputUnion = union {
        key::ControllerKey controllerKey;
        key::KeyboardKey keyboardKey;
    };

  private:
    InputKind fKind;
    InputUnion fInput;

  public:
    key::KeyboardKey keyboardKey() {
        if (fKind == InputKind::KeyboardInput) {
            return fInput.keyboardKey;
        } else {
            return key::KeyboardKey::Unknown;
        }
    }

    key::ControllerKey controllerKey() {
        if (fKind == InputKind::ControllerInput) {
            return fInput.controllerKey;
        } else {
            return key::ControllerKey::Unknown;
        }
    }

    Input(key::ControllerKey key) : fKind(InputKind::ControllerInput) {
        fInput.controllerKey = key;
    }
    Input(key::KeyboardKey key) : fKind(InputKind::KeyboardInput) {
        fInput.keyboardKey = key;
    }
    Input() : fKind(InputKind::KeyboardInput) {}

    void operator=(key::ControllerKey key) {
        fKind                = InputKind::ControllerInput;
        fInput.controllerKey = key;
    }
    void operator=(key::KeyboardKey key) {
        fKind              = InputKind::KeyboardInput;
        fInput.keyboardKey = key;
    }

    friend std::ostream& operator<<(std::ostream& os, Input& input) {
        switch (input.fKind) {
        case Input::InputKind::KeyboardInput: os << input.fInput.keyboardKey; break;
        case Input::InputKind::ControllerInput: os << input.fInput.controllerKey; break;
        default: os << "<unknown>"; break;
        }
        return os;
    }
};

class ControllerInput {
    $enum_to_vars_decls(f, ControllerKeys);

  public:
    $enum_to_vars_getter(getKey, key, ControllerKey, f, ControllerKeys);
    $enum_to_vars_setter(setKey, key, ControllerKey, f, ControllerKeys);

    virtual void readInput() = 0;
};

class InputCombination {
    std::vector<Input> fInputVector;
    Input fSimpleInput;
    bool fIsSimpleInput;

  public:
    InputCombination(Input input) : fSimpleInput(input), fIsSimpleInput(true) {}
    InputCombination(std::initializer_list<Input> inputs) : fIsSimpleInput(false) {
        fInputVector.insert(fInputVector.begin(), inputs);
    }

    friend std::ostream& operator<<(std::ostream& os, InputCombination& ic) {
        os << "{ ";
        if (ic.fIsSimpleInput) {
            os << ic.fSimpleInput << " }";
        } else {
            for (auto input : ic.fInputVector) { os << input << ", "; }
            os << " }";
        }
        return os;
    }

    // @todo do it for keyboard
    bool isActive(ControllerInput& cInput) {
        if (fIsSimpleInput) {
            auto key = fSimpleInput.controllerKey();
            return cInput.getKey(key);
        }
        for (auto& input : fInputVector) {
            auto key = input.controllerKey();
            if (key != key::ControllerKey::Unknown) {
                if (!cInput.getKey(key)) { return false; }
            } else {
                return false;
            }
        }
        return true;
    }
};

class InputManager {
  private:
    std::map<std::string_view, InputCombination> fInputMap;

    ControllerInput& fControllerInput;

  public:
    InputManager(ControllerInput& controllerInput) : fControllerInput(controllerInput) {}

    void addMapping(std::string_view key, InputCombination inputs) {
        fInputMap.emplace(key, inputs);
    }

    bool isActive(std::string_view key) {
        auto it = fInputMap.find(key);
        if (it == fInputMap.end()) return false;
        auto ret = it->second.isActive(fControllerInput);
        return ret;
    }

    void readInput() { fControllerInput.readInput(); }
};

#undef ControllerKeys
#undef KeyboardKeys

}   // namespace input
}   // namespace window
