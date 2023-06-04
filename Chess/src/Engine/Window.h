#pragma once

#include "GLFW/glfw3.h"

#include "Events/Events.h"
#include "Events/WindowEvents.h"
#include "Events/MouseEvents.h"

#include <iostream>

namespace Engine {


    class Window {
    public:
        Window(unsigned int width, unsigned int height, const char *title,
               std::function<void(Event &)> func);

        ~Window();

        void SetWindowCallbacks();

        void Update();

        GLFWwindow *GetWindow();

        bool OnEvent_WindowClosed(WindowClosedEvent &e);

        bool OnEvent_WindowResize(WindowResizedEvent &e);

        inline bool GetShouldCloseWindow() const { return m_ShouldCloseWindow; }

        std::function<void(Event &)> m_OnEventFunc;

    private:
        GLFWwindow *m_Window;

        unsigned int m_WindowWidth, m_WindowHeight;
        const char *m_WindowTitle;

        bool m_ShouldCloseWindow;
    };


}

