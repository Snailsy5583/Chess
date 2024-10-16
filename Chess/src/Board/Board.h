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
#include "Board/Pieces/SpecialPieces.h"

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
	friend class PromotionBoard;

public:
	Board(const char *vertShaderPath, const char *fragShaderPath);

	~Board() = default;

	void ReadFen(std::string fen);

	void GeneratePieces(std::string fen, int i, Position &currentPos);

	void RenderBoard();

	int CalculateAllLegalMoves();

	void RecalculateCheckLegalMoves(King* king, Piece* checker);

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

	// Gives away ownership
	std::unique_ptr<Piece> GetFullPiecePtr(Position pos);

	void SetPiece(Position pos, std::unique_ptr<Piece> piece);

	void DeletePiece(Position pos);

public:
	inline BoardLayer *GetBoardLayer() { return &m_Layer; }

	inline Color GetTurn() { return m_Turn; }

private:
	BoardLayer m_Layer;

	Engine::RendererObject m_ShadowObj;

	float m_SquareSize;
	Square m_Board[64];

	Position m_ActivatedSquare;
	Color m_Turn;

	std::set<Position> m_ControlledSquares[2]{};

public:
	std::unique_ptr<PromotionBoard> p_PromotionBoard = nullptr;

	Position p_PinnedPiecePos[2]{};

	Position p_KingPos[2]{};
};