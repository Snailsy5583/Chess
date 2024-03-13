#pragma once

#include <string>
#include <vector>
#include <set>
#include <memory>

#include "Engine/Layer.h"
#include "Engine/Renderer.h"
#include "Pieces/Piece.h"
#include "Engine/Events/MouseEvents.h"

#include "PromotionBoard.h"

struct Square {
	Position pos;
	Engine::RendererObject obj;
	std::unique_ptr<Piece> piece;
};

class Board;

class BoardLayer : public Engine::Layer {
public:
	explicit BoardLayer(Board *boardPtr);

	void OnAttach() override;

	void OnDetach() override;

	bool OnEvent(Engine::Event &e) override;

private:
	Board *m_BoardPtr;
};

class Board {
public:
	Board(const char *vertShaderPath, const char *fragShaderPath);

	~Board() = default;

	void ReadFen(std::string fen);

	void GeneratePieces(std::string fen, int i, Position &currentPos);

	void RenderBoard();

	int CalculateAllLegalMoves();

	bool MakeMove(Piece *piece, Position from, Position to,
	              bool overrideLegality = false);

public:
	bool HandleMouseDown(Engine::MouseButtonPressedEvent &e);

	bool HandleMouseReleased(Engine::MouseButtonReleasedEvent &e);

	bool HandleMouseMoved(Engine::MouseMovedEvent &e);

public:
	bool IsSquareOccupied(Position pos) const;

	bool IsPieceCapturable(Position pos, Color color) const;

	bool IsInEnemyTerritory(Position pos, Color color);

	Piece *GetPiece(Position pos) const;

	// Careful -- uses std::move()
	std::unique_ptr<Piece> GetFullPiecePtr(Position pos);

	void SetPiece(Position pos, std::unique_ptr<Piece> piece);

	void DeletePiece(Position pos);

	std::unique_ptr<PromotionBoard> GetPromotionBoard();

	void SetPromotionBoard(std::unique_ptr<PromotionBoard> board = nullptr);

public:
	inline BoardLayer *GetBoardLayer() { return &m_Layer; }

	inline Color GetTurn() { return m_Turn; }

private:
	BoardLayer m_Layer;

	Engine::RendererObject m_ShadowObj;

	std::unique_ptr<PromotionBoard> m_PromotionBoard = nullptr;

	float m_SquareSize;
	Square m_Board[64];

	Position m_ActivatedSquare;
	Color m_Turn;

	Position m_WhiteKingPos, m_BlackKingPos;

	std::set<Position> m_WhiteControlledSquares, m_BlackControlledSquares;

	friend class PromotionBoard;
};