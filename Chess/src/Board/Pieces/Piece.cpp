#include "Piece.h"

#include "Board/Board.h"
#include "SpecialPieces.h"

#include <iostream>

Piece::Piece(Color color, Position pos, float squareSize, std::string pieceName, Board* board)
	: m_Color(color), m_Position(pos), m_TexturePath("Assets/Textures/Pieces/"), m_OwnerBoard(board), m_IsGamePiece(false), m_PieceName(pieceName), m_SquareSize(squareSize)
{
	m_TexturePath.append((color == White ? "W_" : "B_"));
	m_TexturePath.append(pieceName);
	m_TexturePath.append(".png");
	float viewPos[3] = { (-1 + squareSize/2) + (pos.file * squareSize), (-1 + squareSize/2) + (pos.rank * squareSize), 1};

	m_Object = Engine::Renderer::GenQuad(viewPos, squareSize, "Assets/Shaders/Piece.vert", "Assets/Shaders/Piece.frag");

	m_Object.shader.AttachTexture(Engine::Texture(m_TexturePath.c_str()));
}

Piece::~Piece()
{
	Engine::Renderer::DeleteQuad(m_Object);
}

bool Piece::Move(Position pos, bool overrideLegality/*=false*/)
{
	if (m_IsVirgin)
		m_IsVirgin = false;

	bool legal = overrideLegality;

	if (!legal)
	{
		for (const Position& legalMove : m_LegalMoves)
		{
			if (legalMove == pos)
				legal = true;
		}
	}

	if (legal)
	{
		m_Position = pos;

		float viewPos[3] = { (-1 + m_SquareSize / 2) + (pos.file * m_SquareSize), (-1 + m_SquareSize / 2) + (pos.rank * m_SquareSize), 0 };
		Engine::Renderer::MoveQuad(m_Object, viewPos, m_SquareSize); // Change VISUAL position

		float offset[2] = {0, 0};
		m_Object.shader.SetUniformVec(m_Object.shader.GetUniformLocation("renderOffset"), 2, offset);

		return true;
	}
	else
	{
		float offset[2] = { 0, 0 };
		m_Object.shader.SetUniformVec(m_Object.shader.GetUniformLocation("renderOffset"), 2, offset);
		return false;
	}
}

void Piece::Render()
{ Engine::Renderer::SubmitObject(m_Object); }
