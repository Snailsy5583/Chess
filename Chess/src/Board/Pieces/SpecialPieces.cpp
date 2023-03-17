#include "SpecialPieces.h"

#include "SlidingPieces.h"

#include "Board/Board.h"

#include <iostream>


////////////////////////// King ////////////////////////////////////////////////////////

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

void King::CalculateLegalMoves()
{
	m_LegalMoves.clear();
	m_ControlledSquares.clear();

	for (const Position& movePattern : m_MovePatterns)
	{
		if (!m_OwnerBoard->IsValidPosition(m_Position + movePattern))
			continue;

		if  (
				(
					!m_OwnerBoard->IsSquareOccupied(m_Position + movePattern)
					|| m_OwnerBoard->IsPieceCapturable(m_Position + movePattern, m_Color)
				)
				&& !m_OwnerBoard->IsInEnemyTerritory(m_Position + movePattern, m_Color)
			)
		{
			m_LegalMoves.push_back(m_Position + movePattern);
		}

		m_ControlledSquares.push_back(m_Position + movePattern);
	}

	CheckCastling(-1);
	CheckCastling(1);
}

const bool King::CheckCastling(int direction)
{
	if (m_IsVirgin)
	{
		Position movePattern = Position({ direction, 0 });
		// TODO: check whether the king will be walking through check while castling
		bool castleCheck = !m_OwnerBoard->IsSquareOccupied(m_Position + movePattern) &&
			!m_OwnerBoard->IsSquareOccupied(m_Position + (movePattern * 2));
		if (castleCheck)
		{
			//Position rookPos = m_Position + (movePattern * (direction > 0 ? 4 : 3) /** ((m_Color * 2)-1)*/); // if king is white and direction is to the right, check for the rook 3 spaces to the right, or else check 4 spaces to the other direction

			Position rookPos;
			if (direction < 0)
				rookPos = { 7, (m_Color ? 0 : 7) };
			else
				rookPos = { 0, (m_Color ? 0 : 7) };


			if (!m_OwnerBoard->IsValidPosition(rookPos))
				return false;
			Piece* piece = m_OwnerBoard->GetPiece(rookPos);
			if (m_OwnerBoard->IsSquareOccupied(rookPos) && piece->GetPieceName() == "rook" && piece->GetIsVirgin()) // if the piece is a rook that hasn't moved
			{
				m_LegalMoves.push_back(m_Position + movePattern * 2); // The king is **FINALLY** able to castle
				return true;
			}
		}
	}

	return false;
}

