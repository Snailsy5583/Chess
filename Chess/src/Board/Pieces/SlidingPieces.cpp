#include "SlidingPieces.h"

#include "Board/Board.h"

SlidingPiece::SlidingPiece(Color color, Position pos, float squareSize,
                           char *pieceName,
                           Board *board)
    : Piece(color, pos, squareSize, pieceName, board) {}

void SlidingPiece::CalculateLegalMoves(bool findPinnedPieces) {
    m_LegalMoves.clear();
    m_ControlledSquares.clear();

    for (const Position &movePattern: m_MovePatterns) {
        // if this piece is pinned
        if (m_PinnedDirection != Position({0,0}) &&
            (m_PinnedDirection != movePattern &&
                m_PinnedDirection != -movePattern)) {
            continue;
        }

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
            m_OwnerBoard->GetPiece(latest + movePattern)->GetPieceName() !=
            "en_passant";
        if (isEnemyPiece) {
            m_LegalMoves.push_back(latest + movePattern);
            if (m_OwnerBoard->GetPiece(latest + movePattern)->GetPieceName() ==
                "king") {
                latest += movePattern;

                while ((latest + movePattern).IsValid()) {
                    m_ControlledSquares.push_back(latest + movePattern);
                    latest += movePattern;
                }
            } else if (findPinnedPieces)
                FindPinnedPiece(latest, movePattern);
        }
    }

}

void SlidingPiece::CalculateLegalMoves() {
    CalculateLegalMoves(true);
}

bool SlidingPiece::CheckDirectionIsViable(Position latest,
                                          Position movePattern) {
    return
        (
            (latest + movePattern).IsValid() &&
            !m_OwnerBoard->IsSquareOccupied(latest + movePattern)
        );
}

bool SlidingPiece::FindPinnedPiece(Position currentPos, Position movePattern) {
    currentPos += movePattern;

    Position piecePos = currentPos; // position of piece being pinned
    Piece *p = m_OwnerBoard->GetPiece(piecePos); // piece being pinned

    bool wasPreviouslyPinned = piecePos == m_PinnedPiecePos;

    while ((currentPos+movePattern).IsValid()) { // can check the next pos
        currentPos += movePattern;

        if (!m_OwnerBoard->IsSquareOccupied(currentPos)) // square is empty
            continue;
        else if (m_OwnerBoard->GetPiece(currentPos)->GetPieceName() != "king") {
            // the only 2 defensive options to unpin a pinned piece are either:
            //      block the pin or move the king

            // in this branch, a pin has been blocked

            // a pin can be blocked in between the piece and the pinner or
            // in between the piece and the king

            // pin blocked in between piece and king
            if (wasPreviouslyPinned)
                UnPinPiece(p);

            // pin blocked in between piece and pinner
            else if (m_PinnedPiecePos == currentPos)
                UnPinPiece(m_OwnerBoard->GetPiece(currentPos));

            break;
        }

        // unpin previously pinned piece
        if (m_PinnedPiecePos.IsValid() &&
            m_OwnerBoard->IsSquareOccupied(m_PinnedPiecePos)) {
            UnPinPiece(m_OwnerBoard->GetPiece(m_PinnedPiecePos));
        }

        PinPiece(p, movePattern);

        return true; // piece is pinned
    }

    if (wasPreviouslyPinned) { // the king has been moved, so unpin the piece
        UnPinPiece(p);
        return false; // piece is not pinned
    }
    return false;
}

void SlidingPiece::PinPiece(Piece *p, Position dir) {
    p->Pin(dir);
    m_PinnedPiecePos = p->GetPosition();

    auto *sp = dynamic_cast<SlidingPiece *>(p);
    if (sp)
        sp->CalculateLegalMoves(false);
    else
        p->CalculateLegalMoves();
}

void SlidingPiece::UnPinPiece(Piece *p) {
    p->UnPin();
    m_PinnedPiecePos = {-1, -1};

    auto *sp = dynamic_cast<SlidingPiece *>(p);
    if (sp)
        sp->CalculateLegalMoves(false);
    else
        p->CalculateLegalMoves();
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