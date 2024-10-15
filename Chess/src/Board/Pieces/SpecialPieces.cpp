#include "SpecialPieces.h"
#include "Application.h"
#include "Board/PromotionBoard.h"

#include <iostream>


////////////////////////// King ////////////////////////////////////////////////////////

King::King(Color color, Position pos, float squareSize, Board *board,
           bool isVirgin/*=true*/)
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

	for (
		const Position &movePattern: m_MovePatterns
			) {
		// king actually cannot be pinned to anything

		if (!(m_Position + movePattern).IsValid())
			continue;

		if (
				(
						!m_OwnerBoard->IsSquareOccupied(m_Position + movePattern)
						|| m_OwnerBoard->IsPieceCapturable(
								m_Position + movePattern,
								m_Color
						)
				)
				&&
				!m_OwnerBoard->IsInEnemyTerritory(m_Position + movePattern, m_Color)
				)
			m_LegalMoves.push_back(m_Position + movePattern);

		m_ControlledSquares.push_back(m_Position + movePattern);
	}

	CheckCastling(-1);
	CheckCastling(1);
}

bool King::IsInCheck(SlidingPiece* &checker) {
	checker = nullptr;
	if (!m_OwnerBoard->IsInEnemyTerritory(m_Position, m_Color))
		return false;

	SlidingPiece* piece;
	for (int i = 0; i < 64; i++) {
		int file = i%8, rank = i/8;

		piece = dynamic_cast<SlidingPiece*>(m_OwnerBoard->GetPiece({file, rank}));
		if (!piece || piece->GetColor() == this->GetColor())
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

	// Can't Castle King Side
	if (direction > 0 && !m_CanCastleK)
		return false;
	// Can't Castle Queen Side
	if (direction < 0 && !m_CanCastleQ)
		return false;

	Position movePattern = Position({direction, 0});
	Position rookPos{(direction < 0 ? 0 : 7), (m_Color ? 0 : 7)};

	bool areCastleSquaresOccupied =
			m_OwnerBoard->IsSquareOccupied(m_Position + movePattern) ||
			m_OwnerBoard->IsSquareOccupied(m_Position + (movePattern * 2));
	bool areSquaresInCheck =
			m_OwnerBoard->IsInEnemyTerritory(m_Position + movePattern, m_Color) ||
			m_OwnerBoard->IsInEnemyTerritory(m_Position + movePattern * 2, m_Color);
	if (!areCastleSquaresOccupied && !areSquaresInCheck) {
		Piece *piece = m_OwnerBoard->GetPiece(rookPos);

		if (piece &&
		    piece->GetPieceName() == "rook" &&
		    piece->GetIsVirgin()) // if the piece is a rook that hasn't moved
		{
			// The king is able to castle
			m_LegalMoves.push_back(m_Position + movePattern * 2);
			return true;
		}
	}

	return false;
}

bool King::Move(Position pos, bool overrideLegality/* =false */) {
	Position prev = m_Position;

	if (Piece::Move(pos, overrideLegality)) {
		if ((prev - pos).file > 1 ||
		    (prev - pos).file < -1) // if king just castled
		{
			Position rookPos;
			if ((prev - pos).file < 0)  // castle right side
				rookPos = {7, (m_Color ? 0 : 7)};
			else                        // castle left side
				rookPos = {0, (m_Color ? 0 : 7)};


			Piece *rook = m_OwnerBoard->GetPiece(rookPos);

			m_OwnerBoard->MakeMove(
					rook, rookPos,
					((prev - pos).file < 0 ? prev +
					                         Position({1, 0}) :
					 prev - Position({1, 0})), true
			);
		}
		return true;
	} else
		return false;
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

	for (const Position &movePattern: m_MovePatterns) {
		if (m_PinnedDirection != Position({0, 0}))
			break; // knight can't move when pinned

		if (!(m_Position + movePattern).IsValid())
			continue;

		m_ControlledSquares.push_back(m_Position + movePattern);

		if (!m_OwnerBoard->IsSquareOccupied(m_Position + movePattern) ||
				m_OwnerBoard->GetPiece(m_Position + movePattern)->GetColor() != m_Color)
			m_LegalMoves.push_back(m_Position + movePattern);
	}
}

///////////////////////////////// Pawn /////////////////////////////////////////

Pawn::Pawn(Color color, Position pos, float squareSize, Board *board)
		: Piece(color, pos, squareSize, "pawn", board) {
	m_MovePatterns.push_back({0, (color ? 1 : -1)});
	m_MovePatterns.push_back({0, (color ? 2 : -2)});

	m_MovePatterns.push_back({-1, (color ? 1 : -1)});
	m_MovePatterns.push_back({1, (color ? 1 : -1)});
}

void Pawn::CalculateLegalMoves() {
	m_LegalMoves.clear();
	m_ControlledSquares.clear();

	bool isValidSquare = (m_Position + m_MovePatterns[0]).IsValid();
	bool moveIsValid = isValidSquare && !m_OwnerBoard->IsSquareOccupied(
			m_Position + m_MovePatterns[0]
	);

	bool pawnIsPinnedInMoveDir = (
			m_PinnedDirection == m_MovePatterns[0] ||
			m_PinnedDirection == -m_MovePatterns[0]
	);

	// Check if pawn can be Pushed
	if (moveIsValid && (m_PinnedDirection == Position({0, 0}) ||
	                    pawnIsPinnedInMoveDir)) {
		m_LegalMoves.push_back(m_Position + m_MovePatterns[0]);

		// Check if pawn can be pushed twice
		if (m_IsVirgin) {
			if (!m_OwnerBoard->IsSquareOccupied(
					m_Position + m_MovePatterns[1]
			)) {
				m_LegalMoves.push_back(m_Position + m_MovePatterns[1]);
			}
		} else {
			std::cout << m_Position.ToString() << " pawn is not a virgin\n";
		}
	}

	// Check if pawn can capture
	if (
			(m_Position + m_MovePatterns[2]).IsValid()
			&& // pinned directions
			(!m_IsPinned || (m_PinnedDirection == m_MovePatterns[2] ||
			                 m_PinnedDirection == -m_MovePatterns[2]))
			) {
		m_ControlledSquares.push_back(m_Position + m_MovePatterns[2]);

		if (m_OwnerBoard->IsPieceCapturable(
				m_Position + m_MovePatterns[2],
				m_Color
		))
			m_LegalMoves.push_back(m_Position + m_MovePatterns[2]);
	}
	if (
			(m_Position + m_MovePatterns[3]).IsValid()
			&& // pinned directions
			(!m_IsPinned || (m_PinnedDirection == m_MovePatterns[3] ||
			                 m_PinnedDirection == -m_MovePatterns[3]))
			) {
		m_ControlledSquares.push_back(m_Position + m_MovePatterns[3]);

		if (m_OwnerBoard->IsPieceCapturable(
				m_Position + m_MovePatterns[3],
				m_Color
		))
			m_LegalMoves.push_back(m_Position + m_MovePatterns[3]);
	}

}

bool Pawn::Move(Position pos, bool overrideLegality) {
	Position prev = m_Position;
	if (Piece::Move(pos, overrideLegality)) {
		Position enPassantPos = m_Position + Position{0, -((m_Color * 2) - 1)};

		// pawn promotion
		if (m_Color == Color::White ? m_Position.rank == 7 : m_Position.rank == 0) {
			auto pb = std::make_unique<PromotionBoard>(
					m_Position, m_OwnerBoard, m_Color,
					"Assets/Shaders/Board.vert",
					"Assets/Shaders/Board.frag"
			);
			Application::AddLayer(pb->GetBoardLayer());
			m_OwnerBoard->p_PromotionBoard = std::move(pb);
		}

		// Do the En Passant things
		if (abs((pos - prev).rank) > 1) // If pawn moved two squares
		{
			// create fake piece to make this pawn en passant-able
			m_OwnerBoard->SetPiece(
					enPassantPos,
					std::make_unique<EnPassantPiece>(
							enPassantPos, this, m_OwnerBoard
					));
			return true;
		}
		return true;
	}
	return false;
}

Piece *Pawn::Promote(std::unique_ptr<Piece> piece) {
	// remove pawn from chess board
	auto pawn = std::move(m_OwnerBoard->GetFullPiecePtr(m_Position));

	// promote the pawn into whatever piece was chosen
	m_OwnerBoard->SetPiece(m_Position, std::move(piece));
	m_OwnerBoard->GetPiece(m_Position)->Move(m_Position, true);

	m_OwnerBoard->CalculateAllLegalMoves();

	return m_OwnerBoard->GetPiece(m_Position);

	// this pawn gets deleted here bc we have the pawn unique_ptr
}

void Pawn::Render() {
	Piece::Render();
}

///////////////////// En Passant Placeholder ///////////////////////////////////

EnPassantPiece::EnPassantPiece(Position pos, Pawn *originalPawn, Board *board)
		: m_FirstMove(true), m_OriginalPawn(originalPawn),
		  Piece::Piece(originalPawn->GetColor(), pos, 0, "en_passant", board) {}

void EnPassantPiece::CancelEnPassantOffer(bool deletePawn/*=false*/) {
	if (deletePawn)
		DeleteOwningPawn();

	m_OwnerBoard->DeletePiece(m_Position);
}

void EnPassantPiece::DeleteOwningPawn() {
	m_OwnerBoard->DeletePiece(m_OriginalPawn->GetPosition());
}

void EnPassantPiece::CalculateLegalMoves() {
	if (m_FirstMove) {
		m_FirstMove = false;
	} else
		CancelEnPassantOffer();

	m_LegalMoves.clear();
}

/////////////////////////// Legal Move Sprite //////////////////////////////////

LegalMoveSprite::LegalMoveSprite(float squareSize, float spriteSize,
                                 Position pos) {
	float viewPos[3] = {static_cast<float>(pos.file),
	                    static_cast<float>(pos.rank), 1};
	//float viewPos[3] = { (-1 + m_SquareSize / 2) + (pos.file * m_SquareSize), (-1 + m_SquareSize / 2) + (pos.rank * m_SquareSize) };
	m_SquareSize = squareSize;

	m_Obj = Engine::Renderer::GenQuad(
			viewPos, spriteSize,
			"Assets/Shaders/Piece.vert",
			"Assets/Shaders/Piece.frag"
	);
	m_Obj.shader.AttachTexture(
			Engine::Texture("Assets/Textures/LegalMove.png"));
	float tint[4] = {0.3f, 0.3f, 0.3f, 0.75f};
	m_Obj.shader.SetUniformVec(
			m_Obj.shader.GetUniformLocation("tint"), 4,
			tint
	);
}

LegalMoveSprite::~LegalMoveSprite() { Engine::Renderer::DeleteQuad(m_Obj); }

void LegalMoveSprite::SetPosition(Position pos) const {
	float viewPos[2] = {(-1 + m_SquareSize / 2) + ((float)pos.file * m_SquareSize),
	                    (-1 + m_SquareSize / 2) + ((float)pos.rank * m_SquareSize)};
	m_Obj.shader.SetUniformVec(
			m_Obj.shader.GetUniformLocation("renderOffset"),
			2, viewPos
	);
}

void LegalMoveSprite::Render() const { Engine::Renderer::SubmitObject(m_Obj); }