#include "Board.h"

#include "Pieces/SlidingPieces.h"
#include "Pieces/SpecialPieces.h"

#include <string>
#include <iostream>
#include <sstream>

Board::Board(const char* vertShaderPath, const char* fragShaderPath)
	: m_Layer(this)
{
	m_Turn = Color::White;
	m_ActivatedSquare = {-1, -1};
	m_SquareSize = 2.f/8.f;

	for (int i=0; i<64; i++)
	{
		Square& square = m_Board[i];

		square.pos = { i % 8, i / 8 };
		float pos[3] = {(-1+ m_SquareSize /2) + (square.pos.file* m_SquareSize), (-1+ m_SquareSize /2) + (square.pos.rank* m_SquareSize), 0};
		square.obj = Engine::Renderer::GenQuad(pos, m_SquareSize, vertShaderPath, fragShaderPath);

		square.obj.shader.SetUniform(square.obj.shader.GetUniformLocation("isWhite"), ((square.pos.file + square.pos.rank) % 2));

		square.piece = nullptr;
	}
}

Board::~Board() {}

void Board::GenerateBoard(std::string str)
{
	Position currentPos = {0,7};

	for (int i=0; i < str.length(); i++)
	{
		if (str[i] == '/')
			currentPos = {0, currentPos.rank-1};
		else if (std::isdigit(str[i]))
		{
			currentPos.file += str[i] - '0';
		}
		else
		{
			int pos = currentPos.ToIndex();
			switch (std::tolower(str[i]))
			{
			case 'k':
				SetPiece(currentPos, std::make_unique<King>(static_cast<Color>(std::isupper(str[i])), currentPos, m_SquareSize, this));
				break;
			case 'q':
				m_Board[pos].piece = std::make_unique<Queen>(static_cast<Color>(std::isupper(str[i])), currentPos, m_SquareSize, this);
				break;
			case 'r':
				m_Board[pos].piece = std::make_unique<Rook>(static_cast<Color>(std::isupper(str[i])), currentPos, m_SquareSize, this);
				break;
			case 'b':
				m_Board[pos].piece = std::make_unique<Bishop>(static_cast<Color>(std::isupper(str[i])), currentPos, m_SquareSize, this);
				break;
			case 'n':
				m_Board[pos].piece = std::make_unique<Knight>(static_cast<Color>(std::isupper(str[i])), currentPos, m_SquareSize, this);
				break;
			case 'p':
				m_Board[pos].piece = std::make_unique<Pawn>(static_cast<Color>(std::isupper(str[i])), currentPos, m_SquareSize, this);
				break;
			default:
				continue;
			}
			currentPos.file++;
		}
	}

	CalculateAllLegalMoves();
}

void Board::RenderBoard()
{
	// Instanciate a legal move which is then rendered multiple times in different locations
	LegalMoveSprite legalMoveSpriteInst(m_SquareSize, m_SquareSize * 0.75f, {0,0});

	for (Square& square : m_Board)
	{
		// Highlight controlled squares
		/*
		{
			bool controlledByWhite = m_WhiteControlledSquares.find(square.pos) != m_WhiteControlledSquares.end();
			bool controlledByBlack = m_BlackControlledSquares.find(square.pos) != m_BlackControlledSquares.end();
			if (controlledByWhite)
			{
				float tint[4] = { 0.75f, 0.5f, 0.5f, 0.66f };
				square.obj.shader.SetUniformVec(square.obj.shader.GetUniformLocation("tint"), 4, tint);
			}
			else if (controlledByBlack)
			{
				float tint[4] = { 0.2f, 0.3f, 0.4f, 0.85f };
				square.obj.shader.SetUniformVec(square.obj.shader.GetUniformLocation("tint"), 4, tint);
			}
			if (controlledByWhite && controlledByBlack)
			{
				float tint[4] = { 0.2f, 0.5f, 0.2f, 0.5f };
				square.obj.shader.SetUniformVec(square.obj.shader.GetUniformLocation("tint"), 4, tint);
			}
		}
		*/

		// Render background
		Engine::Renderer::SubmitObject(square.obj);
		
		// Render every piece except for fake pieces
		if (square.piece && square.piece->GetPieceName() != "en_passant")
			square.piece->Render();
	}

	// If a piece is activated display its legal moves
	if (m_ActivatedSquare.file != -1)
	{
		for (const Position& legalMove : GetPiece(m_ActivatedSquare)->GetLegalMoves())
		{
			legalMoveSpriteInst.SetPosition(legalMove);
			legalMoveSpriteInst.Render();
		}

		// the piece might be covered up by the background squares
		// so render it on top
		GetPiece(m_ActivatedSquare)->Render();
	}
}

