#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Application.h"

int main()
{
	unsigned int windowWidth=850, WindowHeight=850;
	std::string windowTitle = "Chess";

	Application app(windowWidth, WindowHeight, windowTitle.c_str());

	app.Run();
}