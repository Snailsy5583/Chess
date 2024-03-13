#pragma once

#include "Piece.h"

// a piece that can move in specified directions till collision with existing pieces/capturing opponent pieces
class SlidingPiece : public Piece {
public:
	void CalculateLegalMoves(bool findPinnedPiece);

	void CalculateLegalMoves() override;

private:
	bool CheckDirectionIsViable(Position latest, Position movePattern);

	bool FindPinnedPiece(Position curPos, Position movePattern);

	void PinPiece(Piece *p, Position dir);

	void UnPinPiece(Piece *p);

protected:
	SlidingPiece(Color color, Position pos, float squareSize, char *pieceName,
	             Board *board);

	// Only the sliding pieces can pin other pieces
	// Special pieces can't because of the way they move
	Position m_PinnedPiecePos;
};

class Bishop : public SlidingPiece {
public:
	Bishop(Color color, Position pos, float squareSize, Board *board);
};

class Rook
		: public SlidingPiece { // This is a bit special because it can castle
public:
	Rook(Color color, Position pos, float squareSize, Board *board);
};

class Queen : public SlidingPiece {
public:
	Queen(Color color, Position pos, float squareSize, Board *board);
};