#pragma once

#include <memory>
#include <chrono>

#include "Engine/Window.h"
#include "Engine/Events/Events.h"
#include "Engine/Layer.h"
#include "Board/Board.h"

class Application {
public:
    Application(unsigned int width, unsigned int height, const char *title);

    ~Application();

    void Run();

    void OnEvent(Engine::Event &e);

private:
    Engine::Window *m_MainWindow;

    Engine::LayerStack m_LayerStack;

    std::unique_ptr<Board> m_ChessBoard;

    std::chrono::time_point<std::chrono::steady_clock> m_LastFrame;
    float m_DeltaTime;
};