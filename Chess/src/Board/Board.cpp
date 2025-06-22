#include "Application.h"

#include "Board.h"
#include "PromotionBoard.h"

#include "Pieces/SlidingPieces.h"
#include "Pieces/SpecialPieces.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

Board::Board(const char *vertShaderPath, const char *fragShaderPath)
	: m_Layer(this), m_Turn(White), m_ActivatedSquare({-1, -1}),
	  m_SquareSize(2.f / 8.f) {
	m_EnPassantPieceInst = std::make_unique<EnPassantPiece>(this);
	m_EnPassantPositionPtr =
		dynamic_cast<EnPassantPiece *>(m_EnPassantPieceInst.get())
			->GetPosition();

	for (int i = 0; i < 64; i++) {
		Square &square = m_Board[i];

		square.pos = {i % 8, i / 8};
		float pos[3] = {
			(-1 + m_SquareSize / 2) + ((float) square.pos.file * m_SquareSize),
			(-1 + m_SquareSize / 2) + ((float) square.pos.rank * m_SquareSize),
			0};

		square.background = Engine::Renderer::GenQuad(
			pos, m_SquareSize, vertShaderPath, fragShaderPath
		);

		square.background.shader.SetUniform(
			square.background.shader.GetUniformLocation("isWhite"),
			!((square.pos.file + square.pos.rank) % 2)
		);

		square.piece = nullptr;
	}

	float pos[3] = {0, 0, 0};
	float tint_color[4] = {0, 0, 0, .333};
	m_ShadowObj = Engine::Renderer::GenQuad(
		pos, m_SquareSize * 8, vertShaderPath, fragShaderPath
	);

	m_ShadowObj.shader.SetUniformVec(
		m_ShadowObj.shader.GetUniformLocation("tint"), 4, tint_color
	);
	m_ShadowObj.shader.SetUniform(
		m_ShadowObj.shader.GetUniformLocation("tint_mix"), 1.f
	);
}

void Board::ReadFen(std::string fen) {
	enum Phase {
		GenPieces = 0,
		ActiveColor = 1,
		CheckCastling = 2,
		CheckEnPassant = 3,
		HalfMove = 4,
		FullMove = 5
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
		case GenPieces: GeneratePieces(fen, i, currentPos); break;
		case ActiveColor: m_Turn = (Color) (fen[i] == 'w'); break;
		case CheckCastling:
			if (fen[i] == '-')
				break;
			{
				king = dynamic_cast<King *>(
					GetPiece(p_KingPos[std::isupper(fen[i])])
				);
				k = (fen[i] == (std::isupper(fen[i]) ? 'K' : 'k'));
				q = (fen[i + 1] == (std::isupper(fen[i + 1]) ? 'Q' : 'q'));
				king->SetCastling(k, q);
				i++;
				break;
			}
		case CheckEnPassant:
			if (fen[i] == '-')
				break;
			{
				currentPos.file = (fen[i] - 'a');
				i++;
				currentPos.rank = (fen[i] - '1');
				Position pawnPos = currentPos + Position({0, 1});
				if (!GetPiece(pawnPos))
					pawnPos = currentPos + Position({0, -1});
				StartEnPassanting(
					dynamic_cast<Pawn *>(GetPiece(pawnPos)), currentPos, 0
				);
				break;
			}
		case HalfMove:
		case FullMove: break;
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
		case 'k':
			SetPiece(
				currentPos,
				std::make_unique<King>(color, currentPos, m_SquareSize, this)
			);
			p_KingPos[color] = currentPos; // set king pos
			break;
		case 'q':
			SetPiece(
				currentPos,
				std::make_unique<Queen>(color, currentPos, m_SquareSize, this)
			);
			break;
		case 'r':
			SetPiece(
				currentPos,
				std::make_unique<Rook>(color, currentPos, m_SquareSize, this)
			);
			break;
		case 'b':
			SetPiece(
				currentPos,
				std::make_unique<Bishop>(color, currentPos, m_SquareSize, this)
			);
			break;
		case 'n':
			SetPiece(
				currentPos,
				std::make_unique<Knight>(color, currentPos, m_SquareSize, this)
			);
			break;
		case 'p':
			SetPiece(
				currentPos,
				std::make_unique<Pawn>(
					color, currentPos, m_SquareSize, this,
					(color ? currentPos.rank == 1 : currentPos.rank == 6)
				)
			);
			break;
		}
		currentPos.file++;
	}
}

