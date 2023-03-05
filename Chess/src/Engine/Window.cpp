#include "Window.h"

namespace Engine
{


	Window::Window(unsigned int width, unsigned int height, const char* title, std::function<void(Event&)> func)
	{
		m_WindowWidth = width;
		m_WindowHeight = height;
		m_WindowTitle = title;
		m_OnEventFunc = func;

		m_ShouldCloseWindow = false;

		m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, m_WindowTitle, NULL, NULL);
		glfwMakeContextCurrent(m_Window);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_VERSION_MINOR, 3);

		glfwSwapInterval(1);

		if (!m_Window)
		{
			std::cout << "FAILED TO CREATE WINDOW\n";
		}

		SetWindowCallbacks();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
	}

	void Window::SetWindowCallbacks()
	{
		glfwSetWindowUserPointer(m_Window, (void*) this);
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
				Window& win = *(Window*) glfwGetWindowUserPointer(window);
				win.m_OnEventFunc(WindowClosedEvent());
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
				Window& win = *(Window*) glfwGetWindowUserPointer(window);
				win.m_OnEventFunc(WindowResizedEvent(width, height));
			});
		
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
				Window& win = *(Window*)glfwGetWindowUserPointer(window);

				double mouseX, mouseY;
				glfwGetCursorPos(window, &mouseX, &mouseY);

				int width, height;
				glfwGetWindowSize(window, &width, &height);

				if (action == GLFW_PRESS)
				{
					win.m_OnEventFunc(MouseButtonPressedEvent(button, mouseX * 2 / width - 1, -mouseY * 2 / height + 1));
				}
				else if (action == GLFW_RELEASE)
				{
					win.m_OnEventFunc(MouseButtonReleasedEvent(button, mouseX * 2 / width - 1, -mouseY * 2 / height + 1));
				}
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double mouseX, double mouseY) {
				Window& win = *(Window*)glfwGetWindowUserPointer(window);

				int width, height;
				glfwGetWindowSize(window, &width, &height);

				win.m_OnEventFunc(MouseMovedEvent(mouseX*2/width-1, -mouseY*2/height+1));
			});
	}

	void Window::Update()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	GLFWwindow* Window::GetWindow()
	{ return m_Window; }

	bool Window::OnEvent_WindowClosed(WindowClosedEvent& e)
	{
		m_ShouldCloseWindow = true;
		return true;
	}

	bool Window::OnEvent_WindowResize(WindowResizedEvent& e)
	{
		m_WindowWidth = e.GetWidth();
		m_WindowHeight = e.GetHeight();
		std::cout << "Event: Window Resized (" << m_WindowWidth << ", " << m_WindowHeight << ")" << std::endl;
		return true;
	}

}