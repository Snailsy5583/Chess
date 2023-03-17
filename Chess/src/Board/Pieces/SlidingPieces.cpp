#include "SlidingPieces.h"

#include "Board/Board.h"

SlidingPiece::SlidingPiece(Color color, Position pos, float squareSize, char* pieceName, Board* board)
	: Piece(color, pos, squareSize, pieceName, board)
{}

void SlidingPiece::CalculateLegalMoves()
{
	m_LegalMoves.clear();
	m_ControlledSquares.clear();

	for (const Position& movePattern : m_MovePatterns)
	{
		Position latest = m_Position;

		if (!m_OwnerBoard->IsValidPosition(latest+movePattern))
			continue;

		while (		m_OwnerBoard->IsValidPosition(latest + movePattern)
				&& (!m_OwnerBoard->IsSquareOccupied(latest + movePattern)
				|| m_OwnerBoard->GetPiece(latest+movePattern)->GetPieceName() == "en_passant"))
		{
			m_LegalMoves.push_back(latest + movePattern);
			m_ControlledSquares.push_back(latest+movePattern);
			latest += movePattern;
		}

		if (!m_OwnerBoard->IsValidPosition(latest + movePattern))
			continue;

		m_ControlledSquares.push_back(latest+movePattern);

		if (m_OwnerBoard->IsPieceCapturable(latest + movePattern, m_Color) && 
			m_OwnerBoard->GetPiece(latest+movePattern)->GetPieceName() != "en_passant")
		{
			m_LegalMoves.push_back(latest + movePattern);
			if (m_OwnerBoard->GetPiece(latest + movePattern)->GetPieceName() == "king")
			{
				latest += movePattern;
				while (m_OwnerBoard->IsValidPosition(latest + movePattern))
				{
					m_ControlledSquares.push_back(latest+movePattern);
					latest += movePattern;
				}
			}
		}
	}
}

/////////////////////////////// Instances ////////////////////////////////////

Bishop::Bishop(Color color, Position pos, float squareSize, Board* board)
	: SlidingPiece(color, pos, squareSize, "bishop", board)
{
	m_MovePatterns.push_back({ -1, -1 });
	m_MovePatterns.push_back({ -1,  1 });
	m_MovePatterns.push_back({  1, -1 });
	m_MovePatterns.push_back({  1,  1 });
}

Rook::Rook(Color color, Position pos, float squareSize, Board* board)
	: SlidingPiece(color, pos, squareSize, "rook", board)
{
	m_MovePatterns.push_back({  0, -1 });
	m_MovePatterns.push_back({  0,  1 });
	m_MovePatterns.push_back({ -1,  0 });
	m_MovePatterns.push_back({  1,  0 });
}

Queen::Queen(Color color, Position pos, float squareSize, Board* board)
	: SlidingPiece(color, pos, squareSize, "queen", board)
{
	m_MovePatterns.push_back({ -1, -1 });
	m_MovePatterns.push_back({ -1,  1 });
	m_MovePatterns.push_back({ 1, -1 });
	m_MovePatterns.push_back({ 1,  1 });

	m_MovePatterns.push_back({ 0, -1 });
	m_MovePatterns.push_back({ 0,  1 });
	m_MovePatterns.push_back({ -1,  0 });
	m_MovePatterns.push_back({ 1,  0 });
}