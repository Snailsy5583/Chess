#include "PromotionBoard.h"

#include "Application.h"
#include "Board.h"
#include "Board/Pieces/SlidingPieces.h"
#include "Board/Pieces/SpecialPieces.h"

PromotionBoard::PromotionBoard(
	Position position, Board *board, Color color, const char *vertShaderPath,
	const char *fragShaderPath
)
	: m_Layer(this), m_Board(board), m_Origin(position), m_Color(color),
	  m_PromotionBoard(5) {
	float squareSize = m_Board->m_SquareSize;
	for (int i = 0; i < 5; i++) {
		Square &square = m_PromotionBoard[i];

		square.pos = {position.file, (m_Color == Color::White ? 7 - i : i)};
		float pos[3] = {
			(-1 + squareSize / 2) + ((float) square.pos.file * squareSize),
			(-1 + squareSize / 2) + ((float) square.pos.rank * squareSize), 0};

		square.background = Engine::Renderer::GenQuad(
			pos, squareSize, vertShaderPath, fragShaderPath
		);

		square.background.shader.SetUniform(
			square.background.shader.GetUniformLocation("isWhite"),
			((square.pos.file + square.pos.rank) % 2)
		);


		square.piece = nullptr;
		switch (i) {
		case 1:
			SetPiece(
				square.pos,
				std::make_unique<Queen>(color, square.pos, squareSize, board)
			);
			break;
		case 2:
			SetPiece(
				square.pos,
				std::make_unique<Rook>(color, square.pos, squareSize, board)
			);
			break;
		case 3:
			SetPiece(
				square.pos,
				std::make_unique<Bishop>(color, square.pos, squareSize, board)
			);
			break;
		case 4:
			SetPiece(
				square.pos,
				std::make_unique<Knight>(color, square.pos, squareSize, board)
			);
			break;
		default:
			SetPiece(
				square.pos,
				std::make_unique<Pawn>(color, square.pos, squareSize, board)
			);
			break;
		}
	}
}

PromotionBoard::~PromotionBoard() { Application::GetLayerStack()->PopFront(); };

void PromotionBoard::RenderBoard() {
	for (Square &square : m_PromotionBoard) {
		// Render every piece except for fake pieces
		Engine::Renderer::SubmitObject(square.background);

		if (square.piece && square.piece->GetPieceName() != "en_passant")
			square.piece->Render();
	}
}

// always returns true because we don't want events passing through
bool PromotionBoard::HandleMouseReleased(Engine::MouseButtonReleasedEvent &e) {
	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);

	Position squarePos {
		(int) ((1 + mouseX) / m_Board->m_SquareSize),
		(int) ((1 + mouseY) / m_Board->m_SquareSize)};

	// square that was clicked on
	Square *chosenSquare = GetSquare(squarePos, true);

	// if chosen square does not exist we still don't want the
	// real board to be handling events
	if (!chosenSquare)
		return true;

	auto pawn = dynamic_cast<Pawn *>(m_Board->GetPiece(m_Origin));
	//	pawn->Promote<decltype(chosenSquare->piece.get())>();
	switch (m_Color ? 7 - squarePos.rank : squarePos.rank) {
	case 1: pawn->Promote<Queen>(); break;
	case 2: pawn->Promote<Rook>(); break;
	case 3: pawn->Promote<Bishop>(); break;
	case 4: pawn->Promote<Knight>(); break;
	}

	m_Board->p_PromotionBoard.reset();
	return true; // event was handled
}

void PromotionBoard::SetPiece(Position pos, std::unique_ptr<Piece> piece) {
	Square *square = GetSquare(pos);
	square->piece = std::move(piece);
}

Square *PromotionBoard::GetSquare(Position pos, bool ignore0 /*=false*/) {
	if (pos.file != m_Origin.file)
		return nullptr;

	int index = (m_Color == Color::White ? 7 - pos.rank : pos.rank);

	if (index > 4 || index < (ignore0 ? 1 : 0))
		return nullptr;

	return &m_PromotionBoard[index];
}

////////////////// PromotionBoardLayer ////////////////////////////////

PromotionBoardLayer::PromotionBoardLayer(PromotionBoard *boardPtr)
	: m_BoardPtr(boardPtr) {}

void PromotionBoardLayer::OnAttach() {}

void PromotionBoardLayer::OnDetach() {}

bool PromotionBoardLayer::OnEvent(Engine::Event &e) {
	Engine::EventDispatcher dispatcher(e);

	if (dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(
			[](const Engine::MouseButtonPressedEvent &) -> bool { return true; }
		))
		return true;
	if (dispatcher.Dispatch<Engine::MouseMovedEvent>(
			[](const Engine::MouseMovedEvent &) -> bool { return true; }
		))
		return true;
	if (dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(
			BIND_EVENT_FUNC(PromotionBoard::HandleMouseReleased, m_BoardPtr)
		))
		return true;

	return false; // if it hasn't returned true
}
