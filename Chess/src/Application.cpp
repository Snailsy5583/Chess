#include "Application.h"
#include "Engine/Events/MouseEvents.h"
#include "Engine/Events/WindowEvents.h"

#include <fstream>

#define BIND_EVENT_FUNC(x, obj) std::bind(&x, obj, std::placeholders::_1)

Application::Application(unsigned int width, unsigned int height,
                         const char *title) {
    m_DeltaTime = 0;
    if (!glfwInit())
        std::cout << "GLFW INIT FAILED\n";

    m_MainWindow = new Engine::Window(
        width, height, title,BIND_EVENT_FUNC(Application::OnEvent, this));
    //m_MainWindow = std::make_unique<Engine::Window>(
    // width, height, title, BIND_EVENT_FUNC(Application::OnEvent, this));

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        std::cout << "Failed to initialize GLAD\n";

    m_ChessBoard = std::make_unique<Board>("Assets/Shaders/Board.vert",
                                           "Assets/Shaders/Board.frag");

    m_LayerStack.Push(m_ChessBoard->GetBoardLayer());

    m_ChessBoard->ReadFen(
        "rnbqk2r/pppp1ppp/8/2b1p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1");
}

Application::~Application() {
    glfwTerminate();
}

void Application::Run() {
    m_LastFrame = std::chrono::steady_clock::now();

    while (!m_MainWindow->GetShouldCloseWindow()) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_ChessBoard->RenderBoard();

        m_MainWindow->Update();

        m_DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - m_LastFrame).count() / 1000000.f;

        m_LastFrame = std::chrono::steady_clock::now();
    }
}

void Application::OnEvent(Engine::Event &e) {
    Engine::EventDispatcher dispatcher(e);

    dispatcher.Dispatch<Engine::WindowClosedEvent>(
        BIND_EVENT_FUNC(Engine::Window::OnEvent_WindowClosed, m_MainWindow));
    dispatcher.Dispatch<Engine::WindowResizedEvent>(
        BIND_EVENT_FUNC(Engine::Window::OnEvent_WindowResize, m_MainWindow));

    m_LayerStack.OnEvent(e);
}
