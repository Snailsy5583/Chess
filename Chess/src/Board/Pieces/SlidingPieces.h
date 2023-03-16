#pragma once

#include "Piece.h"

// a piece that can move in specified directions till collision with existing pieces/capturing opponent pieces
class SlidingPiece : public Piece
{
public:
	virtual void CalculateLegalMoves() override;

protected:
	SlidingPiece(Color color, Position pos, float squareSize, char* pieceName, Board* board);
};

class Bishop : public SlidingPiece
{
public:
	Bishop(Color color, Position pos, float squareSize, Board* board);
};

class Rook : public SlidingPiece // This is a bit special because it can castle
{
public:
	Rook(Color color, Position pos, float squareSize, Board* board);
};

class Queen : public SlidingPiece
{
public:
	Queen(Color color, Position pos, float squareSize, Board* board);
};