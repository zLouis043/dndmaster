#pragma once
#include <SDL3/SDL.h>
#include <RmlUi/Core/Input.h>
#include "InputCodes.h"

class InputMapper {
public:
    static KeyCode SDLToInternal(SDL_Keycode key) {
        switch (key) {
            case SDLK_A: return KeyCode::A; case SDLK_B: return KeyCode::B; case SDLK_C: return KeyCode::C;
            case SDLK_D: return KeyCode::D; case SDLK_E: return KeyCode::E; case SDLK_F: return KeyCode::F;
            case SDLK_G: return KeyCode::G; case SDLK_H: return KeyCode::H; case SDLK_I: return KeyCode::I;
            case SDLK_J: return KeyCode::J; case SDLK_K: return KeyCode::K; case SDLK_L: return KeyCode::L;
            case SDLK_M: return KeyCode::M; case SDLK_N: return KeyCode::N; case SDLK_O: return KeyCode::O;
            case SDLK_P: return KeyCode::P; case SDLK_Q: return KeyCode::Q; case SDLK_R: return KeyCode::R;
            case SDLK_S: return KeyCode::S; case SDLK_T: return KeyCode::T; case SDLK_U: return KeyCode::U;
            case SDLK_V: return KeyCode::V; case SDLK_W: return KeyCode::W; case SDLK_X: return KeyCode::X;
            case SDLK_Y: return KeyCode::Y; case SDLK_Z: return KeyCode::Z;
            case SDLK_0: return KeyCode::Num0; case SDLK_1: return KeyCode::Num1; case SDLK_2: return KeyCode::Num2;
            case SDLK_3: return KeyCode::Num3; case SDLK_4: return KeyCode::Num4; case SDLK_5: return KeyCode::Num5;
            case SDLK_6: return KeyCode::Num6; case SDLK_7: return KeyCode::Num7; case SDLK_8: return KeyCode::Num8;
            case SDLK_9: return KeyCode::Num9;
            case SDLK_ESCAPE: return KeyCode::Escape; case SDLK_RETURN: return KeyCode::Return;
            case SDLK_BACKSPACE: return KeyCode::Backspace; case SDLK_SPACE: return KeyCode::Space;
            case SDLK_TAB: return KeyCode::Tab; case SDLK_DELETE: return KeyCode::Delete;
            case SDLK_HOME: return KeyCode::Home; case SDLK_END: return KeyCode::End;
            case SDLK_PAGEUP: return KeyCode::PageUp; case SDLK_PAGEDOWN: return KeyCode::PageDown;
            case SDLK_LEFT: return KeyCode::Left; case SDLK_RIGHT: return KeyCode::Right;
            case SDLK_UP: return KeyCode::Up; case SDLK_DOWN: return KeyCode::Down;
            case SDLK_LSHIFT: return KeyCode::LShift; case SDLK_RSHIFT: return KeyCode::RShift;
            case SDLK_LCTRL: return KeyCode::LCtrl; case SDLK_RCTRL: return KeyCode::RCtrl;
            case SDLK_LALT: return KeyCode::LAlt; case SDLK_RALT: return KeyCode::RAlt;
            default: return KeyCode::Unknown;
        }
    }

    static MouseButton SDLMouseButtonToInternal(Uint8 button) {
        switch (button) {
            case SDL_BUTTON_LEFT:   return MouseButton::Left;
            case SDL_BUTTON_RIGHT:  return MouseButton::Right;
            case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
            case SDL_BUTTON_X1:     return MouseButton::X1;
            case SDL_BUTTON_X2:     return MouseButton::X2;
            default:                return MouseButton::None;
        }
    }

    static Rml::Input::KeyIdentifier InternalToRml(KeyCode key) {
        switch (key) {
            case KeyCode::A: return Rml::Input::KI_A; case KeyCode::B: return Rml::Input::KI_B;
            case KeyCode::C: return Rml::Input::KI_C; case KeyCode::D: return Rml::Input::KI_D;
            case KeyCode::E: return Rml::Input::KI_E; case KeyCode::F: return Rml::Input::KI_F;
            case KeyCode::G: return Rml::Input::KI_G; case KeyCode::H: return Rml::Input::KI_H;
            case KeyCode::I: return Rml::Input::KI_I; case KeyCode::J: return Rml::Input::KI_J;
            case KeyCode::K: return Rml::Input::KI_K; case KeyCode::L: return Rml::Input::KI_L;
            case KeyCode::M: return Rml::Input::KI_M; case KeyCode::N: return Rml::Input::KI_N;
            case KeyCode::O: return Rml::Input::KI_O; case KeyCode::P: return Rml::Input::KI_P;
            case KeyCode::Q: return Rml::Input::KI_Q; case KeyCode::R: return Rml::Input::KI_R;
            case KeyCode::S: return Rml::Input::KI_S; case KeyCode::T: return Rml::Input::KI_T;
            case KeyCode::U: return Rml::Input::KI_U; case KeyCode::V: return Rml::Input::KI_V;
            case KeyCode::W: return Rml::Input::KI_W; case KeyCode::X: return Rml::Input::KI_X;
            case KeyCode::Y: return Rml::Input::KI_Y; case KeyCode::Z: return Rml::Input::KI_Z;
            case KeyCode::Num0: return Rml::Input::KI_0; case KeyCode::Num1: return Rml::Input::KI_1;
            case KeyCode::Num2: return Rml::Input::KI_2; case KeyCode::Num3: return Rml::Input::KI_3;
            case KeyCode::Num4: return Rml::Input::KI_4; case KeyCode::Num5: return Rml::Input::KI_5;
            case KeyCode::Num6: return Rml::Input::KI_6; case KeyCode::Num7: return Rml::Input::KI_7;
            case KeyCode::Num8: return Rml::Input::KI_8; case KeyCode::Num9: return Rml::Input::KI_9;
            case KeyCode::Escape: return Rml::Input::KI_ESCAPE; case KeyCode::Space: return Rml::Input::KI_SPACE;
            case KeyCode::Return: return Rml::Input::KI_RETURN; case KeyCode::Backspace: return Rml::Input::KI_BACK;
            case KeyCode::Tab: return Rml::Input::KI_TAB; case KeyCode::Delete: return Rml::Input::KI_DELETE;
            case KeyCode::Left: return Rml::Input::KI_LEFT; case KeyCode::Right: return Rml::Input::KI_RIGHT;
            case KeyCode::Up: return Rml::Input::KI_UP; case KeyCode::Down: return Rml::Input::KI_DOWN;
            case KeyCode::LShift: return Rml::Input::KI_LSHIFT; case KeyCode::RShift: return Rml::Input::KI_RSHIFT;
            case KeyCode::LCtrl: return Rml::Input::KI_LCONTROL; case KeyCode::RCtrl: return Rml::Input::KI_RCONTROL;
            case KeyCode::LAlt: return Rml::Input::KI_LMENU; case KeyCode::RAlt: return Rml::Input::KI_RMENU;
            default: return Rml::Input::KI_UNKNOWN;
        }
    }

    static int InternalModsToRml(const KeyModifiers& mods) {
        int rml_mods = 0;
        if (mods.shift) rml_mods |= Rml::Input::KM_SHIFT;
        if (mods.ctrl)  rml_mods |= Rml::Input::KM_CTRL;
        if (mods.alt)   rml_mods |= Rml::Input::KM_ALT;
        if (mods.system) rml_mods |= Rml::Input::KM_META;
        return rml_mods;
    }
};