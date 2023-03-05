#pragma once

#include <string>
#include <vector>

#include "Engine/Layer.h"
#include "Engine/Renderer.h"
#include "Pieces/Piece.h"
#include "Engine/Events/MouseEvents.h"

struct Square
{
	Position pos;
	Engine::RendererObject obj;
	Piece* piece;
};

class Board;

class BoardLayer : public Engine::Layer
{
public:
	BoardLayer(Board* boardPtr);

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnEvent(Engine::Event& e) override;

private:
	Board* m_BoardPtr;
};

class Board
{
public:
	Board(const char* vertShaderPath, const char* fragShaderPath);
	~Board();

	void CalculateAllLegalMoves();
	bool MakeMove(Piece* piece, Position from, Position to, bool overrideLegality=false);

	void GenerateBoard(std::string fen);

	void RenderBoard();

public:
	bool IsSquareOccupied(Position pos) const;
	bool IsValidPosition(Position pos) const;
	bool IsPieceCapturable(Position pos, Color color);
	Piece* GetPiece(Position pos);

	class King* GetKing();
	
public:
	bool HandleMouseDown(Engine::MouseButtonPressedEvent& e);
	bool HandleMouseReleased(Engine::MouseButtonReleasedEvent& e);
	bool HandleMouseMoved(Engine::MouseMovedEvent& e);

public:
	static int Pos2Index(Position pos);

	inline BoardLayer* GetBoardLayer()
	{ return &m_Layer; }
	inline Color GetTurn()
	{ return m_Turn; }

private:
	BoardLayer m_Layer;

	float m_SquareSize;
	Square m_Board[64];

	class King* m_King;

	Position m_ActivatedSquare;
	Color m_Turn;

	std::vector<Position> m_WhiteControlledSquares;
	std::vector<Position> m_BlackControlledSquares;
};