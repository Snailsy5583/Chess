#include "Board.h"

#include "PromotionBoard.h"

#include "Pieces/SlidingPieces.h"
#include "Pieces/SpecialPieces.h"

#include <string>

Board::Board(const char *vertShaderPath, const char *fragShaderPath)
		: m_Layer(this) {
	m_Turn = Color::White;
	m_ActivatedSquare = {-1, -1};
	m_SquareSize = 2.f / 8.f;

	for (int i = 0; i < 64; i++) {
		Square &square = m_Board[i];

		square.pos = {i % 8, i / 8};
		float pos[3] = {
				(-1 + m_SquareSize / 2) + ((float) square.pos.file * m_SquareSize),
				(-1 + m_SquareSize / 2) + ((float) square.pos.rank * m_SquareSize),
				0
		};

		square.obj = Engine::Renderer::GenQuad(
				pos, m_SquareSize,
				vertShaderPath, fragShaderPath
		);

		square.obj.shader.SetUniform(
				square.obj.shader.GetUniformLocation("isWhite"),
				((square.pos.file + square.pos.rank) % 2));

		square.piece = nullptr;
	}

	float pos[3] = {0, 0, 0};
	float tint_color[4] = {0, 0, 0, .333};
	m_ShadowObj = Engine::Renderer::GenQuad
			(pos, m_SquareSize * 8, vertShaderPath, fragShaderPath);

	m_ShadowObj.shader.SetUniformVec(
			m_ShadowObj.shader.GetUniformLocation("tint"),
			4, tint_color);
	m_ShadowObj.shader.SetUniform
			(m_ShadowObj.shader.GetUniformLocation("tint_mix"), 1.f);
}

void Board::ReadFen(std::string fen) {
	enum Phase {
		GenPieces = 0, ActiveColor = 1, CheckCastling = 2,
		CheckEnPassant = 3, HalfMove = 4, FullMove = 5
	};

	Phase phase = Phase::GenPieces;

	Position currentPos = {0, 7};

	King *king;
	bool k, q;

	for (int i = 0; i < fen.length(); i++) {
		if (fen[i] == ' ') {
			phase = (Phase) (phase + 1);
			currentPos = {-1, -1};
			continue;
		}

		switch (phase) {
		case GenPieces:GeneratePieces(fen, i, currentPos);
			break;
		case ActiveColor:m_Turn = (Color) (fen[i] == 'w');
			break;
		case CheckCastling:
			king = dynamic_cast<King *>(GetPiece(
					(std::isupper(fen[i]) ? p_KingPos[White] : p_KingPos[Black])));
			k = (fen[i] == (std::isupper(fen[i]) ? 'K' : 'k'));
			q = (fen[i+1] == (std::isupper(fen[i+1]) ? 'Q' : 'q'));
			king->SetCastling(k, q);
			i++;
			break;
		case CheckEnPassant:
			if (currentPos.file < 0)
				currentPos.file = (fen[i] - 'a');
			else if (currentPos.rank < 0)
				currentPos.rank = (fen[i] - '1');
			else {
				Position pawn = currentPos + Position({0, 1});
				if (GetPiece(pawn))
					pawn = currentPos + Position({0, -1});
				SetPiece(
						currentPos,
						std::make_unique<EnPassantPiece>(
								currentPos,
								dynamic_cast<Pawn *>(GetPiece(
										pawn
								)),
								this
						));
			}
			break;
		case HalfMove:
		case FullMove:break;
		}
	}

	// Calculate moves after the board is set up.
	CalculateAllLegalMoves();
}

