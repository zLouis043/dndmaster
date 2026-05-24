#pragma once
#include "../events/EventDispatcher.h"
#include "../events/Events.h"
#include "InputMapper.h"
#include <SDL3/SDL.h>

class InputSystem {
public:
    static void processSDLBytEvent(const SDL_Event& event, EventDispatcher& dispatcher) {
    switch (event.type) {
        case SDL_EVENT_QUIT:
            dispatcher.dispatch(QuitEvent());
            break;

        case SDL_EVENT_MOUSE_MOTION:
            dispatcher.dispatch(MouseMoveEvent(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel));
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            dispatcher.dispatch(MouseButtonEvent(
                InputMapper::SDLMouseButtonToInternal(event.button.button),
                event.type == SDL_EVENT_MOUSE_BUTTON_DOWN,
                event.button.x, event.button.y
            ));
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            dispatcher.dispatch(MouseWheelEvent(event.wheel.y));
            break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            KeyModifiers mods(
                (event.key.mod & SDL_KMOD_SHIFT) != 0,
                (event.key.mod & SDL_KMOD_CTRL) != 0,
                (event.key.mod & SDL_KMOD_ALT) != 0,
                (event.key.mod & SDL_KMOD_GUI) != 0
            );
            dispatcher.dispatch(KeyEvent(
                InputMapper::SDLToInternal(event.key.key),
                mods,
                event.type == SDL_EVENT_KEY_DOWN
            ));
            break;
        }

        case SDL_EVENT_TEXT_INPUT:
            dispatcher.dispatch(TextInputEvent(event.text.text));
            break;

        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            dispatcher.dispatch(WindowResizeEvent(event.window.data1, event.window.data2));
            break;
    }
}
};