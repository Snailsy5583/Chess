#include "SpecialPieces.h"

#include "SlidingPieces.h"

#include "Board/Board.h"

#include <iostream>

King::King(Color color, Position pos, float squareSize, Board* board, bool isVirgin/*=true*/)
	: Piece(color, pos, squareSize, "king", board), m_InCheck(false)
{
	m_IsGamePiece = true;
	m_IsVirgin = isVirgin;

	m_MovePatterns.push_back({ -1, -1 });
	m_MovePatterns.push_back({ -1,  1 });
	m_MovePatterns.push_back({ 1, -1 });
	m_MovePatterns.push_back({ 1,  1 });

	m_MovePatterns.push_back({ 0, -1 });
	m_MovePatterns.push_back({ 0,  1 });
	m_MovePatterns.push_back({ -1,  0 });
	m_MovePatterns.push_back({ 1,  0 });
}

const std::vector<Position>& King::CalculateLegalMoves()
{
	m_LegalMoves.clear();

	for (const Position& movePattern : m_MovePatterns)
	{
		if (!m_OwnerBoard->IsValidPosition(m_Position + movePattern))
			continue;

		if  (
				!m_OwnerBoard->IsSquareOccupied(m_Position + movePattern) ||
				m_OwnerBoard->IsPieceCapturable(m_Position + movePattern, m_Color)
			)
		{
			m_LegalMoves.push_back(m_Position + movePattern);

			if (m_IsVirgin
				&&
				(movePattern == Position({ -1, 0 }) || movePattern == Position({ 1, 0 }))
				)
			{
				// Check castling
				// TODO: check whether the king will be walking through check while castling
				bool castleCheck = !m_OwnerBoard->IsSquareOccupied(m_Position + movePattern) &&
					!m_OwnerBoard->IsSquareOccupied(m_Position + (movePattern * 2));
				std::cout << m_Color << " can almost castle\n";
				if (castleCheck)
				{
					Position rookPos = m_Position + (movePattern *
						(movePattern == Position({ 1, 0 }) ? 3 : 4)); // if king is white and movePattern is to the right of it then check 3 spaces to the right for the rook else check 4 spaces in the other direction
					Piece* piece = m_OwnerBoard->GetPiece(rookPos);
					if (piece && piece->GetPieceName() == "rook" && piece->GetIsVirgin())
					{
						m_LegalMoves.push_back(m_Position + movePattern*2); // The king is **FINALLY** able to castle
						std::cout << m_Color << " can fully castle\n";
					}
				}
			}
		}

	}

	return m_LegalMoves;
}

bool King::Move(Position pos, bool overrideLegality/* =false */)
{
	Position prev = m_Position;
	if (Piece::Move(pos, overrideLegality))
	{
		std::cout << (prev - pos).file << std::endl;
		if ((prev - pos).file > 1 || (prev - pos).file < -1) // move is castling
		{
			Position rookPos = pos;
			if ((prev - pos).file < 0)
				rookPos = { 7, (m_Color ? 0 : 7) };
			else
				rookPos = { 0, (m_Color ? 0 : 7) };
			std::cout << "(" << rookPos.file << ", " << rookPos.rank << ")\n";


			Piece* rook = m_OwnerBoard->GetPiece(rookPos);

			m_OwnerBoard->MakeMove(rook, rookPos, ((prev - pos).file < 0 ? prev + Position({1, 0}) : prev - Position({ 1, 0 })), true);
		}
		return true;
	}
	else
		return false;
}

void King::SetCheck(bool isInCheck)
{ m_InCheck = isInCheck; }


bool King::IsInCheck() const
{ return m_InCheck; }

Pawn::Pawn(Color color, Position pos, float squareSize, Board* board)
	: Piece(color, pos, squareSize, "pawn", board)
{
	m_MovePatterns.push_back({ 0,  (color ? 1 : -1) });
	m_MovePatterns.push_back({ 0,  (color ? 2 : -2) });

	m_MovePatterns.push_back({ -1,   (color ? 1 : -1) });
	m_MovePatterns.push_back({  1,   (color ? 1 : -1) });
}

const std::vector<Position>& Pawn::CalculateLegalMoves()
{
	m_LegalMoves.clear();

	if (!m_OwnerBoard->IsSquareOccupied(m_Position + m_MovePatterns[0]))
	{
		m_LegalMoves.push_back(m_Position + m_MovePatterns[0]);
		if (m_IsVirgin && !m_OwnerBoard->IsSquareOccupied(m_Position + m_MovePatterns[1]))
			m_LegalMoves.push_back(m_Position + m_MovePatterns[1]);
	}

	if (m_OwnerBoard->IsValidPosition(m_Position+m_MovePatterns[2]) && m_OwnerBoard->IsPieceCapturable(m_Position + m_MovePatterns[2], m_Color))
		m_LegalMoves.push_back(m_Position+m_MovePatterns[2]);
	if (m_OwnerBoard->IsValidPosition(m_Position + m_MovePatterns[3]) && m_OwnerBoard->IsPieceCapturable(m_Position + m_MovePatterns[3], m_Color))
		m_LegalMoves.push_back(m_Position + m_MovePatterns[3]);

	
	return m_LegalMoves;
}

Knight::Knight(Color color, Position pos, float squareSize, Board* board)
	: Piece(color, pos, squareSize, "knight", board)
{
	m_MovePatterns.push_back({  2,  1 });
	m_MovePatterns.push_back({  2, -1 });

	m_MovePatterns.push_back({ -2,  1 });
	m_MovePatterns.push_back({ -2, -1 });

	m_MovePatterns.push_back({  1,  2 });
	m_MovePatterns.push_back({ -1,  2 });

	m_MovePatterns.push_back({  1, -2 });
	m_MovePatterns.push_back({ -1, -2 });
}

const std::vector<Position>& Knight::CalculateLegalMoves()
{
	m_LegalMoves.clear();

	for (const Position& movePattern : m_MovePatterns)
	{
		if (!m_OwnerBoard->IsValidPosition(m_Position + movePattern))
			continue;

		if (!m_OwnerBoard->IsSquareOccupied(m_Position+movePattern))
			m_LegalMoves.push_back(m_Position + movePattern);
		else if (m_OwnerBoard->GetPiece(m_Position+movePattern)->GetColor() != m_Color)
			m_LegalMoves.push_back(m_Position + movePattern);
	}
	return m_LegalMoves;
}

///////// Legal Move Sprite /////////////////

LegalMoveSprite::LegalMoveSprite(float squareSize, float spriteSize, Position pos)
{
	float viewPos[3] = { (-1 + squareSize / 2) + (pos.file * squareSize), (-1 + squareSize / 2) + (pos.rank * squareSize), 1 };

	m_Obj = Engine::Renderer::GenQuad(viewPos, spriteSize, "Assets/Shaders/Piece.vert", "Assets/Shaders/Piece.frag");
	m_Obj.shader.AttachTexture(Engine::Texture("Assets/Textures/LegalMove.png"));
	float tint[4] = { 0.3f, 0.3f, 0.3f, 0.75f };
	m_Obj.shader.SetUniformVec(m_Obj.shader.GetUniformLocation("tint"), 4, tint);
}

LegalMoveSprite::~LegalMoveSprite()
{ Engine::Renderer::DeleteQuad(m_Obj); }

void LegalMoveSprite::Render() const
{ Engine::Renderer::SubmitObject(m_Obj); }
