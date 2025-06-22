#include "Piece.h"
#include <iostream>

#include "Board/Board.h"

Piece::Piece(
	Color color, Position pos, float squareSize, const char *pieceName,
	Board *board, bool loadImage /*=true*/
)
	: m_Color(color), m_Position(pos), m_OwnerBoard(board),
	  m_PieceName(pieceName), m_SquareSize(squareSize) {
	m_TexturePath.append((color == White ? "W_" : "B_"));
	m_TexturePath.append(pieceName);
	m_TexturePath.append(".png");
	float viewPos[3] = {
		(-1 + squareSize / 2) + ((float) pos.file * squareSize),
		(-1 + squareSize / 2) + ((float) pos.rank * squareSize), 1};

	m_PinnedDirection = {0, 0};

	if (loadImage) {
		m_Object = Engine::Renderer::GenQuad(
			viewPos, squareSize, "Assets/Shaders/Piece.vert",
			"Assets/Shaders/Piece.frag"
		);

		m_Object.shader.AttachTexture(Engine::Texture(m_TexturePath.c_str()));
	}
}

Piece::~Piece() { Engine::Renderer::DeleteQuad(m_Object); }

bool Piece::Move(Position pos) {
	float offset[2] = {0, 0};
	m_Object.shader.SetUniformVec(
		m_Object.shader.GetUniformLocation("renderOffset"), 2, offset
	);

	if (!IsLegalMove(pos))
		return false;

	if (m_IsVirgin) {
		m_StartingPosition = m_Position;
		m_IsVirgin = false;
	}

	if (m_OwnerBoard->p_PinnedPiecePos[m_Color] == m_Position)
		m_OwnerBoard->p_PinnedPiecePos[m_Color] = pos;

	m_Position = pos;

	// move render obj
	float viewPos[3] = {
		(-1 + m_SquareSize / 2) + ((float) pos.file * m_SquareSize),
		(-1 + m_SquareSize / 2) + ((float) pos.rank * m_SquareSize), 0};
	Engine::Renderer::MoveQuad(m_Object, viewPos, m_SquareSize);

	return true;
}

void Piece::UndoMove(Position from) {
	float offset[2] {0, 0};
	m_Object.shader.SetUniformVec(
		m_Object.shader.GetUniformLocation("renderOffset"), 2, offset
	);

	if (m_OwnerBoard->p_PinnedPiecePos[m_Color] == m_Position)
		m_OwnerBoard->p_PinnedPiecePos[m_Color] = from;

	m_Position = from;
	if (m_Position == m_StartingPosition) // TODO: fix this later
		m_IsVirgin = true;

	// move render obj
	float viewPos[3] = {
		(-1 + m_SquareSize / 2) + ((float) from.file * m_SquareSize),
		(-1 + m_SquareSize / 2) + ((float) from.rank * m_SquareSize), 0};
	Engine::Renderer::MoveQuad(m_Object, viewPos, m_SquareSize);
}

void Piece::Render() {
	// #ifdef FLIP_BOARD
	// #endif
	Engine::Renderer::SubmitObject(m_Object);
}

bool Piece::IsLegalMove(Position sq) const {
	for (auto square : m_LegalMoves)
		if (square == sq)
			return true;
	return false;
}

bool Piece::IsControlledSquare(Position sq) const {
	for (auto square : m_ControlledSquares)
		if (square == sq)
			return true;
	return false;
}
