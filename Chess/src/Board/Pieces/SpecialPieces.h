
#pragma once

#include "Board/Board.h"
#include "Board/PromotionBoard.h"
#include "Piece.h"
#include "SlidingPieces.h"

// Special Piece = any piece that is not a sliding piece

class King : public Piece
{
public:
	King(
		Color color, Position pos, float squareSize, Board *board,
		bool isVirgin = true
	);

	void CalculateLegalMoves() override;

	bool CheckCastling(int direction);

	// returns the piece checking the king
	// returns null if there are multiple pieces checking the king
	bool IsInCheck(Piece *&checker);

	bool DoesMoveBlockCheck(Position move, Piece *checker);

	bool Move(Position to) override;

	void UndoMove(Position from) override;

	void SetCastling(bool K, bool Q);

private:
	std::vector<Piece *> m_Pins;

	bool m_CanCastleK, m_CanCastleQ;
};

class Knight : public Piece
{
public:
	Knight(Color color, Position pos, float squareSize, Board *board);

	void CalculateLegalMoves() override;
};

class Pawn : public Piece
{
public:
	Pawn(
		Color color, Position pos, float squareSize, Board *board,
		bool isVirgin = true
	);

	~Pawn() override = default;

	void CalculateLegalMoves() override;

	bool Move(Position to) override;

	bool CheckIsPromotionMove(Position to);

	template<typename PieceType>
	std::unique_ptr<Piece> Promote() {
		auto pawn = std::move(m_OwnerBoard->GetFullPiecePtr(m_Position));

		// std::unique_ptr<Piece> restricts the PieceType to only subclasses of
		// Piece
		std::unique_ptr<Piece> piece = std::make_unique<PieceType>(
			m_Color, m_Position, m_SquareSize, m_OwnerBoard
		);

		m_OwnerBoard->SetPiece(m_Position, std::move(piece));

		m_OwnerBoard->CalculateAllLegalMoves();

		// return pawn unique_ptr
		return pawn;
	}
};

/////////////// Fake Pieces /////////////////

// Piece generated when a pawn is En Passant-able
class EnPassantPiece : public Piece
{
public:
	EnPassantPiece(Board *board);

	~EnPassantPiece() = default;

	void SetPawn(Pawn *pawn, Position pos, int moveNum = -1);

	void UndoMove(Position from) override;

	std::unique_ptr<Piece> CancelEnPassantOffer(bool deletePawn = false);

	Position *GetPosition() { return &m_Position; }

	// This doesn't have legal move, but it still
	// needs to delete itself after the first move
	void CalculateLegalMoves() override;

	void Render() override {}

public:
	Pawn *p_OwningPawn;

private:
	bool m_MoveWasIgnored = true;

	std::stack<std::tuple<int, Pawn *, Position>> m_MoveCache;
};

// Piece generated to show legal moves
class LegalMoveSprite
{
public:
	LegalMoveSprite(
		float squareSize, float spriteSize, Position pos, bool capture
	);

	~LegalMoveSprite();

	void SetPosition(Position pos) const;

	void Render() const;

private:
	Engine::RendererObject m_Obj;

	float m_SquareSize;
};