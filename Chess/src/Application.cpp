#include "Application.h"
#include "Engine/Events/MouseEvents.h"
#include "Engine/Events/WindowEvents.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <fstream>





#define BIND_EVENT_FUNC(x, obj) std::bind(&x, obj, std::placeholders::_1)

Application::Application(unsigned int width, unsigned int height, const char* title)
{
	m_DeltaTime = 0;
	if (!glfwInit())
		std::cout << "GLFW INIT FAILED\n";

	m_MainWindow = new Engine::Window(width, height, title, BIND_EVENT_FUNC(Application::OnEvent, this));

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD\n";

	m_ChessBoard = new Board
	(
		"Assets/Shaders/Board.vert", 
		"Assets/Shaders/Board.frag"
	);

	m_LayerStack.Push(m_ChessBoard->GetBoardLayer());

	m_ChessBoard->GenerateBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

Application::~Application()
{
	delete m_MainWindow;
	delete m_ChessBoard;

	glfwTerminate();
}

void Application::Run()
{
	m_LastFrame = std::chrono::steady_clock::now();

	while (!m_MainWindow->GetShouldCloseWindow())
	{
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_ChessBoard->RenderBoard();

		m_MainWindow->Update();

		m_DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_LastFrame).count()/1000000.f;

		m_LastFrame = std::chrono::steady_clock::now();
	}
}

void Application::OnEvent(Engine::Event& e)
{
	Engine::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Engine::WindowClosedEvent>(BIND_EVENT_FUNC(Engine::Window::OnEvent_WindowClosed, m_MainWindow));
	dispatcher.Dispatch<Engine::WindowResizedEvent>(BIND_EVENT_FUNC(Engine::Window::OnEvent_WindowResize, m_MainWindow));

	m_LayerStack.OnEvent(e);
}
