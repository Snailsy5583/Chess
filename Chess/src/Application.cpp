#include "Application.h"
#include "Engine/Events/WindowEvents.h"

#include <fstream>

#define BIND_EVENT_FUNC(func, obj) std::bind(&func, obj, std::placeholders::_1)

Application* Application::m_App = nullptr;

Application::Application(unsigned int width, unsigned int height, const char *title)
	: m_DeltaTime(0) {
    m_App = this;

    if (!glfwInit())
        std::cout << "GLFW INIT FAILED\n";

//    m_MainWindow = new Engine::Window(
//        width, height, title, BIND_EVENT_FUNC(Application::OnEvent, this));
    m_MainWindow = std::make_unique<Engine::Window>(
     width, height, title, BIND_EVENT_FUNC(Application::OnEvent, this));

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        std::cout << "Failed to initialize GLAD\n";

    m_ChessBoard = std::make_unique<Board>("Assets/Shaders/Board.vert",
                                           "Assets/Shaders/Board.frag");

    AddLayer(m_ChessBoard->GetBoardLayer());

    std::string starting = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string castling_test = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
    std::string promotion_test = "8/PPPPPPPP/8/K6k/8/8/pppppppp/8 w KQkq - 0 1";

    m_ChessBoard->ReadFen(starting);
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

        m_DeltaTime = (float)std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - m_LastFrame).count() / 1000000.f;

        m_LastFrame = std::chrono::steady_clock::now();
    }
}

void Application::OnEvent(Engine::Event &e) {
    Engine::EventDispatcher dispatcher(e);

    dispatcher.Dispatch<Engine::WindowClosedEvent>(
        BIND_EVENT_FUNC(Engine::Window::OnEvent_WindowClosed, m_MainWindow.get()));
    dispatcher.Dispatch<Engine::WindowResizedEvent>(
        BIND_EVENT_FUNC(Engine::Window::OnEvent_WindowResize, m_MainWindow.get()));

    m_LayerStack.OnEvent(e);
}

void Application::AddLayer(Engine::Layer* layer) {
    if (m_App)
        m_App->m_LayerStack.Push(layer);
}

Engine::LayerStack* Application::GetLayerStack() {
    if (m_App)
        return &m_App->m_LayerStack;
    else
        return nullptr;
}
