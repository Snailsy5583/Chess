#pragma once

#include "Engine/Renderer.h"

#include <string>
#include <vector>

class Board;

struct Position
{
	int file=-1;
	int rank=-1;

	bool operator==(const Position& other) const
	{ return other.file == file && other.rank == rank; }
	bool operator!=(const Position& other) const
	{ return other.file != file || other.rank != rank; }

	Position operator+(const Position& other) const
	{ return { file + other.file, rank + other.rank }; }
	Position operator-(const Position& other) const
	{ return { file - other.file, rank - other.rank }; }
	Position operator*(const Position& other) const
	{ return { file * other.file, rank * other.rank }; }
	Position operator*(const int& other) const
	{ return { file * other, rank * other }; }
	Position operator/(const Position& other) const
	{ return { file / other.file, rank / other.rank }; }
	Position operator/(const int& other) const
	{ return { file / other, rank / other }; }

	Position& operator+=(const Position& other)
	{
		file += other.file;
		rank += other.rank;
		return *this;
	}

	Position& operator-=(const Position& other)
	{
		file -= other.file;
		rank -= other.rank;
		return *this;
	}
};

enum Color
{
	Black=0, White=1
};

class Piece
{
public:
	Piece()=default;
	Piece(Color color, Position pos, float squareSize, std::string pieceName, Board* board);
	virtual ~Piece();

	virtual bool Move(Position pos, bool overrideLegality=false);

	virtual void Render();

	virtual const std::vector<Position>& CalculateLegalMoves() = 0;
	inline const std::vector<Position>& GetLegalMoves() const { return m_LegalMoves; }

	inline Color GetColor() const { return m_Color; }
	inline Position GetPosition() const { return m_Position; }
	inline bool GetIsGamePiece() const { return m_IsGamePiece; }
	inline bool GetIsVirgin() const { return m_IsVirgin; }

	inline const Engine::RendererObject& GetRendererObject() const { return m_Object; }
	inline const char* GetPieceName() const { return m_PieceName.c_str(); }

protected:
	Board* m_OwnerBoard;

	Engine::RendererObject m_Object;
	std::string m_TexturePath;
	std::string m_PieceName;
	
	Position m_Position;
	Color m_Color;

	bool m_IsSlidingPiece;
	bool m_IsGamePiece;
	std::vector<Position> m_MovePatterns;
	std::vector<Position> m_LegalMoves;
	bool m_IsVirgin;

	float m_SquareSize;
};