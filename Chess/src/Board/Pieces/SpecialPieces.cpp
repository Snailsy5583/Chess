#include "SpecialPieces.h"
#include "Application.h"
#include "Board/PromotionBoard.h"

#include <iostream>


////////////////////////// King
///////////////////////////////////////////////////////////

King::King(
	Color color, Position pos, float squareSize, Board *board,
	bool isVirgin /*=true*/
)
	: Piece(color, pos, squareSize, "king", board), m_CanCastleK(true),
	  m_CanCastleQ(true) {
	m_IsVirgin = isVirgin;

	m_MovePatterns.push_back({-1, -1});
	m_MovePatterns.push_back({-1, 1});
	m_MovePatterns.push_back({1, -1});
	m_MovePatterns.push_back({1, 1});

	m_MovePatterns.push_back({0, -1});
	m_MovePatterns.push_back({0, 1});
	m_MovePatterns.push_back({-1, 0});
	m_MovePatterns.push_back({1, 0});
}

void King::CalculateLegalMoves() {
	m_LegalMoves.clear();
	m_ControlledSquares.clear();

	for (const Position &movePattern : m_MovePatterns) {
		if (!(m_Position + movePattern).IsValid())
			continue;

		if ((!m_OwnerBoard->IsSquareOccupied(m_Position + movePattern) ||
		     m_OwnerBoard->IsPieceCapturable(m_Position + movePattern, m_Color)
		    ) &&
		    !m_OwnerBoard->IsInEnemyTerritory(
				m_Position + movePattern, m_Color
			)) {
			m_LegalMoves.push_back(m_Position + movePattern);
		}

		m_ControlledSquares.push_back(m_Position + movePattern);
	}

	Piece *p;
	if (!IsInCheck(p)) {
		CheckCastling(-1);
		CheckCastling(1);
	}
}

bool King::IsInCheck(Piece *&checker) {
	checker = nullptr;
	if (!m_OwnerBoard->IsInEnemyTerritory(m_Position, m_Color))
		return false;

	Piece *piece;
	for (int i = 0; i < 64; i++) {
		int file = i % 8, rank = i / 8;

		piece = m_OwnerBoard->GetPiece({file, rank});
		if (!piece || piece->GetColor() == m_Color)
			continue;

		if (piece->IsLegalMove(m_Position)) {
			if (!checker) {
				checker = piece;
			} else {
				// if there's multiple pieces, it's not possible to
				// kill one of the pieces to stop check
				checker = nullptr;
				return true;
			}
		}
	}

	return true;
}

bool King::CheckCastling(int direction) {
	if (!m_IsVirgin) // if piece already moved
		return false;

	bool kingSide = (direction > 0);
	bool queenSide = (direction < 0);

	if ((kingSide && !m_CanCastleK) || (queenSide && !m_CanCastleQ))
		return false;

	Position movePattern {direction, 0};
	Position rookPos {(direction < 0 ? 0 : 7), (m_Color ? 0 : 7)};

	bool areCastleSquaresOccupied =
		m_OwnerBoard->IsSquareOccupied(m_Position + movePattern) ||
		m_OwnerBoard->IsSquareOccupied(m_Position + movePattern * 2) ||
		(queenSide &&
	     m_OwnerBoard->IsSquareOccupied(m_Position + movePattern * 3));
	bool areSquaresInCheck =
		m_OwnerBoard->IsInEnemyTerritory(m_Position + movePattern, m_Color) ||
		m_OwnerBoard->IsInEnemyTerritory(
			m_Position + movePattern * 2, m_Color
		) ||
		(queenSide &&
	     m_OwnerBoard->IsInEnemyTerritory(m_Position + movePattern * 3, m_Color)
	    );

	if (!areCastleSquaresOccupied && !areSquaresInCheck) {
		Piece *piece = m_OwnerBoard->GetPiece(rookPos);

		if (piece && piece->GetPieceName() == "rook" && piece->GetIsVirgin()) {
			m_LegalMoves.push_back(m_Position + movePattern * 2);
			return true;
		}
	}

	return false;
}

bool King::DoesMoveBlockCheck(Position move, Piece *checker) {
	if (!dynamic_cast<SlidingPiece *>(checker))
		return move == checker->GetPosition();

	Position dir = (GetPosition() - checker->GetPosition()).Normalized();

	for (Position p = checker->GetPosition(); p != GetPosition(); p += dir)
		if (move == p)
			return true;
	return false;
}

