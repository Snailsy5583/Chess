
#pragma once

#include "Piece.h"
#include "Board/PromotionBoard.h"

// Special Piece = any piece that is not a sliding piece

class King : public Piece {
public:
	King(Color color, Position pos, float squareSize, Board *board,
	     bool isVirgin = true);

	void CalculateLegalMoves() override;

	bool CheckCastling(int direction);

	// returns the piece checking the king
	// returns null if there are multiple pieces checking the king
	bool IsInCheck(Piece *&piece);

	bool Move(Position pos, bool overrideLegality) override;

	void SetCastling(bool K, bool Q);

private:
	std::vector<Piece *> m_Pins;

	bool m_CanCastleK, m_CanCastleQ;
};

class Knight : public Piece {
public:
	Knight(Color color, Position pos, float squareSize, Board *board);

	void CalculateLegalMoves() override;
};

class EnPassantPiece;

class Pawn : public Piece {
public:
	Pawn(Color color, Position pos, float squareSize, Board *board);

	void CalculateLegalMoves() override;

	bool Move(Position pos, bool overrideLegality) override;

	Piece *Promote(std::unique_ptr<Piece> piece);

	void Render() override;

};

/////////////// Fake Pieces /////////////////

// Piece generated when a pawn is En Passant-able
class EnPassantPiece : public Piece {
public:
	EnPassantPiece(Position pos, Pawn *originalPawn, Board *board);

	void CancelEnPassantOffer(bool deletePawn = false);

	void DeleteOwningPawn();

	// This doesn't have legal move, but it still
	// needs to delete itself after the first move
	void CalculateLegalMoves() override;

	Position
	GetPosition() const override { return m_OriginalPawn->GetPosition(); }

private:
	Pawn *m_OriginalPawn;

	bool m_FirstMove;
};


// Piece generated to show legal moves
class LegalMoveSprite {
public:
	LegalMoveSprite(float squareSize, float spriteSize, Position pos);

	~LegalMoveSprite();

	void SetPosition(Position pos);

	void Render() const;

private:
	Engine::RendererObject m_Obj;

	float m_SquareSize;
};