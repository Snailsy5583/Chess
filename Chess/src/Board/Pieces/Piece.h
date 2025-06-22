#pragma once

#include "Engine/Renderer.h"

#include <memory>
#include <sstream>
#include <stack>
#include <vector>

class Board;

struct Position {
	int file = -1;
	int rank = -1;

	bool operator==(const Position &other) const {
		return other.file == file && other.rank == rank;
	}

	bool operator!=(const Position &other) const {
		return other.file != file || other.rank != rank;
	}

	bool operator<(const Position &other) const {
		return ToIndex() < other.ToIndex();
	}

	bool operator>(const Position &other) const {
		return ToIndex() > other.ToIndex();
	}

	Position operator+(const Position &other) const {
		return {file + other.file, rank + other.rank};
	}

	Position operator-(const Position &other) const {
		return {file - other.file, rank - other.rank};
	}

	Position operator-() const { return {0 - file, 0 - rank}; }

	Position operator*(const Position &other) const {
		return {file * other.file, rank * other.rank};
	}

	Position operator*(const int &other) const {
		return {file * other, rank * other};
	}

	Position operator/(const Position &other) const {
		return {file / other.file, rank / other.rank};
	}

	Position operator/(const int &other) const {
		return {file / other, rank / other};
	}

	Position &operator+=(const Position &other) {
		file += other.file;
		rank += other.rank;
		return *this;
	}

	Position &operator-=(const Position &other) {
		file -= other.file;
		rank -= other.rank;
		return *this;
	}

	std::string ToString() const {
		std::stringstream ss;
		ss << (char) ('a' + file) << rank + 1;
		return ss.str();
	}

	int ToIndex() const { return rank * 8 + file; }

	bool IsValid() const {
		return file >= 0 && file < 8 && rank >= 0 && rank < 8;
	}

	Position Normalized() const {
		auto sign = [](int x) -> int { return (x > 0) - (x < 0); };
		return {sign(file), sign(rank)};
	}
};

enum Color { Black = 0, White = 1 };

class Piece
{
public:
	Piece(
		Color color, Position pos, float squareSize, const char *pieceName,
		Board *board, bool loadImage = true
	);

	virtual ~Piece();

	virtual bool Move(Position to);

	virtual void UndoMove(Position from);

	virtual void Pin(Position dir) {
		m_IsPinned = true;
		m_PinnedDirection = dir;
	}

	virtual void UnPin() {
		m_IsPinned = false;
		m_PinnedDirection = {0, 0};
	}

	virtual void Render();

	virtual void CalculateLegalMoves() = 0;

	virtual inline void ClearLegalMoves() { m_LegalMoves.clear(); }

	inline const std::vector<Position> &GetLegalMoves() const {
		return m_LegalMoves;
	}

	inline void RemoveLegalMove(std::vector<Position>::const_iterator i) {
		m_LegalMoves.erase(i);
	}

	bool IsLegalMove(Position sq) const;

	bool IsControlledSquare(Position sq) const;

	inline std::vector<Position> &GetControlledSquares() {
		return m_ControlledSquares;
	}

	inline Color GetColor() const { return m_Color; }

	virtual inline Position GetPosition() const { return m_Position; }

	inline bool GetIsVirgin() const { return m_IsVirgin; }

	inline Engine::RendererObject &GetRendererObject() { return m_Object; }

	inline std::string GetPieceName() const { return std::string(m_PieceName); }

protected:
	Board *m_OwnerBoard;

	Engine::RendererObject m_Object;
	std::string m_TexturePath = "Assets/Textures/Pieces/";
	const char *m_PieceName = "";

	Position m_Position;
	Color m_Color;

	Position m_StartingPosition {-1, -1};
	bool m_IsVirgin = true;

	bool m_IsPinned = false;
	Position m_PinnedDirection;

	std::vector<Position> m_MovePatterns;
	std::vector<Position> m_LegalMoves;
	std::vector<Position> m_ControlledSquares;

	float m_SquareSize;
};