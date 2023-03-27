
#pragma once

#include "Piece.h"

// Special Piece = any piece that is not a sliding piece

class King : public Piece
{
public:
	King(Color color, Position pos, float squareSize, Board* board, bool isVirgin=true);
	
	// Only Sliding Pieces can pin
	void FindPins();
	void CalculateLegalMoves() override;
	const bool CheckCastling(int direction);

	bool Move(Position pos, bool overrideLegality=false) override;

	void SetCheck(bool isInCheck);
	bool IsInCheck() const;

	void SetCastling(bool K, bool Q);
	inline bool GetCastling(bool side)
	{
		if (side)
			return m_CanCastleK;
		else
			return m_CanCastleQ;
	}

private:
	bool m_InCheck;
	std::vector<Piece*> m_Pins;

	bool m_CanCastleK, m_CanCastleQ;
};

class Knight : public Piece
{
public:
	Knight(Color color, Position pos, float squareSize, Board* board);
	
	void CalculateLegalMoves() override;
};

class EnPassantPiece;

class Pawn : public Piece
{
public:
	Pawn(Color color, Position pos, float squareSize, Board* board);
	~Pawn() override;
	// TODO: Queen
	
	void CalculateLegalMoves() override;
	bool Move(Position pos, bool overrideLegality = false) override;
};

/////////////// Fake Pieces /////////////////

// Piece generated when a pawn is En Passant-able
class EnPassantPiece : public Piece
{
public:
	EnPassantPiece(Position pos, Pawn* originalPawn, Board* board);

	void CancelEnPassantOffer(bool deletePawn = false);
	void DeleteOwningPawn();

	// This doesn't have legal moves but it still needs to delete itself
	// after the first move
	void CalculateLegalMoves() override;

	Position GetPosition() const override { return m_OriginalPawn->GetPosition(); }

private:
	Pawn* m_OriginalPawn;
	
	bool m_FirstMove;
};


// Piece generated to show legal moves
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