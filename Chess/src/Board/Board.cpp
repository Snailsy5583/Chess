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

Board::~Board()
{
	for (Square& square : m_Board)
	{
		if (square.piece && square.piece->GetPosition() == square.pos)
			square.piece;
	}
}

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
			int pos = Pos2Index(currentPos);
			switch (std::tolower(str[i]))
			{
			case 'k':
				m_Board[pos].piece = std::make_unique<King>(static_cast<Color>(std::isupper(str[i])), currentPos, m_SquareSize, this);
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

int Board::CalculateAllLegalMoves()
{
	m_WhiteControlledSquares.clear();
	m_BlackControlledSquares.clear();

	int numOfMoves=0;
	for (int i=0; i<64; i++)
	{
		Square& square = m_Board[i];
		if (square.piece && Position({i%8, i/8}) == square.piece->GetPosition())
		{
			square.piece->CalculateLegalMoves();
			for (Position move : square.piece->GetLegalMoves())
			{
				if (square.piece->GetColor())
					m_WhiteControlledSquares.push_back(move);
				else
					m_BlackControlledSquares.push_back(move);

				if (square.piece->GetColor() == m_Turn)
					numOfMoves++;
			}
		}
	}
	std::cout << numOfMoves << std::endl;

	return numOfMoves;
}

bool Board::MakeMove(Piece* piece, Position from, Position to, bool overrideLegality/*=false*/)
{
	if (piece->Move(to, overrideLegality)) // move is possible
	{
		if (IsPieceCapturable(to, m_Turn) && (GetPiecePtr(to)->GetPosition() == to || GetPiecePtr(from)->GetPieceName() == "pawn") /* only pawns can En Passant*/)
		{
			Position pos = GetPiecePtr(to)->GetPosition();
			m_Board[Pos2Index(to)].piece.reset(); // capture piece
			SetPiece(pos, nullptr); // En passant hack crap
		}
		SetPiece(to, GetPiece(from)); // move piece (finally!)

		if (!overrideLegality)
		{
			m_Turn = (Color)(!(bool)m_Turn); // switch turn
			CalculateAllLegalMoves();
		}

		return true;
	}
	return false;
}

void Board::RenderBoard()
{
	LegalMoveSprite legalMoveSpriteInst(m_SquareSize, m_SquareSize * 0.75f, {0,0});
	for (Square& square : m_Board)
	{
		Engine::Renderer::SubmitObject(square.obj);
		if (square.piece && square.piece->GetPosition() == square.pos)
			square.piece->Render();
	}

	if (m_ActivatedSquare.file != -1)
	{
		if (GetPiecePtr(m_ActivatedSquare))
		{
			for (const Position& legalMove : GetPiecePtr(m_ActivatedSquare)->GetLegalMoves())
			{
				//LegalMoveSprite legalMoveSpriteInst(m_SquareSize, m_SquareSize * 0.75f, legalMove);
				legalMoveSpriteInst.SetPosition(legalMove);
				legalMoveSpriteInst.Render();
			}
		}
		GetPiecePtr(m_ActivatedSquare)->Render();
	}
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
{
	return GetPiecePtr(pos) && GetPiecePtr(pos)->GetPosition() == pos;
}

bool Board::IsPieceCapturable(Position pos, Color color)
{
	if (GetPiecePtr(pos))
		return GetPiecePtr(pos)->GetColor() != color;
	else
		return false;
}

Piece* Board::GetPiecePtr(Position pos) const
{
	return m_Board[Pos2Index(pos)].piece.get();
}

std::unique_ptr<Piece> Board::GetPiece(Position pos)
{
	return std::move(m_Board[Pos2Index(pos)].piece);
}

void Board::SetPiece(Position pos, std::unique_ptr<Piece> piece)
{
	m_Board[Pos2Index(pos)].piece = std::move(piece);
}

void Board::DeletePiece(Position pos)
{
	m_Board[Pos2Index(pos)].piece.reset();
}

bool Board::HandleMouseDown(Engine::MouseButtonPressedEvent& e)
{
	Position invalid = {-1, -1};
	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);
	Position squarePos = { (1 + mouseX) / m_SquareSize, (1 + mouseY) / m_SquareSize };
	
	if (m_ActivatedSquare != invalid)
	{
		if (!MakeMove(GetPiecePtr(m_ActivatedSquare), m_ActivatedSquare, squarePos))
		{
			float offset[2] = { 0, 0 };
			GetPiecePtr(m_ActivatedSquare)->GetRendererObject().shader.SetUniformVec(
				GetPiecePtr(m_ActivatedSquare)->GetRendererObject().shader.GetUniformLocation("renderOffset"),
				2,
				offset
			);
		}

		m_ActivatedSquare = invalid;

		return true;
	}
	else
	{
		if (!GetPiecePtr(squarePos))
		{
			m_ActivatedSquare = invalid;
			return true;
		}
		else if (GetPiecePtr(squarePos)->GetColor() != m_Turn)
		{
			m_ActivatedSquare = invalid;
			return true;
		}

		m_ActivatedSquare = squarePos;
		return true;
	}
}

bool Board::HandleMouseReleased(Engine::MouseButtonReleasedEvent& e)
{
	Position invalid = { -1, -1 };

	if (m_ActivatedSquare == invalid)
		return false;

	float mouseX, mouseY;
	e.GetMousePosition(mouseX, mouseY);
	Position squarePos = { (1 + mouseX) / m_SquareSize, (1 + mouseY) / m_SquareSize };
	
	if (squarePos == m_ActivatedSquare)
	{
		float offset[2] = { 0, 0 };
		GetPiecePtr(m_ActivatedSquare)->GetRendererObject().shader.SetUniformVec(
			GetPiecePtr(m_ActivatedSquare)->GetRendererObject().shader.GetUniformLocation("renderOffset"),
			2,
			offset
		);
		return false;
	}

	if (MakeMove(GetPiecePtr(m_ActivatedSquare), m_ActivatedSquare, squarePos))
	{}
	else
	{
		float offset[2] = { 0, 0 };
		GetPiecePtr(m_ActivatedSquare)->GetRendererObject().shader.SetUniformVec(
			GetPiecePtr(m_ActivatedSquare)->GetRendererObject().shader.GetUniformLocation("renderOffset"),
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

	const Engine::RendererObject& pieceOBJ = GetPiecePtr(m_ActivatedSquare)->GetRendererObject();

	float calculatedOffset[2] = {mouseX - pieceOBJ.position[0], mouseY - pieceOBJ.position[1]};
	
	pieceOBJ.shader.SetUniformVec(
		pieceOBJ.shader.GetUniformLocation("renderOffset"),
		2,
		calculatedOffset
	);
	return true;
}

int Board::Pos2Index(Position pos)
{
	return pos.rank * 8 + pos.file;
}

////////////////// BoardLayer ////////////////////////////////

BoardLayer::BoardLayer(Board* boardPtr)
	: m_BoardPtr(boardPtr)
{}

void BoardLayer::OnAttach()
{}

void BoardLayer::OnDetach()
{}

void BoardLayer::OnEvent(Engine::Event& e)
{
	Engine::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(std::bind(&Board::HandleMouseDown, m_BoardPtr, std::placeholders::_1));
	dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(std::bind(&Board::HandleMouseReleased, m_BoardPtr, std::placeholders::_1));
	dispatcher.Dispatch<Engine::MouseMovedEvent>(std::bind(&Board::HandleMouseMoved, m_BoardPtr, std::placeholders::_1));
}
