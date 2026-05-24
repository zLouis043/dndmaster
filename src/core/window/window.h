#pragma once
#include <SDL3/SDL.h>
#include <string>

class Window {
public:
    Window(const std::string& title, int w, int h);
    ~Window();

    bool init();
    void destroy();
    void swap();
    void show();
    SDL_Window* getNativeWindow() const { return m_window; }
    SDL_GLContext getGLContext() const { return m_glContext; }
    void getDrawableSize(int* w, int* h) { SDL_GetWindowSizeInPixels(m_window, w, h); }

private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext = nullptr;
    std::string m_title;
    int m_width, m_height;
};