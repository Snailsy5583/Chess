#pragma once

#include <chrono>
#include <memory>

#include "Board/Board.h"
#include "Engine/Events/Events.h"
#include "Engine/Layer.h"
#include "Engine/Window.h"


class Application
{
public:
	Application(unsigned int width, unsigned int height, const char *title);

	~Application();

	void Run();

	void OnEvent(Engine::Event &e);

	static void AddLayer(Engine::Layer *layer);

	static Engine::LayerStack *GetLayerStack();

private:
	static Application *m_App;

	std::unique_ptr<Engine::Window> m_MainWindow;

	Engine::LayerStack m_LayerStack;

	std::unique_ptr<Board> m_ChessBoard;

	std::chrono::time_point<std::chrono::steady_clock> m_LastFrame;
	float m_DeltaTime;
};