int Board::CalculateAllLegalMoves()
{
	m_WhiteControlledSquares.clear();
	m_BlackControlledSquares.clear();

	King* kings[2] = { nullptr, nullptr };

	int numOfMoves=0;
	for (int i=0; i<64; i++)
	{
		Square& square = m_Board[i];
		if (square.piece)
		{
			if (square.piece->GetPieceName() == "king")
			{
				if (!kings[0])
					kings[0] = dynamic_cast<King*>(square.piece.get());
				else
					kings[1] = dynamic_cast<King*>(square.piece.get());
				continue;
			}
			square.piece->CalculateLegalMoves();
			
			if (!square.piece)
				continue;

			if (square.piece->GetColor() == m_Turn)
				numOfMoves += square.piece->GetLegalMoves().size();

			for (Position pos : square.piece->GetControlledSquares())
			{
				if (square.piece->GetColor())
					m_WhiteControlledSquares.insert(pos);
				else
					m_BlackControlledSquares.insert(pos);
			}
		}
	}

	// Calculate both kings' controlled squares
	for (King* king : kings)
	{
		if (king)
		{
			king->CalculateLegalMoves();

			// Add controlled positions to the set
			for (Position pos : king->GetControlledSquares())
			{
				if (king->GetColor())
					m_WhiteControlledSquares.insert(pos);
				else
					m_BlackControlledSquares.insert(pos);
			}
		}
	}

	// Calculate King legal moves again so that they can't walk into each other
	for (King* king : kings)
		if (king)
			king->CalculateLegalMoves();

	std::cout << numOfMoves << std::endl;

	return numOfMoves;
}

bool Board::MakeMove(Piece* piece, Position from, Position to, bool overrideLegality/*=false*/)
{
	if (piece->Move(to, overrideLegality)) // if the move is actually possible
	{
		// Capture Piece if piece exists on ending square
		if (IsPieceCapturable(to, m_Turn))
		{
			std::cout << GetPiece(to)->GetPieceName() << std::endl;
			if (GetPiece(to)->GetPieceName() == "en_passant")
			{
				EnPassantPiece* enPassantPiece = dynamic_cast<EnPassantPiece*>(GetPiece(to));
				Position pawnPos = enPassantPiece->GetPosition();

				if (GetPiece(from)->GetPieceName() == "pawn")
					enPassantPiece->CancelEnPassantOffer(true);
				else
					enPassantPiece->CancelEnPassantOffer(false);
			}
			else
				DeletePiece(to); // capture piece

		}

		// move piece
		SetPiece(to, GetFullPiecePtr(from));

		// we don't want to switch the turn or calculate any legal moves
		// if the legality is overrided (for example because of castling)
		if (overrideLegality)
			return true;

		m_Turn = (Color)(!(bool)m_Turn); // switch turn
		CalculateAllLegalMoves();

		return true;
	}
	return false;
}

bool Board::HandleMouseDown(Engine::MouseButtonPressedEvent& e)
{
	Position invalid = {-1, -1};

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);
	
	Position squarePos = { (1 + mouseX) / m_SquareSize, (1 + mouseY) / m_SquareSize };
	
	
	if (m_ActivatedSquare != invalid)
	{ // if a piece is already activated, move to the new square (if possible)
		if (!MakeMove(GetPiece(m_ActivatedSquare), m_ActivatedSquare, squarePos))
		{
			float offset[2] = { 0, 0 };

			// reset piece position, so that it is on the new square
			GetPiece(m_ActivatedSquare)->GetRendererObject().shader.SetUniformVec(
				GetPiece(m_ActivatedSquare)->GetRendererObject().shader.GetUniformLocation("renderOffset"),
				2,
				offset
			);
		}

		m_ActivatedSquare = invalid;

		return true;
	}
	else
	{ // if a piece is not already selected, then select the piece under the mouse
		if (!GetPiece(squarePos))
			return true;
		else if (GetPiece(squarePos)->GetColor() != m_Turn)
			return true;

		m_ActivatedSquare = squarePos;
		return true;
	}
}

