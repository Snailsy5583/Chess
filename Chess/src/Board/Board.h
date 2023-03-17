#pragma once

#include <string>
#include <vector>
#include <set>
#include <memory>

#include "Engine/Layer.h"
#include "Engine/Renderer.h"
#include "Pieces/Piece.h"
#include "Engine/Events/MouseEvents.h"

struct Square
{
	Position pos;
	Engine::RendererObject obj;
	std::unique_ptr<Piece> piece;
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

	void GenerateBoard(std::string fen);
	
	void RenderBoard();

	int CalculateAllLegalMoves();
	bool MakeMove(Piece* piece, Position from, Position to, bool overrideLegality = false);
	
public:
	bool HandleMouseDown(Engine::MouseButtonPressedEvent& e);
	bool HandleMouseReleased(Engine::MouseButtonReleasedEvent& e);
	bool HandleMouseMoved(Engine::MouseMovedEvent& e);

public:
	static bool IsValidPosition(Position pos);

	bool IsSquareOccupied(Position pos) const;

	bool IsPieceCapturable(Position pos, Color color);

	bool IsInEnemyTerritory(Position pos, Color color);

	Piece* GetPiece(Position pos) const;

	// Careful -- uses std::move()
	std::unique_ptr<Piece> GetFullPiecePtr(Position pos);
	void SetPiece(Position pos, std::unique_ptr<Piece> piece);

	void DeletePiece(Position pos);

public:
	inline BoardLayer* GetBoardLayer()
	{ return &m_Layer; }
	inline Color GetTurn()
	{ return m_Turn; }

private:
	BoardLayer m_Layer;

	float m_SquareSize;
	Square m_Board[64];

	Position m_ActivatedSquare;
	Color m_Turn;

	std::set<Position> m_WhiteControlledSquares;
	std::set<Position> m_BlackControlledSquares;
};