bool King::Move(Position pos) {
	Position prev = m_Position;

	if (!Piece::Move(pos))
		return false;

	if ((prev - pos).file > 1 || (prev - pos).file < -1) // if king just castled
	{
		Position rookPos;
		if ((prev - pos).file < 0) // castle right side
			rookPos = {7, (m_Color ? 0 : 7)};
		else // castle left side
			rookPos = {0, (m_Color ? 0 : 7)};


		Rook *rook = dynamic_cast<Rook *>(m_OwnerBoard->GetPiece(rookPos));

		rook->Castle(
			((prev - pos).file < 0 ? prev + Position({1, 0})
		                           : prev - Position({1, 0}))
		);
	}
	return true;
}

void King::SetCastling(bool K, bool Q) {
	m_CanCastleK = K;
	m_CanCastleQ = Q;
}

////////////////////////// Knight //////////////////////////////////////////////

Knight::Knight(Color color, Position pos, float squareSize, Board *board)
	: Piece(color, pos, squareSize, "knight", board) {
	m_MovePatterns.push_back({2, 1});
	m_MovePatterns.push_back({2, -1});

	m_MovePatterns.push_back({-2, 1});
	m_MovePatterns.push_back({-2, -1});

	m_MovePatterns.push_back({1, 2});
	m_MovePatterns.push_back({-1, 2});

	m_MovePatterns.push_back({1, -2});
	m_MovePatterns.push_back({-1, -2});
}

void Knight::CalculateLegalMoves() {
	m_LegalMoves.clear();
	m_ControlledSquares.clear();

	for (const Position &movePattern : m_MovePatterns) {
		if (m_PinnedDirection != Position({0, 0}))
			break; // knight can't move when pinned

		if (!(m_Position + movePattern).IsValid())
			continue;

		m_ControlledSquares.push_back(m_Position + movePattern);

		if (!m_OwnerBoard->IsSquareOccupied(m_Position + movePattern) ||
		    m_OwnerBoard->GetPiece(m_Position + movePattern)->GetColor() !=
		        m_Color)
			m_LegalMoves.push_back(m_Position + movePattern);
	}
}

///////////////////////////////// Pawn /////////////////////////////////////////

Pawn::Pawn(
	Color color, Position pos, float squareSize, Board *board,
	bool isVirgin /*=true*/
)
	: Piece(color, pos, squareSize, "pawn", board) {
	m_IsVirgin = isVirgin;

	m_MovePatterns.push_back({0, (color ? 1 : -1)});
	m_MovePatterns.push_back({0, (color ? 2 : -2)});

	m_MovePatterns.push_back({-1, (color ? 1 : -1)});
	m_MovePatterns.push_back({1, (color ? 1 : -1)});
}

void Pawn::CalculateLegalMoves() {
	m_LegalMoves.clear();
	m_ControlledSquares.clear();

	bool isValidSquare = (m_Position + m_MovePatterns[0]).IsValid();
	bool moveIsValid =
		isValidSquare &&
		!m_OwnerBoard->IsSquareOccupied(m_Position + m_MovePatterns[0]);

	bool pawnIsPinnedInMoveDir =
		(m_PinnedDirection == m_MovePatterns[0] ||
	     m_PinnedDirection == -m_MovePatterns[0]);

	// Check if pawn can be Pushed
	if (moveIsValid &&
	    (m_PinnedDirection == Position({0, 0}) || pawnIsPinnedInMoveDir)) {
		m_LegalMoves.push_back(m_Position + m_MovePatterns[0]);

		// Check if pawn can be pushed twice
		if (m_IsVirgin) {
			if (!m_OwnerBoard->IsSquareOccupied(
					m_Position + m_MovePatterns[1]
				)) {
				m_LegalMoves.push_back(m_Position + m_MovePatterns[1]);
			}
		}
	}

	// Check if pawn can capture
	if ((m_Position + m_MovePatterns[2]).IsValid() &&
	    (!m_IsPinned || (m_PinnedDirection == m_MovePatterns[2] ||
	                     m_PinnedDirection == -m_MovePatterns[2]))) {
		m_ControlledSquares.push_back(m_Position + m_MovePatterns[2]);

		if (m_OwnerBoard->IsPieceCapturable(
				m_Position + m_MovePatterns[2], m_Color
			))
			m_LegalMoves.push_back(m_Position + m_MovePatterns[2]);
	}
	if ((m_Position + m_MovePatterns[3]).IsValid() && //
	    (!m_IsPinned || (m_PinnedDirection == m_MovePatterns[3] ||
	                     m_PinnedDirection == -m_MovePatterns[3]))) {
		m_ControlledSquares.push_back(m_Position + m_MovePatterns[3]);

		if (m_OwnerBoard->IsPieceCapturable(
				m_Position + m_MovePatterns[3], m_Color
			))
			m_LegalMoves.push_back(m_Position + m_MovePatterns[3]);
	}
}

