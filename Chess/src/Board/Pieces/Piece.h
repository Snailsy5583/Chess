#pragma once

#include "Engine/Renderer.h"

#include <sstream>
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
	bool operator<(const Position& other) const
	{ return ToIndex() < other.ToIndex(); }
	bool operator>(const Position & other) const
	{ return ToIndex() > other.ToIndex(); }

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

	std::string ToString() const 
	{
		std::stringstream ss;
		ss << "(" << file << ", " << rank << ")";
		return ss.str();
	}

	int ToIndex() const { return rank * 8 + file; }
};

enum Color
{
	Black=0, White=1
};

class Piece
{
public:
	Piece(Color color, Position pos, float squareSize, const char* pieceName, Board* board, bool loadImage=true);
	virtual ~Piece();

	virtual bool Move(Position pos, bool overrideLegality=false);

	virtual void Render();

	virtual void CalculateLegalMoves() = 0;
	inline const std::vector<Position>& GetLegalMoves() const { return m_LegalMoves; }
	inline const std::vector<Position>& GetControlledSquares() const { return m_ControlledSquares; }

	inline Color GetColor() const { return m_Color; }
	virtual inline Position GetPosition() const { return m_Position; }
	inline bool GetIsGamePiece() const { return m_IsGamePiece; }
	inline bool GetIsVirgin() const { return m_IsVirgin; }

	inline const Engine::RendererObject& GetRendererObject() const { return m_Object; }
	inline const char* GetPieceName() const { return m_PieceName; }

protected:
	Board* m_OwnerBoard;

	Engine::RendererObject m_Object;
	std::string m_TexturePath;
	const char* m_PieceName;
	
	Position m_Position;
	Color m_Color;

	bool m_IsSlidingPiece;
	bool m_IsGamePiece;
	std::vector<Position> m_MovePatterns;
	std::vector<Position> m_LegalMoves;
	std::vector<Position> m_ControlledSquares;
	bool m_IsVirgin;

	float m_SquareSize;
};