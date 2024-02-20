#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "ui.h"
#include <raylib.h>

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    time_t t;
    srand((unsigned)time(&t));

    const Vector2 screen = {
        .x = 800,
        .y = 800,
    };

    Board board;
    initialize_board(&board);
    Vector2 mousePoint = {0.0f, 0.0f};
    int gameOver = 0;
    int playerWon = 0;
    int gameStart = 1;

    InitWindow(screen.x, screen.y, "Minesweeper");

    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------

        mousePoint = GetMousePosition();
        // Check if the game is over or the player has won
        if (!gameOver && !playerWon && !gameStart)
        {
            reveal_on_hover(&board, mousePoint);
            reveal_on_click(&board, mousePoint, &gameOver);
        }
        playerWon = isPlayerWin(&board);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        draw_board(&board);

        if (gameOver || playerWon || gameStart)
        {
            if (gameStart)
            {
                draw_text_message(&board, "Start New Level", screen, GRAY, &gameStart);
            }
            else
                draw_text_message(&board, gameOver ? "Game Over" : "You Won", screen, gameOver ? RED : DARKGREEN, gameOver ? &gameOver : &playerWon);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}