bool Pawn::Move(Position pos) {
	Position prev = m_Position;
	if (Piece::Move(pos)) {
		// pawn promotion
		if (m_Color ? m_Position.rank == 7 : m_Position.rank == 0) {
			auto pb = std::make_unique<PromotionBoard>(
				m_Position, m_OwnerBoard, m_Color, "Assets/Shaders/Board.vert",
				"Assets/Shaders/Board.frag"
			);
			Application::AddLayer(pb->GetBoardLayer());
			m_OwnerBoard->p_PromotionBoard = std::move(pb);
		}

		// En Passant
		if (abs((pos - prev).rank) > 1) {
			auto enPassantPos = m_Position + Position {0, -((m_Color * 2) - 1)};

			m_OwnerBoard->ResetEnPassantPiece();
			m_OwnerBoard->StartEnPassanting(this, enPassantPos);
			return true;
		}
		return true;
	}
	return false;
}

///////////////////// En Passant Placeholder ///////////////////////////////////

EnPassantPiece::EnPassantPiece(Board *board)
	: p_OriginalPawn(nullptr),
	  Piece::Piece(White, {-1, -1}, 0, "en_passant", board) {}

void EnPassantPiece::SetPawn(Pawn *pawn, Position pos) {
	m_PreviousOriginalPawn = p_OriginalPawn;
	m_PreviousPosition = m_Position;

	m_Position = pos;
	p_OriginalPawn = pawn;
	m_Color = pawn ? pawn->GetColor() : White;
	m_FirstMove = pawn;
}

std::unique_ptr<Piece>
EnPassantPiece::CancelEnPassantOffer(bool deletePawn /*=false*/) {
	std::unique_ptr<Piece> pawn;
	if (deletePawn)
		pawn = m_OwnerBoard->GetFullPiecePtr(p_OriginalPawn->GetPosition());

	m_OwnerBoard->ResetEnPassantPiece();
	return pawn;
}

void EnPassantPiece::UndoMove(Position from) {
	if (!m_PreviousOriginalPawn)
		return;

	auto pawn = m_PreviousOriginalPawn;
	auto pos = m_PreviousPosition;

	m_OwnerBoard->ResetEnPassantPiece();
	m_OwnerBoard->StartEnPassanting(pawn, pos);
}

void EnPassantPiece::CalculateLegalMoves() {
	if (m_FirstMove)
		m_FirstMove = false;
	else
		CancelEnPassantOffer();
}

/////////////////////////// Legal Move Sprite //////////////////////////////////

LegalMoveSprite::LegalMoveSprite(
	float squareSize, float spriteSize, Position pos, bool capture
) {
	float viewPos[3] = {(float) (pos.file), (float) (pos.rank), 1};
	// float viewPos[3] = { (-1 + m_SquareSize / 2) + (pos.file * m_SquareSize),
	// (-1 + m_SquareSize / 2) + (pos.rank * m_SquareSize) };
	m_SquareSize = squareSize;

	m_Obj = Engine::Renderer::GenQuad(
		viewPos, spriteSize, "Assets/Shaders/Piece.vert",
		"Assets/Shaders/Piece.frag"
	);
	m_Obj.shader.AttachTexture(Engine::Texture(
		(capture ? "Assets/Textures/Capture.png"
	             : "Assets/Textures/LegalMove.png")
	));
	float tint[4] = {0.35f, 0.35f, 0.35f, 0.5f};
	m_Obj.shader.SetUniformVec(
		m_Obj.shader.GetUniformLocation("tint"), 4, tint
	);
}

LegalMoveSprite::~LegalMoveSprite() { Engine::Renderer::DeleteQuad(m_Obj); }

void LegalMoveSprite::SetPosition(Position pos) const {
	float viewPos[2] = {
		(-1 + m_SquareSize / 2) + ((float) pos.file * m_SquareSize),
		(-1 + m_SquareSize / 2) + ((float) pos.rank * m_SquareSize)};
	m_Obj.shader.SetUniformVec(
		m_Obj.shader.GetUniformLocation("renderOffset"), 2, viewPos
	);
}

void LegalMoveSprite::Render() const { Engine::Renderer::SubmitObject(m_Obj); }