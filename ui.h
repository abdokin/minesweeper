#ifndef UI_H
#define UI_H
#include <raylib.h>
#include "board.h"
typedef struct
{
    Rectangle rect;
    Color color;
    const char *text;
    int *valueToChange;
} Button;

void draw_button(Button button, Vector2 mousePoint, int *value)
{
    // Add padding to the button rectangle
    float paddingX = 10.0f;
    float paddingY = 5.0f;

    Rectangle paddedRect = {
        .x = button.rect.x - paddingX,
        .y = button.rect.y - paddingY,
        .width = button.rect.width + 2 * paddingX,
        .height = button.rect.height + 2 * paddingY};

    // Draw button background
    DrawRectangleRec(paddedRect, button.color);

    // Check if mouse is over the padded button
    if (CheckCollisionPointRec(mousePoint, paddedRect))
    {
        // Draw button outline if mouse is over
        DrawRectangleLinesEx(paddedRect, 2, BLACK);

        static double lastClickTime = 0.0;
        double currentTime = GetTime();

        // Check if left mouse button is pressed and timer is elapsed
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastClickTime) > 0.5)
        {
            // Change the value when the button is clicked and reset the timer
            (*button.valueToChange) = !(*button.valueToChange);
            lastClickTime = currentTime;
            *value = 1;
        }
    }

    // Draw button text with padding
    DrawText(button.text, button.rect.x + button.rect.width / 2 - MeasureText(button.text, 20) / 2,
             button.rect.y + button.rect.height / 2 - 20 / 2, 20, WHITE);
}

void draw_text_message(Board *board, char *message, Vector2 screen, Color c, int *value)
{
    // Calculate the positions of the texts
    const char *button_message = "Start Over";
    Vector2 messageTextPos = {screen.x / 2 - MeasureText(message, 40) / 2, screen.y / 2 - 40};
    Vector2 startOverTextPos = {screen.x / 2 - MeasureText(button_message, 20) / 2, screen.y / 2 + 10};

    // Draw the background for the message and button
    float boxWidth = MeasureText(message, 40) + 40; // Adjust the width based on your text
    float boxHeight = 120;                          // Adjust the height based on your text size

    DrawRectangle(screen.x / 2 - boxWidth / 2, screen.y / 2 - boxHeight / 2, boxWidth, boxHeight, LIGHTGRAY);
    DrawRectangleLines(screen.x / 2 - boxWidth / 2, screen.y / 2 - boxHeight / 2, boxWidth, boxHeight, BLACK);

    // Draw the message text
    DrawText(message, messageTextPos.x, messageTextPos.y, 40, c);

    // Example button
    Button startOverButton = {
        .rect = {screen.x / 2 - MeasureText(button_message, 20) / 2, screen.y / 2 + 10, MeasureText(button_message, 20), 20},
        .color = BLUE,
        .text = button_message,
        .valueToChange = value,
    };

    int isButtonPressed = 0;
    // Draw the button using the draw_button function
    draw_button(startOverButton, GetMousePosition(), &isButtonPressed);

    // Check if the user clicks to start over using the button
    if (isButtonPressed && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        initialize_board(board);
        *value = 0;
    }
}

#endif // UI_H