#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Engine/Events/KeyboardEvents.h"
#include "Engine/Events/MouseEvents.h"
#include "Engine/Layer.h"
#include "Engine/Renderer.h"
#include "Pieces/Piece.h"

#include "PromotionBoard.h"

struct Square {
	Position pos;
	Engine::RendererObject background;
	std::unique_ptr<Piece> piece;
};

struct Move {
	Position from;
	Position to;

	bool operator==(Move other) const {
		return from == other.from && to == other.to;
	}
};

class Board;

class BoardLayer : public Engine::Layer
{
public:
	explicit BoardLayer(Board *boardPtr);

	inline void OnAttach() override {}

	inline void OnDetach() override {}

	bool OnEvent(Engine::Event &e) override;

private:
	Board *m_BoardPtr;
};

class Board
{
	friend class PromotionBoard;

public: // construction
	Board(
		const char *vertShaderPath, const char *fragShaderPath,
		bool trackMoves = true
	);

	void ReadFen(std::string fen);

	void GeneratePieces(std::string fen, int i, Position &currentPos);

public: // rendering
	void RenderBoard();

	void FlipBoard(Square &square);

public: // calculating legal moves
	unsigned int CalculateAllLegalMoves();

	void RecalculateCheckLegalMoves(
		class King *king, Piece *checker, unsigned int &numMoves
	);

	void RecalculatePinnedPieceLegalMoves(unsigned &numMoves);

public: // En Passant
	// expects en passant piece to be in m_EnPassantPieceInst
	void StartEnPassanting(class Pawn *pawn, Position pos);

	void ResetEnPassantPiece();

	class EnPassantPiece *GetEnPassantPiece();

public: // moving pieces
	bool MakeMove(Move move);

	void UnMakeMove(Move move);

	void GameOver();

public: // handling events
	void ApplyOffset(float x, float y);

	bool HandleMouseDown(Engine::MouseButtonPressedEvent &e);

	bool HandleMouseReleased(Engine::MouseButtonReleasedEvent &e);

	bool HandleMouseMoved(Engine::MouseMovedEvent &e);

	bool HandleKeyPressed(Engine::KeyPressedEvent &e);

public: // utility functions
	inline bool IsSquareOccupied(Position pos) const {
		return pos.IsValid() && GetPiece(pos) &&
		       GetPiece(pos)->GetPieceName() != "en_passant";
	}

	inline bool IsPieceCapturable(Position pos, Color color) const {
		return pos.IsValid() && GetPiece(pos) &&
		       GetPiece(pos)->GetColor() != color;
	}

	inline bool IsInEnemyTerritory(Position pos, Color color) const {
		return (
			m_ControlledSquares[!color].find(pos) !=
			m_ControlledSquares[!color].end()
		);
	}

	inline Piece *GetPiece(Position pos) const {
		return pos.IsValid() ? m_Board[pos.ToIndex()].piece.get() : nullptr;
	}

	// Gives away ownership
	inline std::unique_ptr<Piece> GetFullPiecePtr(Position pos) {
		return std::move(m_Board[pos.ToIndex()].piece);
	}

	inline void SetPiece(Position pos, std::unique_ptr<Piece> piece) {
		m_Board[pos.ToIndex()].piece = std::move(piece);
	}

	inline void DeletePiece(Position pos) {
		m_Board[pos.ToIndex()].piece.reset();
	}

	inline Color GetTurn() { return m_Turn; }

	inline BoardLayer *GetBoardLayer() { return &m_Layer; }

private:
	BoardLayer m_Layer;

	Engine::RendererObject m_ShadowObj;

	float m_SquareSize;
	Square m_Board[64];
	Position m_ActivatedSquare;
	Color m_Turn;
	bool m_MouseReleased = true;
	std::unique_ptr<Piece> m_EnPassantPieceInst;
	Position *m_EnPassantPositionPtr;

	bool m_ShouldTrackMoves;
	std::vector<Move> m_MovesPlayed;
	std::set<Position> m_ControlledSquares[2] {};

public:
	std::unique_ptr<PromotionBoard> p_PromotionBoard = nullptr;

	Position p_PinnedPiecePos[2] {};

	Position p_KingPos[2] {};
};