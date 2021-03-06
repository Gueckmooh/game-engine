#pragma once

#include <iostream>

#include <macros/macros.hpp>

namespace window {
namespace input {

#define _MKeyboardKeys                                                                   \
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
        Space,        /* Space key */                                                    \
        LeftAlt,      /* LeftAlt key */                                                  \
        RightAlt,     /* RightAlt key */                                                 \
        LeftControl,  /* LeftControl key */                                              \
        RightControl, /* RightControl key */                                             \
        LeftShift,    /* LeftShift key */                                                \
        RightShift,   /* RightShift key */                                               \
        Delete,       /* Delete key */                                                   \
        Backspace,    /* Backspace key */

#define _MControllerKeys                                                                 \
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

#define _MMouseKeys                                                                      \
    Unknown,    /* Unknown */                                                            \
        Left,   /* Mouse left button (main) */                                           \
        Right,  /* Mouse right button (secondary) */                                     \
        Middle, /* Mouse middle button (wheel) */

#define _MWheelKeys                                                                      \
    Unknown,        /* Unknown */                                                        \
        Vertical,   /* Vertical mouse wheel (main) */                                    \
        Horizontal, /* Horizontal mouse wheel (secondary) */

$enum_class(key, WheelKey, _MWheelKeys);
$enum_class(key, MouseKey, _MMouseKeys);
$enum_class(key, KeyboardKey, _MKeyboardKeys);
$enum_class(key, ControllerKey, _MControllerKeys);

}   // namespace input
}   // namespace window
