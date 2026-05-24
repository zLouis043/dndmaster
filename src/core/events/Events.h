#pragma once
#include "../input/InputCodes.h"
#include "EventDispatcher.h"
#include <string>

struct KeyEvent : public IEvent {
    KeyCode key;
    KeyModifiers mods;
    bool pressed;

    KeyEvent(KeyCode k, KeyModifiers m, bool p) 
        : key(k), mods(m), pressed(p) {}
};

struct MouseButtonEvent : public IEvent {
    MouseButton button;
    bool pressed;
    float x, y;

    MouseButtonEvent(MouseButton b, bool p, float mx, float my) 
        : button(b), pressed(p), x(mx), y(my) {}
};

struct MouseMoveEvent : public IEvent {
    float x, y;
    float dx, dy;

    MouseMoveEvent(float mx, float my, float mdx, float mdy) 
        : x(mx), y(my), dx(mdx), dy(mdy) {}
};

struct MouseWheelEvent : public IEvent {
    float delta;

    MouseWheelEvent(float d) : delta(d) {}
};

struct TextInputEvent : public IEvent {
    std::string text;

    TextInputEvent(const std::string& t) : text(t) {}
};

struct WindowResizeEvent : public IEvent {
    int width, height;

    WindowResizeEvent(int w, int h) : width(w), height(h) {}
};

struct QuitEvent : public IEvent {};