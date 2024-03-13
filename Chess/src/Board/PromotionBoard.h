//
// Created by r6awe on 2/29/2024.
//
#pragma once

//#include "Board.h"

#include "Engine/Layer.h"
#include "Board/Pieces/Piece.h"
#include "Engine/Events/MouseEvents.h"

class PromotionBoard;

class PromotionBoardLayer : public Engine::Layer {
public:
	explicit PromotionBoardLayer(PromotionBoard *boardPtr);

	void OnAttach() override;

	void OnDetach() override;

	bool OnEvent(Engine::Event &e) override;

private:
	PromotionBoard *m_BoardPtr;
};

class PromotionBoard {
public:
	PromotionBoard(Position position, Board *board, Color color,
	               const char *vertShaderPath, const char *fragShaderPath);

	~PromotionBoard();

	void RenderBoard();

	void SetPiece(Position pos, std::unique_ptr<Piece> piece);

	class Square *GetSquare(Position pos, bool ignore0 = false);

public:
	bool HandleMouseReleased(Engine::MouseButtonReleasedEvent &e);

public:
	inline PromotionBoardLayer *GetBoardLayer() { return &m_Layer; }

private:
	std::vector<Square> m_PromotionBoard;
	Board *m_Board;

	Position m_Origin;
	Color m_Color;

	PromotionBoardLayer m_Layer;

};