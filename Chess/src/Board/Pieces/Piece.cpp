#include "Piece.h"

#include "Board/Board.h"

Piece::Piece(Color color, Position pos, float squareSize, const char *pieceName,
             Board *board, bool loadImage/*=true*/)
		: m_Color(color), m_Position(pos), m_OwnerBoard(board),
		  m_PieceName(pieceName), m_SquareSize(squareSize) {
	m_TexturePath.append((color == White ? "W_" : "B_"));
	m_TexturePath.append(pieceName);
	m_TexturePath.append(".png");
	float viewPos[3] = {(-1 + squareSize / 2) + ((float) pos.file * squareSize),
	                    (-1 + squareSize / 2) + ((float) pos.rank * squareSize), 1};

	m_PinnedDirection = {0, 0};

	if (loadImage) {
		m_Object = Engine::Renderer::GenQuad(
				viewPos, squareSize,
				"Assets/Shaders/Piece.vert",
				"Assets/Shaders/Piece.frag"
		);

		m_Object.shader.AttachTexture(Engine::Texture(m_TexturePath.c_str()));
	}
}

Piece::~Piece() {
	Engine::Renderer::DeleteQuad(m_Object);
}

bool Piece::Move(Position pos, bool overrideLegality/*=false*/) {
	bool legal = overrideLegality;

	if (!legal) { // if legality was not overridden
		for (const Position &legalMove: m_LegalMoves) {
			if (legalMove == pos)
				legal = true;
		}
	}

	if (legal) {
		if (m_IsVirgin)
			m_IsVirgin = false;

		m_Position = pos;

		float viewPos[3] = {(-1 + m_SquareSize / 2) + ((float)pos.file * m_SquareSize),
		                    (-1 + m_SquareSize / 2) + ((float)pos.rank * m_SquareSize),
		                    0};
		Engine::Renderer::MoveQuad(
				m_Object, viewPos,
				m_SquareSize
		); // Change VISUAL position

		float offset[2] = {0, 0};
		m_Object.shader.SetUniformVec(
				m_Object.shader.GetUniformLocation("renderOffset"),
				2,
				offset
		);
	} else {
		float offset[2] = {0, 0};
		m_Object.shader.SetUniformVec(
				m_Object.shader.GetUniformLocation("renderOffset"),
				2,
				offset
		);
	}

	return legal;
}

void Piece::Render() { Engine::Renderer::SubmitObject(m_Object); }