#define SHOW_CONTROLLED_SQUARES

void Board::RenderBoard() {
	// Instantiate a legal move which is then rendered
	// multiple times in different locations
	LegalMoveSprite legalMoveSpriteInst(
		m_SquareSize, m_SquareSize * 0.75f, {0, 0}, false
	);
	LegalMoveSprite captureSpriteInst(m_SquareSize, m_SquareSize, {0, 0}, true);

	for (auto &square : m_Board) {
#ifdef SHOW_CONTROLLED_SQUARES
		bool controlledByWhite = m_ControlledSquares[White].find(square.pos) !=
		                         m_ControlledSquares[White].end();
		bool controlledByBlack = m_ControlledSquares[Black].find(square.pos) !=
		                         m_ControlledSquares[Black].end();
		if (controlledByWhite) {
			float tint[4] = {0.75f, 0.5f, 0.5f, 1};
			square.background.shader.SetUniformVec(
				square.background.shader.GetUniformLocation("tint"), 4, tint
			);
			square.background.shader.SetUniform(
				square.background.shader.GetUniformLocation("tint_mix"), .66f
			);
		} else if (controlledByBlack) {
			float tint[4] = {0.5f, 0.75f, 0.5f, 1};
			square.background.shader.SetUniformVec(
				square.background.shader.GetUniformLocation("tint"), 4, tint
			);
			square.background.shader.SetUniform(
				square.background.shader.GetUniformLocation("tint_mix"), .66f
			);
		}
		if (controlledByWhite && controlledByBlack) {
			float tint[4] = {0.8f, 0.75f, 0.35f, 1};
			square.background.shader.SetUniformVec(
				square.background.shader.GetUniformLocation("tint"), 4, tint
			);
			square.background.shader.SetUniform(
				square.background.shader.GetUniformLocation("tint_mix"), .66f
			);
		}
#endif
		// FlipBoard(square);

		// Render background
		Engine::Renderer::SubmitObject(square.background);

		if (square.piece)
			square.piece->Render();
	}

	// If a piece is activated display its legal moves
	if (m_ActivatedSquare.IsValid()) {
		for (Position legalMove :
		     GetPiece(m_ActivatedSquare)->GetLegalMoves()) {
			if (IsSquareOccupied(legalMove)) {
				captureSpriteInst.SetPosition(legalMove);
				captureSpriteInst.Render();
			} else {
				legalMoveSpriteInst.SetPosition(legalMove);
				legalMoveSpriteInst.Render();
			}
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

void Board::FlipBoard(Square &square) {
	float pos[3] = {
		((float) square.pos.file * ((float) m_Turn * 2 - 1) -
	     7 * ((float) m_Turn - 1)),
		((float) square.pos.rank * ((float) m_Turn * 2 - 1) -
	     7 * ((float) m_Turn - 1)),
		0};
	pos[0] = (-1 + m_SquareSize / 2) + (pos[0] * m_SquareSize);
	pos[1] = (-1 + m_SquareSize / 2) + (pos[1] * m_SquareSize);
	Engine::Renderer::MoveQuad(square.background, pos, m_SquareSize);

	if (square.piece)
		Engine::Renderer::MoveQuad(
			square.piece->GetRendererObject(), pos, m_SquareSize
		);
}

unsigned int Board::CalculateAllLegalMoves(std::set<Move> *legalMoves) {
	// need to calculate both colors bc of revealed checks and such
	m_ControlledSquares[Black].clear();
	m_ControlledSquares[White].clear();

	for (auto pos : p_PinnedPiecePos)
		if (GetPiece(pos))
			GetPiece(pos)->UnPin();


	King *kings[2] = {
		dynamic_cast<King *>(GetPiece(p_KingPos[Black])),
		dynamic_cast<King *>(GetPiece(p_KingPos[White]))};

	for (auto &square : m_Board) {
		if (!square.piece || dynamic_cast<EnPassantPiece *>(square.piece.get()))
			continue;
		square.piece->CalculateLegalMoves();
		if (!square.piece)
			continue;

		m_ControlledSquares[square.piece->GetColor()].insert(
			square.piece->GetControlledSquares().begin(),
			square.piece->GetControlledSquares().end()
		);
	}

	GetEnPassantPiece()->CalculateLegalMoves();

	RecalculatePinnedPieceLegalMoves();

	// Calculate King legal moves again so that they can't walk into each other
	for (King *king : kings) {
		king->CalculateLegalMoves();

		Piece *checker = nullptr;
		if (king->IsInCheck(checker))
			RecalculateCheckLegalMoves(king, checker);
	}

	// Calculate number of moves and populate legalMoves and controlledSquares
	unsigned int numMoves = 0;
	for (auto &square : m_Board) {
		if (!square.piece)
			continue;

		if (square.piece->GetColor() == m_Turn) {
			numMoves += square.piece->GetLegalMoves().size();
			if (legalMoves)
				PopulatePieceLegalMovesIntoSet(legalMoves, square);
		}
	}

	return numMoves;
}

void Board::PopulatePieceLegalMovesIntoSet(
	std::set<Move> *legalMoves, Square &square
) {
	auto pawn = dynamic_cast<Pawn *>(square.piece.get());
	if (!pawn) {
		for (auto move : square.piece->GetLegalMoves())
			legalMoves->insert({square.pos, move});
		return;
	}

	for (auto move : square.piece->GetLegalMoves()) {
		if (pawn->CheckIsPromotionMove(move))
			// insert 4 possible moves for a single pawn promotion
			//  - one for each possible promotion piece
			for (int i = 1; i <= 4; i++)
				legalMoves->insert({square.pos, move, i});
		else
			legalMoves->insert({square.pos, move});
	}
}

void Board::RecalculatePinnedPieceLegalMoves() {
	for (auto piecePos : p_PinnedPiecePos)
		if (auto p = GetPiece(piecePos))
			p->CalculateLegalMoves();
}

void Board::RecalculateCheckLegalMoves(King *king, Piece *checker) {
	for (auto &square : m_Board) {
		if (!square.piece || square.piece->GetColor() != king->GetColor() ||
		    square.piece->GetPieceName() == "king")
			continue;

		if (!checker) {
			square.piece->ClearLegalMoves();
			continue;
		}

		// check if the move_ptr can kill the checker or block the check
		auto move_ptr = square.piece->GetLegalMoves().begin();
		while (move_ptr != square.piece->GetLegalMoves().end()) {
			if (king->DoesMoveBlockCheck(*move_ptr, checker))
				move_ptr++;
			else
				square.piece->RemoveLegalMove(move_ptr);
		}
	}
}

void Board::StartEnPassanting(Pawn *pawn, Position pos, int moveNum) {
	ResetEnPassantPiece();
	GetEnPassantPiece()->SetPawn(pawn, pos, moveNum);

	SetPiece(pos, std::move(m_EnPassantPieceInst));
}

void Board::ResetEnPassantPiece() {
	if (!m_EnPassantPositionPtr->IsValid()) // already reset
	{
		GetEnPassantPiece()->SetPawn(nullptr, {});
		return;
	}

	m_EnPassantPieceInst = GetFullPiecePtr(*m_EnPassantPositionPtr);
	GetEnPassantPiece()->SetPawn(nullptr, {});
}

EnPassantPiece *Board::GetEnPassantPiece() {
	auto ep = dynamic_cast<EnPassantPiece *>(m_EnPassantPieceInst.get());
	if (!ep)
		ep = dynamic_cast<EnPassantPiece *>(GetPiece(*m_EnPassantPositionPtr));
	return ep;
}

bool Board::MakeMove(Move move) {
	auto piece = GetPiece(move.from);
	if (!piece->Move(move.to)) // en passanting also happens in here
		return false;

	// Capture Piece if piece exists on ending square
	if (IsPieceCapturable(move.to, m_Turn)) {
		if (auto ep = dynamic_cast<EnPassantPiece *>(GetPiece(move.to))) {
			auto pawn = ep->CancelEnPassantOffer(
				GetPiece(move.from)->GetPieceName() == "pawn"
			);
			if (pawn) {
				m_CapturedPiecesCache[m_Turn].emplace(
					GetNumMovesPlayed(), std::move(pawn)
				);
			}
		} else
			m_CapturedPiecesCache[m_Turn].emplace(
				GetNumMovesPlayed(), GetFullPiecePtr(move.to)
			);
	}

	if (piece->GetPieceName() == "king")
		p_KingPos[piece->GetColor()] = move.to;

	// move piece
	SetPiece(move.to, GetFullPiecePtr(move.from));

	if (move.isPawnPromotionMove()) {
		// TODO: do what needs to be done if pawn promotion move
	}

	m_MovesPlayed.push_back(move);
	m_Turn = (Color) !m_Turn;

	return true;
}

void Board::UndoMove(Move move) {
	m_Turn = (Color) !m_Turn;
	m_MovesPlayed.pop_back();

	if (move.isPawnPromotionMove()) {
		// TODO: do what needs to be undone if pawn promotion move
	}

	auto piece = GetPiece(move.to);

	SetPiece(move.from, GetFullPiecePtr(move.to));
	if (piece->GetPieceName() == "king")
		p_KingPos[piece->GetColor()] = move.from;

	if (!m_CapturedPiecesCache[m_Turn].empty()) {
		auto &cap = m_CapturedPiecesCache[m_Turn].top();
		auto pos = cap.second->GetPosition();
		// only uncapture piece if this is the piece that was captured
		if (cap.first == GetNumMovesPlayed()) {
			SetPiece(pos, std::move(cap.second));
			m_CapturedPiecesCache[m_Turn].pop();
		}
	}

	piece->UndoMove(move.from);

	GetEnPassantPiece()->UndoMove({});
}

uint64_t Board::Perft(
	int depth, bool printMoves, const std::function<void()> &windowUpdate
) {
#ifdef DEBUG
#	define RENDER_PERFT(x)                                                    \
		RenderBoard();                                                         \
		windowUpdate();                                                        \
		std::this_thread::sleep_for(std::chrono::milliseconds(x))
#else
#	define RENDER_PERFT(x)
#endif

	std::set<Move> moveList;
	uint64_t nodes = 0;

	CalculateAllLegalMoves(&moveList);

	if (depth == 1)
		return moveList.size();

	for (auto move : moveList) {
		if (printMoves)
			std::cout << move << ": " << std::flush;

		Move testMove = {{4, 7}, {2, 7}};
		if (depth == 3 && move == testMove) {
			int i = 0;
		}
		MakeMove(move);
		RENDER_PERFT(0);

		auto moves = Perft(depth - 1, false, windowUpdate);
		if (printMoves)
			std::cout << moves << std::endl;

		nodes += moves;
		UndoMove(move);
		CalculateAllLegalMoves(&moveList);
		RENDER_PERFT(0);
	}
	return nodes;
}

void Board::GameOver() {}

void Board::ApplyOffset(float mouseX, float mouseY) {
	const Engine::RendererObject &pieceOBJ =
		GetPiece(m_ActivatedSquare)->GetRendererObject();
	float calculatedOffset[2] = {
		mouseX - pieceOBJ.position[0], mouseY - pieceOBJ.position[1]};

	// set the render offset so the piece follows the mouse
	pieceOBJ.shader.SetUniformVec(
		pieceOBJ.shader.GetUniformLocation("renderOffset"), 2, calculatedOffset
	);
}

bool Board::HandleMouseDown(Engine::MouseButtonPressedEvent &e) {
	Position invalid = {-1, -1};

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);

	Position squarePos = {
		(int) ((1 + mouseX) / m_SquareSize),
		(int) ((1 + mouseY) / m_SquareSize)};

	// if a piece is already activated, move move.to the new square (if
	// possible)
	if (m_ActivatedSquare != invalid &&
	    MakeMove({m_ActivatedSquare, squarePos})) {
		CalculateAllLegalMoves();
		m_ActivatedSquare = invalid;
	} else { // if a piece is not already selected, then select the piece
		     // under the mouse
		if (!GetPiece(squarePos) || GetPiece(squarePos)->GetColor() != m_Turn)
			return false;

		m_ActivatedSquare = squarePos;
		m_MouseReleased = false;
		ApplyOffset(mouseX, mouseY);
	}

	return true;
}

bool Board::HandleMouseReleased(Engine::MouseButtonReleasedEvent &e) {
	m_MouseReleased = true;
	Position invalid = {-1, -1};

	// do nothing if no piece is already selected
	if (m_ActivatedSquare == invalid)
		return false;

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);
	Position squarePos = {
		static_cast<int>((1 + mouseX) / m_SquareSize),
		static_cast<int>((1 + mouseY) / m_SquareSize)};

	if (squarePos == m_ActivatedSquare) {
		float offset[2] = {0, 0};

		// reset piece position, so that it is on the new square
		GetPiece(m_ActivatedSquare)
			->GetRendererObject()
			.shader.SetUniformVec(
				GetPiece(m_ActivatedSquare)
					->GetRendererObject()
					.shader.GetUniformLocation("renderOffset"),
				2, offset
			);
		return false;
	}

	if (MakeMove({m_ActivatedSquare, squarePos}))
		CalculateAllLegalMoves();

	m_ActivatedSquare = invalid;
	return true;
}

bool Board::HandleMouseMoved(Engine::MouseMovedEvent &e) {
	if (m_MouseReleased)
		return false;

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);

	ApplyOffset(mouseX, mouseY);
	return true;
}

