#ifndef BOARD_H
#define BOARD_H
#include <raylib.h>
#include <time.h>
#include <stdlib.h>
#define Clamp(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))
#define MAX_CELLS_COUNT 40
#define CELL_SIZE 40
#define NUM_MINES 40

typedef struct
{
    Rectangle rect;
    int shown;
    int hover;
    int state;
    int marked;
} Cell;
typedef struct
{
    int row, col;
    Cell data[100][100];
} Board;
void generate_board(Board *b)
{
    for (int i = 0; i < b->row; i++)
    {
        for (int j = 0; j < b->col; j++)
        {
            Rectangle cellRect = {
                j * CELL_SIZE,
                i * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE};

            b->data[i][j].state = 0;
            b->data[i][j].marked = 0;
            b->data[i][j].shown = 0;
            b->data[i][j].rect = cellRect;
        }
    }
}

void calcule_neighbors(Board *b)
{
    int i, j;
    for (i = 0; i < b->row; i++)
    {
        for (j = 0; j < b->col; j++)
        {
            if (b->data[i][j].state == -1)
            {
                if (i - 1 >= 0 && j - 1 >= 0)
                    b->data[i - 1][j - 1].state++;
                if (i - 1 >= 0)
                    b->data[i - 1][j].state++;
                if (i - 1 >= 0 && j + 1 < b->col)
                    b->data[i - 1][j + 1].state++;
                if (j - 1 >= 0)
                    b->data[i][j - 1].state++;
                if (j + 1 < b->col)
                    b->data[i][j + 1].state++;
                if (i + 1 < b->row && j - 1 >= 0)
                    b->data[i + 1][j - 1].state++;
                if (i + 1 < b->row)
                    b->data[i + 1][j].state++;
                if (i + 1 < b->row && j + 1 < b->col)
                    b->data[i + 1][j + 1].state++;
            }
        }
    }
}
void fill_numbers(Board *b)
{
    for (int i = 0; i < b->row; i++)
    {
        for (int j = 0; j < b->col; j++)
        {
            if (b->data[i][j].state != -1)
            {
                // Count neighboring mines
                int count = 0;
                for (int ni = -1; ni <= 1; ni++)
                {
                    for (int nj = -1; nj <= 1; nj++)
                    {
                        int newRow = i + ni;
                        int newCol = j + nj;

                        // Check if the neighbor is within bounds and has a mine
                        if (newRow >= 0 && newRow < b->row && newCol >= 0 && newCol < b->col &&
                            b->data[newRow][newCol].state == -1)
                        {
                            count++;
                        }
                    }
                }

                // Assign the count to the cell
                b->data[i][j].state = count;
            }
        }
    }
}

// Function to fill the Minesweeper board with mines
void fill_mines(Board *b)
{
    // Create an array to store all possible cell positions
    Rectangle allPositions[b->row * b->col];
    int positionCount = 0;

    // Fill the array with all possible positions
    for (int i = 0; i < b->row; i++)
    {
        for (int j = 0; j < b->col; j++)
        {
            allPositions[positionCount++] = b->data[i][j].rect;
        }
    }

    // Shuffle the array using Fisher-Yates algorithm
    for (int i = positionCount - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Rectangle temp = allPositions[i];
        allPositions[i] = allPositions[j];
        allPositions[j] = temp;
    }

    // Place mines using the first NUM_MINES positions
    for (int i = 0; i < NUM_MINES; i++)
    {
        // Calculate row and column indices with additional randomness
        int row = (int)(allPositions[i].y / CELL_SIZE) + (rand() % 3 - 1); // Random offset between -1 and 1
        int col = (int)(allPositions[i].x / CELL_SIZE) + (rand() % 3 - 1);

        // Ensure the indices are within valid bounds
        row = Clamp(row, 0, b->row - 1);
        col = Clamp(col, 0, b->col - 1);

        b->data[row][col].state = -1;
    }
}

