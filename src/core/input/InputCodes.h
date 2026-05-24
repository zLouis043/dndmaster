#pragma once

enum class KeyCode {
    Unknown = 0,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    Escape, Space, Return, Backspace, Tab, Delete, Insert, Home, End, PageUp, PageDown,
    Left, Right, Up, Down,
    LShift, RShift, LCtrl, RCtrl, LAlt, RAlt, LSystem, RSystem,
    Capslock, ScrollLock, NumLock, PrintScreen, Pause
};

enum class MouseButton {
    None = 0,
    Left,
    Right,
    Middle,
    X1,
    X2
};

struct KeyModifiers {
    bool shift = false;
    bool ctrl = false;
    bool alt = false;
    bool system = false;

    KeyModifiers() = default;
    KeyModifiers(bool s, bool c, bool a, bool sys) 
        : shift(s), ctrl(c), alt(a), system(sys) {}
};

namespace Mod {
    const KeyModifiers None  {false, false, false, false};
    const KeyModifiers Ctrl  {false, true,  false, false};
    const KeyModifiers Shift {true,  false, false, false};
    const KeyModifiers Alt   {false, false, true,  false};
    const KeyModifiers CtrlShift {true, true, false, false};
}