void Board::GeneratePieces(std::string fen, int i, Position &currentPos) {
	if (fen[i] == '/')
		currentPos = {0, currentPos.rank - 1};
	else if (std::isdigit(fen[i])) {
		currentPos.file += fen[i] - '0';
	} else {
		auto color = static_cast<Color>(std::isupper(fen[i]));
		switch (std::tolower(fen[i])) {
		case 'k':SetPiece(currentPos, std::make_unique<King>(color, currentPos, m_SquareSize, this));
			if (color)
				p_KingPos[White] = currentPos;
			else
				p_KingPos[Black] = currentPos;

			(color ? p_KingPos[White] : p_KingPos[Black]) = currentPos; // set king pos
			break;
		case 'q':
			SetPiece(
					currentPos, std::make_unique<Queen>(
							color, currentPos,
							m_SquareSize,
							this
					));
			break;
		case 'r':
			SetPiece(
					currentPos,
					std::make_unique<Rook>(
							color, currentPos, m_SquareSize,
							this
					));
			break;
		case 'b':
			SetPiece(
					currentPos, std::make_unique<Bishop>(
							color, currentPos,
							m_SquareSize,
							this
					));
			break;
		case 'n':
			SetPiece(
					currentPos, std::make_unique<Knight>(
							color, currentPos,
							m_SquareSize,
							this
					));
			break;
		case 'p':
			SetPiece(
					currentPos, std::make_unique<Pawn>(
							static_cast<Color>(std::isupper(fen[i])), currentPos,
							m_SquareSize, this
					));
			break;
		}
		currentPos.file++;
	}
}

#define SHOW_CONTROLLED_SQUARES

void Board::RenderBoard() {
	// Instantiate a legal move which is then rendered multiple times in
	// different locations
	LegalMoveSprite legalMoveSpriteInst(
			m_SquareSize, m_SquareSize * 0.75f,
			{0, 0}
	);

	for (Square &square: m_Board) {
// Highlight controlled squares
#ifdef SHOW_CONTROLLED_SQUARES
		bool controlledByWhite = m_ControlledSquares[White].find(square.pos) !=
								 m_ControlledSquares[White].end();
		bool controlledByBlack = m_ControlledSquares[Black].find(square.pos) !=
								 m_ControlledSquares[Black].end();
		if (controlledByWhite) {
			float tint[4] = {0.75f, 0.5f, 0.5f, 1};
			square.obj.shader.SetUniformVec(
				square.obj.shader.GetUniformLocation("tint"), 4, tint);
			square.obj.shader.SetUniform(
				square.obj.shader.GetUniformLocation("tint_mix"), .66f
			);
		} else if (controlledByBlack) {
			float tint[4] = {0.5f, 0.75f, 0.5f, 1};
			square.obj.shader.SetUniformVec(
				square.obj.shader.GetUniformLocation("tint"), 4, tint);
			square.obj.shader.SetUniform(
				square.obj.shader.GetUniformLocation("tint_mix"), .66f
			);
		}
		if (controlledByWhite && controlledByBlack) {
			float tint[4] = {0.8f, 0.75f, 0.35f, 1};
			square.obj.shader.SetUniformVec(
				square.obj.shader.GetUniformLocation("tint"), 4, tint);
			square.obj.shader.SetUniform(
				square.obj.shader.GetUniformLocation("tint_mix"), .66f
			);
		}
#endif

		// Render background
		Engine::Renderer::SubmitObject(square.obj);

		// Render every piece except for fake pieces
		if (square.piece && square.piece->GetPieceName() != "en_passant")
			square.piece->Render();
	}

	// If a piece is activated display its legal moves
	if (m_ActivatedSquare.file != -1) {
		for (
			const Position &legalMove: GetPiece(m_ActivatedSquare)->GetLegalMoves()) {
			legalMoveSpriteInst.SetPosition(legalMove);
			legalMoveSpriteInst.Render();
		}

		// the piece might be covered up by the background squares
		// so render it on top
		GetPiece(m_ActivatedSquare)->Render();
	}

	if (p_PromotionBoard) {
		Engine::Renderer::SubmitObject(m_ShadowObj);
		p_PromotionBoard->RenderBoard();
	}
}

