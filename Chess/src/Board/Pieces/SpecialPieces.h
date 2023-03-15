
#pragma once

#include "Piece.h"

// Special Piece = any piece that is not a sliding piece

class King : public Piece
{
public:
	King(Color color, Position pos, float squareSize, Board* board, bool isVirgin=true);
	
	const std::vector<Position>& CalculateLegalMoves() override;
	const bool CheckCastling(int direction);

	bool Move(Position pos, bool overrideLegality=false) override;

	void SetCheck(bool isInCheck);
	bool IsInCheck() const;

private:
	bool m_InCheck;
};

class Pawn : public Piece
{
public:
	Pawn(Color color, Position pos, float squareSize, Board* board);
	~Pawn() override;
	
	const std::vector<Position>& CalculateLegalMoves() override;
	bool Move(Position pos, bool overrideLegality = false) override;

	void CancelEnPassantOffer(bool deleteOriginal=false);
	inline Position GetEnPassantPos() const { return m_EnPassantPos; }

private:
	Position m_EnPassantPos;
	bool m_FirstMove;
};

// Fake Piece
class EnPassantPiece : public Piece // TODO: Finish this
{
public:
	EnPassantPiece(Position pos, Piece* originalPawn);

private:
	Piece* m_OriginalPawn;
};

class Knight : public Piece
{
public:
	Knight(Color color, Position pos, float squareSize, Board* board);
	
	const std::vector<Position>& CalculateLegalMoves() override;
};

///////// Legal Move Sprite /////////////////

// Not actually a piece but still important
class LegalMoveSprite
{
public:
	LegalMoveSprite(float squareSize, float spriteSize, Position pos);
	~LegalMoveSprite();

	void SetPosition(Position pos);

	void Render() const;

private:
	Engine::RendererObject m_Obj;

	float m_SquareSize;
};