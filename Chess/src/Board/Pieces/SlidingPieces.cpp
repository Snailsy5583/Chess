#include "SlidingPieces.h"

#include "Board/Board.h"

SlidingPiece::SlidingPiece(Color color, Position pos, float squareSize,
                           char *pieceName,
                           Board *board)
    : Piece(color, pos, squareSize, pieceName, board) {}

void SlidingPiece::CalculateLegalMoves() {
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
            } else
                FindPinnedPiece(latest, movePattern);
        }
    }

}

bool SlidingPiece::CheckDirectionIsViable(Position latest,
                                          Position movePattern) {
    return
        (
            (latest + movePattern).IsValid() &&
            (
                !m_OwnerBoard->IsSquareOccupied(latest + movePattern) ||
                m_OwnerBoard->GetPiece(latest + movePattern)->GetPieceName() ==
                "en_passant"
            )
        );
}

bool SlidingPiece::FindPinnedPiece(Position curPos, Position movePattern) {
    curPos += movePattern;
    Position piecePos = curPos;

    bool isAlreadyPinned = (m_PinnedPiecePos == piecePos);

    bool pinned = false;
    while ((curPos + movePattern).IsValid()) {
        if (m_OwnerBoard->IsSquareOccupied(curPos + movePattern)) {
            if (m_OwnerBoard->GetPiece(curPos + movePattern)->GetPieceName() ==
                "king") {
                pinned = true;

                Piece *piece = m_OwnerBoard->GetPiece(piecePos);
                piece->Pin(movePattern);

                if (!isAlreadyPinned) {
                    if (m_PinnedPiecePos.IsValid()) {
                        std::cout << "unpinned: " << m_PinnedPiecePos.ToString()
                                  << std::endl;
                        Piece *prevPinnedPiece = m_OwnerBoard->GetPiece(
                            m_PinnedPiecePos);
                        prevPinnedPiece->UnPin();
                        prevPinnedPiece->CalculateLegalMoves();

                        m_PinnedPiecePos = {-1, -1};
                    } else
                        piece->CalculateLegalMoves();
                }

                m_PinnedPiecePos = piecePos;
            } else if (m_PinnedPiecePos.IsValid()) {
                std::cout << "unpinned: " << m_PinnedPiecePos.ToString()
                          << std::endl;
                Piece *prevPinnedPiece = m_OwnerBoard->GetPiece(
                    m_PinnedPiecePos);

                // this means we killed the piece
                // without it, there will be an invisible infinite loop
                if (!prevPinnedPiece || prevPinnedPiece == this) {
                    m_PinnedPiecePos = {-1, -1};
                    break;
                }

                prevPinnedPiece->UnPin();
                prevPinnedPiece->CalculateLegalMoves();

                m_PinnedPiecePos = {-1, -1};
            }
            break;
        }

        curPos += movePattern;
    }

    if (!pinned && isAlreadyPinned && m_PinnedPiecePos.IsValid()) {
        Piece *piece = m_OwnerBoard->GetPiece(m_PinnedPiecePos);
        piece->UnPin();
        piece->CalculateLegalMoves();

        m_PinnedPiecePos = {-1, -1};
    }

    return pinned;
}

/////////////////////////////// Actual Pieces ////////////////////////////////////

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