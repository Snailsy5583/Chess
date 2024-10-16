#include "SlidingPieces.h"

#include "Board/Board.h"

SlidingPiece::SlidingPiece(Color color, Position pos, float squareSize,
                           const char *pieceName,
                           Board *board)
		: Piece(color, pos, squareSize, pieceName, board) {}

void SlidingPiece::CalculateLegalMoves() {
	m_LegalMoves.clear();
	m_ControlledSquares.clear();

	for (const Position &movePattern : m_MovePatterns) {
		// if this piece is pinned
		if ( m_PinnedDirection != Position({0, 0}) &&
		     (m_PinnedDirection != movePattern && m_PinnedDirection != -movePattern) )
			continue;

		Position latest = m_Position;

		if (!(latest + movePattern).IsValid())
			continue;

		while (CheckDirectionIsViable(latest, movePattern)) {
			m_LegalMoves.push_back(latest + movePattern);
			m_ControlledSquares.push_back(latest + movePattern);
			latest += movePattern;
		}

		if (!(latest + movePattern).IsValid())
			continue;

		// if latest+movePattern is a piece, regardless of color,
		// the square is controlled by this piece
		m_ControlledSquares.push_back(latest + movePattern);

		bool isEnemyPiece =
				m_OwnerBoard->IsPieceCapturable(latest + movePattern, m_Color) &&
				m_OwnerBoard->GetPiece(latest + movePattern)->GetPieceName() != "en_passant";

		if (isEnemyPiece) {
			m_LegalMoves.push_back(latest + movePattern);
			if (m_OwnerBoard->GetPiece(latest + movePattern)->GetPieceName() == "king") {
				latest += movePattern;

				while ((latest + movePattern).IsValid()) {
					m_ControlledSquares.push_back(latest + movePattern);
					latest += movePattern;
				}
			} else {
				CheckPiecePinned(latest + movePattern, movePattern);
			}
		}
	}
}

bool SlidingPiece::CheckDirectionIsViable(Position latest, Position movePattern) {
	return  ((latest + movePattern).IsValid() &&
			 !m_OwnerBoard->IsSquareOccupied(latest + movePattern));
}

bool SlidingPiece::CheckPiecePinned(Position pinCheckPiecePos, Position movePattern) {
	Piece *pinCheckPiece = m_OwnerBoard->GetPiece(pinCheckPiecePos);

	Position prevPinnedPiecePos = m_OwnerBoard->p_PinnedPiecePos[pinCheckPiece->GetColor()];
	bool wasPreviouslyPinned = (pinCheckPiecePos == prevPinnedPiecePos);

	Position currentPos = pinCheckPiecePos;
	while ((currentPos + movePattern).IsValid()) { // can check the next pos
		currentPos += movePattern;

		if (!m_OwnerBoard->IsSquareOccupied(currentPos)) // square is empty
			continue;

		// the only 2 defensive options to unpin a pinned pinCheckPiece are either:
		//
		// block the pin
		// or
		// move the king

		// if pin has been blocked
		if (m_OwnerBoard->GetPiece(currentPos)->GetPieceName() != "king") {

			// pin blocked in between pinCheckPiece and king
			if (wasPreviouslyPinned)
				UnPinPiece(pinCheckPiece);

			// pin blocked in between pinCheckPiece and pinner
			else if (prevPinnedPiecePos == currentPos)
				UnPinPiece(m_OwnerBoard->GetPiece(prevPinnedPiecePos));

			return false;
		}

		// unpin previously pinned pinCheckPiece
		if (prevPinnedPiecePos.IsValid() && m_OwnerBoard->IsSquareOccupied(prevPinnedPiecePos))
			UnPinPiece(m_OwnerBoard->GetPiece(prevPinnedPiecePos));

		PinPiece(pinCheckPiece, movePattern);

		return true; // pinCheckPiece is pinned
	}

	// the king has been moved
	if (wasPreviouslyPinned)
		UnPinPiece(pinCheckPiece);

	return false;
}

void SlidingPiece::PinPiece(Piece *p, Position dir) {
	p->Pin(dir);

	m_OwnerBoard->p_PinnedPiecePos[p->GetColor()] = p->GetPosition();
}

void SlidingPiece::UnPinPiece(Piece *p) {
	p->UnPin();

	m_OwnerBoard->p_PinnedPiecePos[p->GetColor()] = {-1, -1};
}


/////////////////////////////// Actual Pieces //////////////////////////////////

Bishop::Bishop(Color color, Position pos, float squareSize, Board *board)
		: SlidingPiece(color, pos, squareSize, "bishop", board) {
	m_MovePatterns.push_back({-1, -1});
	m_MovePatterns.push_back({-1, 1});
	m_MovePatterns.push_back({1, -1});
	m_MovePatterns.push_back({1, 1});
}

Rook::Rook(Color color, Position pos, float squareSize, Board *board)
		: SlidingPiece(color, pos, squareSize, "rook", board) {
	m_MovePatterns.push_back({0, -1});
	m_MovePatterns.push_back({0, 1});
	m_MovePatterns.push_back({-1, 0});
	m_MovePatterns.push_back({1, 0});
}

Queen::Queen(Color color, Position pos, float squareSize, Board *board)
		: SlidingPiece(color, pos, squareSize, "queen", board) {
	m_MovePatterns.push_back({-1, -1});
	m_MovePatterns.push_back({-1, 1});
	m_MovePatterns.push_back({1, -1});
	m_MovePatterns.push_back({1, 1});

	m_MovePatterns.push_back({0, -1});
	m_MovePatterns.push_back({0, 1});
	m_MovePatterns.push_back({-1, 0});
	m_MovePatterns.push_back({1, 0});
}