bool King::Move(Position pos, bool overrideLegality/* =false */)
{
	Position prev = m_Position;

	if (Piece::Move(pos, overrideLegality))
	{
		if ((prev - pos).file > 1 || (prev - pos).file < -1) // if king just castled
		{
			Position rookPos = pos;
			if ((prev - pos).file < 0)
				rookPos = { 7, (m_Color ? 0 : 7) };
			else
				rookPos = { 0, (m_Color ? 0 : 7) };


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

////////////////////////// Knight //////////////////////////////////////////////////////

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

void Knight::CalculateLegalMoves()
{
	m_LegalMoves.clear();
	m_ControlledSquares.clear();

	for (const Position& movePattern : m_MovePatterns)
	{
		if (!m_OwnerBoard->IsValidPosition(m_Position + movePattern))
			continue;

		m_ControlledSquares.push_back(m_Position + movePattern);

		if (!m_OwnerBoard->IsSquareOccupied(m_Position+movePattern))
			m_LegalMoves.push_back(m_Position + movePattern);
		else if (m_OwnerBoard->GetPiece(m_Position+movePattern)->GetColor() != m_Color)
			m_LegalMoves.push_back(m_Position + movePattern);
	}
}

///////////////////////////////// Pawn /////////////////////////////////////////////////

Pawn::Pawn(Color color, Position pos, float squareSize, Board* board)
	: Piece(color, pos, squareSize, "pawn", board)
{
	m_MovePatterns.push_back({ 0,  (color ? 1 : -1) });
	m_MovePatterns.push_back({ 0,  (color ? 2 : -2) });

	m_MovePatterns.push_back({ -1,   (color ? 1 : -1) });
	m_MovePatterns.push_back({  1,   (color ? 1 : -1) });
}

Pawn::~Pawn()
{
	Piece::~Piece();
}

void Pawn::CalculateLegalMoves()
{
	m_LegalMoves.clear();
	m_ControlledSquares.clear();

	// Check if pawn can be Pushed
	if (m_OwnerBoard->IsValidPosition(m_Position + m_MovePatterns[0])
		&& !m_OwnerBoard->IsSquareOccupied(m_Position + m_MovePatterns[0]))
	{
		m_LegalMoves.push_back(m_Position + m_MovePatterns[0]);

		// Check if pawn can be pushed twice
		if (m_IsVirgin && !m_OwnerBoard->IsSquareOccupied(m_Position + m_MovePatterns[1]))
			m_LegalMoves.push_back(m_Position + m_MovePatterns[1]);
	}

	// Check if pawn can capture
	if (m_OwnerBoard->IsValidPosition(m_Position+m_MovePatterns[2]))
	{
		m_ControlledSquares.push_back(m_Position + m_MovePatterns[2]);

		if (m_OwnerBoard->IsPieceCapturable(m_Position + m_MovePatterns[2], m_Color))
			m_LegalMoves.push_back(m_Position + m_MovePatterns[2]);
	}
	if (m_OwnerBoard->IsValidPosition(m_Position + m_MovePatterns[3]))
	{
		m_ControlledSquares.push_back(m_Position + m_MovePatterns[3]);

		if (m_OwnerBoard->IsPieceCapturable(m_Position + m_MovePatterns[3], m_Color))
			m_LegalMoves.push_back(m_Position + m_MovePatterns[3]);
	}

}

bool Pawn::Move(Position pos, bool overrideLegality)
{
	Position prev = m_Position;
	if (Piece::Move(pos, overrideLegality))
	{
		Position enPassantPos = m_Position + Position{ 0, -((m_Color * 2) - 1) };

		// Do the En Passant things
		if (abs((pos - prev).rank) > 1) // If pawn moved two squares
		{
			// create fake piece to make this pawn en passant-able
			m_OwnerBoard->SetPiece(enPassantPos, std::make_unique<EnPassantPiece>(enPassantPos, this, m_OwnerBoard));
			return true;
		}
		return true;
	}
	return false;
}

///////////////////// En Passant Placeholder ///////////////////////////////////////////

EnPassantPiece::EnPassantPiece(Position pos, Pawn* originalPawn, Board* board)
	: m_FirstMove(true), m_OriginalPawn(originalPawn), Piece::Piece(originalPawn->GetColor(),pos, 0, "en_passant", board)
{}

void EnPassantPiece::CancelEnPassantOffer(bool deletePawn/*=false*/)
{
	if (deletePawn)
		DeleteOwningPawn();

	m_OwnerBoard->DeletePiece(m_Position);
}

void EnPassantPiece::DeleteOwningPawn()
{
	m_OwnerBoard->DeletePiece(m_OriginalPawn->GetPosition());
}

void EnPassantPiece::CalculateLegalMoves()
{
	if (m_FirstMove)
	{
		m_FirstMove = false;
		std::cout << "first move\n";
	}
	else
		CancelEnPassantOffer();

	m_LegalMoves.clear();
}

/////////////////////////// Legal Move Sprite //////////////////////////////////////////

LegalMoveSprite::LegalMoveSprite(float squareSize, float spriteSize, Position pos)
{
	float viewPos[3] = { pos.file, pos.rank, 1 };
	//float viewPos[3] = { (-1 + m_SquareSize / 2) + (pos.file * m_SquareSize), (-1 + m_SquareSize / 2) + (pos.rank * m_SquareSize) };
	m_SquareSize = squareSize;

	m_Obj = Engine::Renderer::GenQuad(viewPos, spriteSize, "Assets/Shaders/Piece.vert", "Assets/Shaders/Piece.frag");
	m_Obj.shader.AttachTexture(Engine::Texture("Assets/Textures/LegalMove.png"));
	float tint[4] = { 0.3f, 0.3f, 0.3f, 0.75f };
	m_Obj.shader.SetUniformVec(m_Obj.shader.GetUniformLocation("tint"), 4, tint);
}

LegalMoveSprite::~LegalMoveSprite()
{ Engine::Renderer::DeleteQuad(m_Obj); }

void LegalMoveSprite::SetPosition(Position pos)
{
	float viewPos[2] = { (-1 + m_SquareSize / 2) + (pos.file * m_SquareSize), (-1 + m_SquareSize / 2) + (pos.rank * m_SquareSize) };
	m_Obj.shader.SetUniformVec(m_Obj.shader.GetUniformLocation("renderOffset"), 2, viewPos);
}

void LegalMoveSprite::Render() const
{ Engine::Renderer::SubmitObject(m_Obj); }