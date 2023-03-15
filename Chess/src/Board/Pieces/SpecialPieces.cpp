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
				// TODO: Check whether king will be walking into check
			)
		{
			m_LegalMoves.push_back(m_Position + movePattern);
		}

	}

	CheckCastling(-1);
	CheckCastling(1);

	return m_LegalMoves;
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
			Position rookPos = m_Position + (movePattern *
				(direction > 0 ? 3 : 4)); // if king is white and movePattern is to the right of it then check 3 spaces to the right for the rook else check 4 spaces in the other direction
			if (!m_OwnerBoard->IsValidPosition(rookPos))
				return false;
			Piece* piece = m_OwnerBoard->GetPiecePtr(rookPos);
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
		if ((prev - pos).file > 1 || (prev - pos).file < -1) // move is castling
		{
			Position rookPos = pos;
			if ((prev - pos).file < 0)
				rookPos = { 7, (m_Color ? 0 : 7) };
			else
				rookPos = { 0, (m_Color ? 0 : 7) };


			Piece* rook = m_OwnerBoard->GetPiecePtr(rookPos);

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
	: Piece(color, pos, squareSize, "pawn", board),	m_EnPassantPos({ -1, -1 }), m_FirstMove(false)
{
	m_MovePatterns.push_back({ 0,  (color ? 1 : -1) });
	m_MovePatterns.push_back({ 0,  (color ? 2 : -2) });

	m_MovePatterns.push_back({ -1,   (color ? 1 : -1) });
	m_MovePatterns.push_back({  1,   (color ? 1 : -1) });
}

Pawn::~Pawn()
{
	CancelEnPassantOffer(true);
	Piece::~Piece();
}

const std::vector<Position>& Pawn::CalculateLegalMoves()
{
	if (m_FirstMove)
		m_FirstMove = false;
	else
		CancelEnPassantOffer();

	m_LegalMoves.clear();

	if (!m_OwnerBoard->IsSquareOccupied(m_Position + m_MovePatterns[0]))
	{ // Check if it can be Pushed
		m_LegalMoves.push_back(m_Position + m_MovePatterns[0]);
		if (m_IsVirgin && !m_OwnerBoard->IsSquareOccupied(m_Position + m_MovePatterns[1])) // Check if it can go forward twice
			m_LegalMoves.push_back(m_Position + m_MovePatterns[1]);
	}

	// Check Capturing
	if (m_OwnerBoard->IsValidPosition(m_Position+m_MovePatterns[2]) && m_OwnerBoard->IsPieceCapturable(m_Position + m_MovePatterns[2], m_Color))
		m_LegalMoves.push_back(m_Position+m_MovePatterns[2]);
	if (m_OwnerBoard->IsValidPosition(m_Position + m_MovePatterns[3]) && m_OwnerBoard->IsPieceCapturable(m_Position + m_MovePatterns[3], m_Color))
		m_LegalMoves.push_back(m_Position + m_MovePatterns[3]);
	
	return m_LegalMoves;
}

bool Pawn::Move(Position pos, bool overrideLegality)
{
	Position prev = m_Position;
	if (Piece::Move(pos, overrideLegality))
	{
		// En Passant hack
		if (abs((pos - prev).rank) > 1) // If pawn moved two squares
		{
			m_FirstMove = true;
			m_EnPassantPos = pos - Position({ 0, (m_Color ? 1 : -1) });
			m_OwnerBoard->SetPiece(m_EnPassantPos, std::make_unique<Pawn>(*this));
			// Set previous square as also containing the piece
			// So that another pawn has the ability to capture it.
			return true;
		}
		return true;
	}
	return false;
}

void Pawn::CancelEnPassantOffer(bool deleteOriginal/*=false*/)
{
	if (!m_OwnerBoard->IsValidPosition(m_EnPassantPos))
		return;

	Piece* enPassantPiece = m_OwnerBoard->GetPiecePtr(m_EnPassantPos);

	if (enPassantPiece && enPassantPiece->GetColor() == m_Color && enPassantPiece->GetPieceName() == "pawn")
	{
		m_OwnerBoard->DeletePiece(m_EnPassantPos);
		m_EnPassantPos = {-1,-1};
		std::cout << "canceled\n";

		if (deleteOriginal)
			m_OwnerBoard->DeletePiece(m_Position);
	}
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
		else if (m_OwnerBoard->GetPiecePtr(m_Position+movePattern)->GetColor() != m_Color)
			m_LegalMoves.push_back(m_Position + movePattern);
	}
	return m_LegalMoves;
}

///////// Legal Move Sprite /////////////////

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

EnPassantPiece::EnPassantPiece(Position pos, Piece* originalPawn)
	: m_OriginalPawn(originalPawn)
{
	m_Position = pos;
}
