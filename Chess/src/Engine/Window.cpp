#include "Window.h"

#include <utility>

namespace Engine
{


	Window::Window(
		unsigned int width, unsigned int height, const char *title,
		std::function<void(Event &)> func
	) {
		m_WindowWidth = width;
		m_WindowHeight = height;
		m_WindowTitle = title;
		m_OnEventFunc = std::move(func);

		m_ShouldCloseWindow = false;

		m_Window = glfwCreateWindow(
			(int) m_WindowWidth, (int) m_WindowHeight, m_WindowTitle, nullptr,
			nullptr
		);
		glfwMakeContextCurrent(m_Window);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_VERSION_MINOR, 3);

		//		glfwSwapInterval(1);

		if (!m_Window) {
			std::cout << "FAILED TO CREATE WINDOW\n";
		}

		SetWindowCallbacks();
	}

	Window::~Window() { glfwDestroyWindow(m_Window); }

	void Window::SetWindowCallbacks() {
		glfwSetWindowUserPointer(m_Window, (void *) this);
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
			Window &win = *(Window *) glfwGetWindowUserPointer(window);
			WindowClosedEvent e;
			win.m_OnEventFunc(e);
		});

		glfwSetWindowSizeCallback(
			m_Window,
			[](GLFWwindow *window, int width, int height) {
				Window &win = *(Window *) glfwGetWindowUserPointer(window);
				WindowResizedEvent e(width, height);
				win.m_OnEventFunc(e);
			}
		);

		glfwSetMouseButtonCallback(
			m_Window,
			[](GLFWwindow *window, int button, int action, int mods) {
				Window &win = *(Window *) glfwGetWindowUserPointer(window);

				double mouseX, mouseY;
				glfwGetCursorPos(window, &mouseX, &mouseY);

				int width, height;
				glfwGetWindowSize(window, &width, &height);

				if (action == GLFW_PRESS) {
					MouseButtonPressedEvent e(
						button, (float) mouseX * 2 / (float) width - 1,
						-(float) mouseY * 2 / (float) height + 1
					);
					win.m_OnEventFunc(e);
				} else if (action == GLFW_RELEASE) {
					MouseButtonReleasedEvent e(
						button, (float) mouseX * 2 / (float) width - 1,
						-(float) mouseY * 2 / (float) height + 1
					);
					win.m_OnEventFunc(e);
				}
			}
		);

		glfwSetCursorPosCallback(
			m_Window,
			[](GLFWwindow *window, double mouseX, double mouseY) {
				Window &win = *(Window *) glfwGetWindowUserPointer(window);

				int width, height;
				glfwGetWindowSize(window, &width, &height);

				MouseMovedEvent e(
					(float) mouseX * 2 / (float) width - 1,
					-(float) mouseY * 2 / (float) height + 1
				);
				win.m_OnEventFunc(e);
			}
		);

		glfwSetKeyCallback(
			m_Window,
			[](GLFWwindow *window, int key, int scancode, int action,
		       int mods) {
				Window &win = *(Window *) glfwGetWindowUserPointer(window);

				switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent e(key);

					win.m_OnEventFunc(e);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent e(key);

					win.m_OnEventFunc(e);
					break;
				}
				default: break;
				}
			}
		);
	}

	void Window::Update() {
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	GLFWwindow *Window::GetWindow() { return m_Window; }

	bool Window::OnEvent_WindowClosed(WindowClosedEvent &e) {
		m_ShouldCloseWindow = true;
		return true;
	}

	bool Window::OnEvent_WindowResize(WindowResizedEvent &e) {
		m_WindowWidth = e.GetWidth();
		m_WindowHeight = e.GetHeight();
		return true;
	}


} // namespace Engine