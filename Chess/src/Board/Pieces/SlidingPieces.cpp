#include "SlidingPieces.h"

#include "Board/Board.h"

SlidingPiece::SlidingPiece(Color color, Position pos, float squareSize, char* pieceName, Board* board)
	: Piece(color, pos, squareSize, pieceName, board)
{}

const std::vector<Position>& SlidingPiece::CalculateLegalMoves()
{
	m_LegalMoves.clear();

	for (const Position& movePattern : m_MovePatterns)
	{
		Position latest = m_Position;

		if (!m_OwnerBoard->IsValidPosition(latest+movePattern))
			continue;

		while (!m_OwnerBoard->IsSquareOccupied(latest + movePattern) && m_OwnerBoard->IsValidPosition(latest + movePattern))
		{
			m_LegalMoves.push_back(latest + movePattern);
			latest += movePattern;
		}

		if (!m_OwnerBoard->IsValidPosition(latest + movePattern))
			continue;

		if (m_OwnerBoard->IsPieceCapturable(latest + movePattern, m_Color))
		{
			m_LegalMoves.push_back(latest + movePattern);
		}
	}
	return m_LegalMoves;
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