bool Board::HandleKeyPressed(Engine::KeyPressedEvent &e) {
	if (e.GetKey() != GLFW_KEY_LEFT)
		return false;

	if (m_MovesPlayed.empty())
		return false;

	UndoMove(m_MovesPlayed.back());
	CalculateAllLegalMoves();
	m_ActivatedSquare = {};

	return true;
}

//////////////////////////////// BoardLayer
///////////////////////////////////////

BoardLayer::BoardLayer(Board *boardPtr) : m_BoardPtr(boardPtr) {}

bool BoardLayer::OnEvent(Engine::Event &e) {
	Engine::EventDispatcher dispatcher(e);

	if (dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(
			BIND_EVENT_FUNC(Board::HandleMouseDown, m_BoardPtr)
		))
		return true;
	if (dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(
			BIND_EVENT_FUNC(Board::HandleMouseReleased, m_BoardPtr)
		))
		return true;
	if (dispatcher.Dispatch<Engine::MouseMovedEvent>(
			BIND_EVENT_FUNC(Board::HandleMouseMoved, m_BoardPtr)
		))
		return true;
	if (dispatcher.Dispatch<Engine::KeyPressedEvent>(
			BIND_EVENT_FUNC(Board::HandleKeyPressed, m_BoardPtr)
		))
		return true;

	return false;
}