int Board::CalculateAllLegalMoves() {
	// reset piece states
//	if (m_Turn == Color::White)
		m_ControlledSquares[White].clear();
//	else
		m_ControlledSquares[Black].clear();

	for (auto &square: m_Board)
		if (square.piece)
			square.piece->UnPin();

	King *kings[2] = {dynamic_cast<King *>(GetPiece(p_KingPos[White])),
	                  dynamic_cast<King *>(GetPiece(p_KingPos[Black]))};

	int numOfMoves = 0;
	for (auto &square: m_Board) {
		if (!square.piece)
			continue;

		square.piece->CalculateLegalMoves();

		// this is bc of the en_passant piece getting deleted when calculating
        if (!square.piece)
            continue;

		if (square.piece->GetColor() == m_Turn)
			numOfMoves += (int) square.piece->GetLegalMoves().size();

		for (Position pos : square.piece->GetControlledSquares()) {
			if (square.piece->GetColor())
				m_ControlledSquares[White].insert(pos);
			else
				m_ControlledSquares[Black].insert(pos);
		}
	}

	// Calculate King legal moves again so that they can't walk into each other
	for (King *king: kings) {
		if (king)
			king->CalculateLegalMoves();

		SlidingPiece* checker;
		if (king->IsInCheck(checker)) {
			// remove all legal moves
			for (auto &square: m_Board) {
				if (!square.piece || square.piece->GetPieceName() == "king")
					continue;

				// TODO: check if the move can block the check
				// TODO: check if the move can kill the checker

				square.piece->ClearLegalMoves();
			}
		}
	}

	for (auto piecePos : p_PinnedPiecePos) {
		auto piece = GetPiece(piecePos);

		if (piece)
			piece->CalculateLegalMoves();
	}

	return numOfMoves;
}

bool Board::MakeMove(Piece *piece, Position from, Position to,
                     bool overrideLegality/*=false*/) {
	if (piece->Move(to, overrideLegality)) // if the move is actually possible
	{
		// Capture Piece if piece exists on ending square
		if (IsPieceCapturable(to, m_Turn)) {
			if (GetPiece(to)->GetPieceName() == "en_passant") {
				auto *enPassantPiece = dynamic_cast<EnPassantPiece *>(GetPiece(
						to
				));

				if (GetPiece(from)->GetPieceName() == "pawn")
					enPassantPiece->CancelEnPassantOffer(true);
				else
					enPassantPiece->CancelEnPassantOffer(false);
			} else
				DeletePiece(to); // capture piece

		}

		if (piece->GetPieceName() == "king")
			(piece->GetColor() ? p_KingPos[White] : p_KingPos[Black]) = to;

		// move piece
		SetPiece(to, GetFullPiecePtr(from));

		// we don't want to switch the turn or calculate any legal moves
		// if the legality is overridden (for example because of castling)
		if (overrideLegality)
			return true;

		m_Turn = (Color) (!(bool) m_Turn); // switch turn
		CalculateAllLegalMoves();

		return true;
	}
	return false;
}

bool Board::HandleMouseDown(Engine::MouseButtonPressedEvent &e) {
	Position invalid = {-1, -1};

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);

	Position squarePos = {(int) ((1 + mouseX) / m_SquareSize),
	                      (int) ((1 + mouseY) / m_SquareSize)};


	if (m_ActivatedSquare != invalid) { // if a piece is already activated, move to the new square (if possible)
		if (!MakeMove(
				GetPiece(m_ActivatedSquare), m_ActivatedSquare,
				squarePos
		)) {
			float offset[2] = {0, 0};

			// reset piece position, so that it is on the new square
			GetPiece(
					m_ActivatedSquare
			)->GetRendererObject().shader.SetUniformVec(
					GetPiece(
							m_ActivatedSquare
					)->GetRendererObject().shader.GetUniformLocation(
							"renderOffset"
					),
					2,
					offset
			);
		}

		m_ActivatedSquare = invalid;
	} else { // if a piece is not already selected, then select the piece under the mouse
		if (!GetPiece(squarePos) || GetPiece(squarePos)->GetColor() != m_Turn)
			return false;

		m_ActivatedSquare = squarePos;
	}

	return true;
}