bool Board::HandleMouseReleased(Engine::MouseButtonReleasedEvent& e)
{
	Position invalid = { -1, -1 };

	// do nothing if no piece is already selected
	if (m_ActivatedSquare == invalid)
		return false;

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);
	Position squarePos = { (1 + mouseX) / m_SquareSize, (1 + mouseY) / m_SquareSize };
	
	if (squarePos == m_ActivatedSquare)
	{
		float offset[2] = { 0, 0 };

		// reset piece position, so that it is on the new square
		GetPiece(m_ActivatedSquare)->GetRendererObject().shader.SetUniformVec(
			GetPiece(m_ActivatedSquare)->GetRendererObject().shader.GetUniformLocation("renderOffset"),
			2,
			offset
		);
		return false;
	}

	if (MakeMove(GetPiece(m_ActivatedSquare), m_ActivatedSquare, squarePos))
	{}
	else
	{
		float offset[2] = { 0, 0 };

		// reset piece position, so that it is on the new square
		GetPiece(m_ActivatedSquare)->GetRendererObject().shader.SetUniformVec(
			GetPiece(m_ActivatedSquare)->GetRendererObject().shader.GetUniformLocation("renderOffset"),
			2,
			offset
		);
	}

	m_ActivatedSquare = invalid;
	return true;
}

bool Board::HandleMouseMoved(Engine::MouseMovedEvent& e)
{
	Position invalid = { -1, -1 };

	if (m_ActivatedSquare == invalid)
		return false;

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);

	const Engine::RendererObject& pieceOBJ = GetPiece(m_ActivatedSquare)->GetRendererObject();
	float calculatedOffset[2] = {mouseX - pieceOBJ.position[0], mouseY - pieceOBJ.position[1]};
	
	// set the render offset so the piece follows the mouse
	pieceOBJ.shader.SetUniformVec(
		pieceOBJ.shader.GetUniformLocation("renderOffset"),
		2,
		calculatedOffset
	);
	return true;
}

bool Board::IsValidPosition(Position pos)
{
	if (pos.file >= 0 && pos.file < 8 &&
		pos.rank >= 0 && pos.rank < 8)
		return true;
	else
		return false;
}

bool Board::IsSquareOccupied(Position pos) const
{ return GetPiece(pos) && GetPiece(pos)->GetPieceName() != "en_passant"; }

bool Board::IsPieceCapturable(Position pos, Color color)
{
	if (!IsValidPosition(pos))
		return false;

	if (GetPiece(pos))
		return GetPiece(pos)->GetColor() != color;
	else
		return false;
}

bool Board::IsInEnemyTerritory(Position pos, Color color)
{
	if (color)
	{
		return (m_BlackControlledSquares.find(pos) != m_BlackControlledSquares.end());
	}
	else
	{
		return (m_WhiteControlledSquares.find(pos) != m_WhiteControlledSquares.end());
	}
}

Piece* Board::GetPiece(Position pos) const
{
	return m_Board[pos.ToIndex()].piece.get();
}

std::unique_ptr<Piece> Board::GetFullPiecePtr(Position pos)
{
	return std::move(m_Board[pos.ToIndex()].piece);
}

void Board::SetPiece(Position pos, std::unique_ptr<Piece> piece)
{
	m_Board[pos.ToIndex()].piece = std::move(piece);
}

void Board::DeletePiece(Position pos)
{
	m_Board[pos.ToIndex()].piece.reset();
}

////////////////// BoardLayer ////////////////////////////////

BoardLayer::BoardLayer(Board* boardPtr)
	: m_BoardPtr(boardPtr)
{}

void BoardLayer::OnAttach() {}

void BoardLayer::OnDetach() {}

void BoardLayer::OnEvent(Engine::Event& e)
{
	Engine::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(std::bind(&Board::HandleMouseDown, m_BoardPtr, std::placeholders::_1));
	dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(std::bind(&Board::HandleMouseReleased, m_BoardPtr, std::placeholders::_1));
	dispatcher.Dispatch<Engine::MouseMovedEvent>(std::bind(&Board::HandleMouseMoved, m_BoardPtr, std::placeholders::_1));
}
