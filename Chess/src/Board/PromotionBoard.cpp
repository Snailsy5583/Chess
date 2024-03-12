//
// Created by r6awe on 2/29/2024.
//

#include "PromotionBoard.h"

#include "Board.h"
#include "Board/Pieces/SlidingPieces.h"
#include "Board/Pieces/SpecialPieces.h"
#include "Application.h"

PromotionBoard::PromotionBoard(Position position, Board *board, Color color,
                               const char* vertShaderPath,
                               const char* fragShaderPath)
    :   m_Layer(this), m_Board(board), m_Origin(position), m_Color(color),
        m_PromotionBoard(5)
{
    float squareSize = m_Board->m_SquareSize;
    for (int i=0; i<5; i++) {
        Square& square = m_PromotionBoard[i];

        square.pos = {position.file, (
            m_Color == Color::White ? 7-i : i
        )};
        std::cout << square.pos.ToString() << std::endl;
        float pos[3] = {
            (-1 + squareSize / 2) + ((float) square.pos.file * squareSize),
            (-1 + squareSize / 2) + ((float) square.pos.rank * squareSize),
            0
        };

        square.obj = Engine::Renderer::GenQuad(pos, squareSize,
                                                   vertShaderPath, fragShaderPath);

        square.obj.shader.SetUniform(
            square.obj.shader.GetUniformLocation("isWhite"),
            ((square.pos.file + square.pos.rank) % 2));


        square.piece = nullptr;
        switch (i) {
            case 1:
                SetPiece(
                    square.pos,
                    std::make_unique<Queen>(
                        color,
                        square.pos,
                        squareSize,
                        board
                    )
                );
                break;
            case 2:
                SetPiece(
                    square.pos,
                    std::make_unique<Rook>(
                        color,
                        square.pos,
                        squareSize,
                        board
                    )
                );
                break;
            case 3:
                SetPiece(
                    square.pos,
                    std::make_unique<Bishop>(
                        color,
                        square.pos,
                        squareSize,
                        board
                    )
                );
                break;
            case 4:
                SetPiece(
                    square.pos,
                    std::make_unique<Knight>(
                        color,
                        square.pos,
                        squareSize,
                        board
                    )
                );
                break;
            default:
                SetPiece(
                    square.pos,
                    std::make_unique<Pawn>(
                        color,
                        square.pos,
                        squareSize,
                        board
                    )
                );
                break;
        }
    }
}

PromotionBoard::~PromotionBoard() = default;

void PromotionBoard::RenderBoard() {
    for (Square &square : m_PromotionBoard) {
        // Render every piece except for fake pieces
        Engine::Renderer::SubmitObject(square.obj);

        if (square.piece && square.piece->GetPieceName() != "en_passant")
            square.piece->Render();
    }
}

bool PromotionBoard::HandleMouseReleased(Engine::MouseButtonReleasedEvent &e) {
    float mouseX, mouseY;
    e.GetMousePosition(mouseX, mouseY);

    Position squarePos = {static_cast<int>((1 + mouseX)/m_Board->m_SquareSize),
                          static_cast<int>((1 + mouseY)/m_Board->m_SquareSize)};

    // square that was clicked on
    Square* chosenSquare = GetSquare(squarePos, true);

    if (!chosenSquare)
        return true;    // chosen square does not exist but we don't want the
                        // board to be handling events

    Application::GetLayerStack()->PopFront();
    dynamic_cast<Pawn*>(m_Board->GetPiece(m_Origin))->Promote(
        std::move(chosenSquare->piece)
    );

	m_Board->GetPromotionBoard().reset(); // delete this;

    return true; // event was handled
}

void PromotionBoard::SetPiece(Position pos, std::unique_ptr<Piece> piece) {
    Square* square = GetSquare(pos);
    square->piece = std::move(piece);
}

Square* PromotionBoard::GetSquare(Position pos, bool ignore0/*=false*/) {
    if (pos.file != m_Origin.file)
        return nullptr;
    int index = (m_Color == Color::White ? 7-pos.rank : pos.rank);
    if (index > 4 || index < (ignore0 ? 1 : 0))
        return nullptr;

    return &m_PromotionBoard[index];
}

////////////////// PromotionBoardLayer ////////////////////////////////

PromotionBoardLayer::PromotionBoardLayer(PromotionBoard *boardPtr)
    : m_BoardPtr(boardPtr) {}

void PromotionBoardLayer::OnAttach() {}

void PromotionBoardLayer::OnDetach() {}

bool PromotionBoardLayer::OnEvent(Engine::Event &e) {
    Engine::EventDispatcher dispatcher(e);

    if (dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(
        [](const Engine::MouseButtonPressedEvent&) ->bool { return true; }
    )) return true;
    if (dispatcher.Dispatch<Engine::MouseMovedEvent>(
        [](const Engine::MouseMovedEvent&) ->bool { return true; }
    )) return true;
    if (dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(
        std::bind(&PromotionBoard::HandleMouseReleased, m_BoardPtr,std::placeholders::_1)
    )) return true;

    return false; // if it hasn't returned true
}