bool Board::HandleMouseReleased(Engine::MouseButtonReleasedEvent &e) {
	Position invalid = {-1, -1};

	// do nothing if no piece is already selected
	if (m_ActivatedSquare == invalid)
		return false;

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);
	Position squarePos = {static_cast<int>((1 + mouseX) / m_SquareSize),
	                      static_cast<int>((1 + mouseY) / m_SquareSize)};

	if (squarePos == m_ActivatedSquare) {
		float offset[2] = {0, 0};

		// reset piece position, so that it is on the new square
		GetPiece(m_ActivatedSquare)->GetRendererObject().shader.SetUniformVec(
				GetPiece(
						m_ActivatedSquare
				)->GetRendererObject().shader.GetUniformLocation(
						"renderOffset"
				),
				2,
				offset
		);
		return false;
	}

	if (MakeMove(GetPiece(m_ActivatedSquare), m_ActivatedSquare, squarePos)) {}
	else {
		float offset[2] = {0, 0};

		// reset piece position, so that it is on the new square
		GetPiece(m_ActivatedSquare)->GetRendererObject().shader.SetUniformVec(
				GetPiece(
						m_ActivatedSquare
				)->GetRendererObject().shader.GetUniformLocation(
						"renderOffset"
				),
				2,
				offset
		);
	}

	m_ActivatedSquare = invalid;
	return true;
}

bool Board::HandleMouseMoved(Engine::MouseMovedEvent &e) {
	Position invalid = {-1, -1};

	if (m_ActivatedSquare == invalid)
		return false;

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);

	const Engine::RendererObject &pieceOBJ = GetPiece(
			m_ActivatedSquare
	)->GetRendererObject();
	float calculatedOffset[2] = {mouseX - pieceOBJ.position[0],
	                             mouseY - pieceOBJ.position[1]};

	// set the render offset so the piece follows the mouse
	pieceOBJ.shader.SetUniformVec(
			pieceOBJ.shader.GetUniformLocation("renderOffset"),
			2,
			calculatedOffset
	);
	return true;
}

bool Board::IsSquareOccupied(Position pos) const {
	return pos.IsValid() && GetPiece(pos) &&
	       GetPiece(pos)->GetPieceName() != "en_passant";
}

bool Board::IsPieceCapturable(Position pos, Color color) const {
	if (!pos.IsValid())
		return false;

	if (GetPiece(pos))
		return GetPiece(pos)->GetColor() != color;
	else
		return false;
}

bool Board::IsInEnemyTerritory(Position pos, Color color) {
	if (color) {
		return (m_ControlledSquares[Black].find(pos) !=
		        m_ControlledSquares[Black].end());
	} else {
		return (m_ControlledSquares[White].find(pos) !=
		        m_ControlledSquares[White].end());
	}
}

Piece *Board::GetPiece(Position pos) const {
	if (!pos.IsValid())
		return nullptr;
	return m_Board[pos.ToIndex()].piece.get();
}

std::unique_ptr<Piece> Board::GetFullPiecePtr(Position pos) {
	return std::move(m_Board[pos.ToIndex()].piece);
}

void Board::SetPiece(Position pos, std::unique_ptr<Piece> piece) {
	m_Board[pos.ToIndex()].piece = std::move(piece);
}

void Board::DeletePiece(Position pos) {
	m_Board[pos.ToIndex()].piece.reset();
}

/////////////////////////////////// BoardLayer /////////////////////////////////

BoardLayer::BoardLayer(Board *boardPtr)
		: m_BoardPtr(boardPtr) {}

void BoardLayer::OnAttach() {}

void BoardLayer::OnDetach() {}

bool BoardLayer::OnEvent(Engine::Event &e) {
	Engine::EventDispatcher dispatcher(e);

	if (dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(
			std::bind(&Board::HandleMouseDown, m_BoardPtr, std::placeholders::_1)
	))
		return true;
	if (dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(
			std::bind(&Board::HandleMouseReleased, m_BoardPtr, std::placeholders::_1)
	))
		return true;
	if (dispatcher.Dispatch<Engine::MouseMovedEvent>(
			std::bind(&Board::HandleMouseMoved, m_BoardPtr, std::placeholders::_1)
	))
		return true;

	return false; // if it hasn't returned true already
}