void draw_board(Board *b)
{

    for (int i = 0; i < b->row; i++)
    {
        for (int j = 0; j < b->col; j++)
        {
            Rectangle cellRect = b->data[i][j].rect;

            if (b->data[i][j].shown == 1)
            {

                if (b->data[i][j].state == -1)
                {
                    DrawRectangleRec(cellRect, RED);
                    DrawText("*", cellRect.x + CELL_SIZE / 2 - MeasureText("*", 20) / 2, cellRect.y + CELL_SIZE / 2 - 20 / 2, 20, BLACK);
                }
                else if (b->data[i][j].state == 0)
                {
                    DrawText(" ", cellRect.x + CELL_SIZE / 2 - MeasureText(" ", 20) / 2, cellRect.y + CELL_SIZE / 2 - 20 / 2, 20, BLACK);
                }
                else
                {
                    DrawRectangleRec(cellRect, GRAY);
                    DrawText(TextFormat("%d", b->data[i][j].state), cellRect.x + CELL_SIZE / 2 - MeasureText(TextFormat("%d", b->data[i][j].state), 20) / 2, cellRect.y + CELL_SIZE / 2 - 20 / 2, 20, BLACK);
                }
            }
            else if (b->data[i][j].marked == 1)
            {
                DrawRectangleRec(cellRect, BLUE);
                DrawText("!!", cellRect.x + CELL_SIZE / 2 - MeasureText("!!", 20) / 2, cellRect.y + CELL_SIZE / 2 - 20 / 2, 20, BLACK);
            }
            else
            {
                DrawText("#", cellRect.x + CELL_SIZE / 2 - MeasureText("#", 20) / 2, cellRect.y + CELL_SIZE / 2 - 20 / 2, 20, BLACK);
            }
            DrawRectangleLinesEx(cellRect, 1, BLACK);
        }
    }
}
// Function to reveal fields in Minesweeper
void reveal_field(Board *b, int row, int col)
{
    // Base case: check if the cell is outside the board or already shown
    if (row < 0 || row >= b->row || col < 0 || col >= b->col || b->data[row][col].shown == 1)
    {
        return;
    }

    // Mark the current cell as shown
    if (b->data[row][col].state != -1)
        b->data[row][col].shown = 1;

    // Check if the current cell is empty
    if (b->data[row][col].state == 0)
    {
        // Recursively reveal neighboring cells
        reveal_field(b, row - 1, col - 1);
        reveal_field(b, row - 1, col);
        reveal_field(b, row - 1, col + 1);
        reveal_field(b, row, col - 1);
        reveal_field(b, row, col + 1);
        reveal_field(b, row + 1, col - 1);
        reveal_field(b, row + 1, col);
        reveal_field(b, row + 1, col + 1);
    }
}
void reveal_on_click(Board *b, Vector2 mousePoint, int *gameOver)
{

    // Calculate the cell indices based on the mouse position
    int col = (int)((mousePoint.x - b->data[0][0].rect.x) / CELL_SIZE);
    int row = (int)((mousePoint.y - b->data[0][0].rect.y) / CELL_SIZE);
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {

        // Check if the calculated indices are within bounds
        if (row >= 0 && row < b->row && col >= 0 && col < b->col)
        {

            // If the clicked cell is not empty, simply reveal it
            b->data[row][col].marked = !b->data[row][col].marked;
        }
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        // Check if the calculated indices are within bounds
        if (row >= 0 && row < b->row && col >= 0 && col < b->col)
        {
            // Check if the clicked cell has an empty value
            if (b->data[row][col].state == 0)
            {
                // Reveal the clicked cell and its neighbors
                reveal_field(b, row, col);
            }
            if (b->data[row][col].state == -1)
            {
                *gameOver = 1;
                b->data[row][col].shown = 1;
            }
            else
            {
                // If the clicked cell is not empty, simply reveal it
                b->data[row][col].shown = 1;
            }
        }
    }
}

void reveal_on_hover(Board *b, Vector2 mousePoint)
{
    // Calculate the cell indices based on the mouse position
    int col = (int)((mousePoint.x - b->data[0][0].rect.x) / CELL_SIZE);
    int row = (int)((mousePoint.y - b->data[0][0].rect.y) / CELL_SIZE);

    // Check if the calculated indices are within bounds
    if (row >= 0 && row < b->row && col >= 0 && col < b->col)
    {
        // Calculate the rectangle of the hovered cell
        Rectangle cellRect = {
            b->data[row][col].rect.x,
            b->data[row][col].rect.y,
            CELL_SIZE,
            CELL_SIZE};

        // Draw rectangle lines around the hovered cell
        DrawRectangleLinesEx(cellRect, 2, YELLOW);
    }
}

void initialize_board(Board *board)
{
    board->row = 20;
    board->col = 20;
    generate_board(board);
    fill_mines(board);
    // fill_numbers(&board);
    calcule_neighbors(board);
}
int isPlayerWin(Board *b)
{
    for (int i = 0; i < b->row; i++)
    {
        for (int j = 0; j < b->col; j++)
        {
            // If there is an unrevealed cell that is not a mine, the player has not won
            if (b->data[i][j].state != -1 && !b->data[i][j].shown)
            {
                return 0;
            }
        }
    }

    return 1;
}


#endif