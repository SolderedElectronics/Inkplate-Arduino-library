/**
 **************************************************
 * @file        Inkplate13SPECTRA_Partial_Update.ino
 * @brief       Partial screen update example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Demonstrates partial screen updates on the Inkplate 13SPECTRA.
 *              It draws a grid of 100x100 pixel coloured squares covering the
 *              entire screen, then continuously picks a random square and
 *              updates only that square with a new colour using
 *              displayPartial(), leaving the rest of the screen untouched.
 *
 *              displayPartial(x, y, w, h) accepts coordinates in the same user
 *              space as all drawing functions (rotation = 1: 1600 px wide,
 *              1200 px tall).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) The grid of coloured squares is drawn, then single squares keep changing
 *    colour via partial updates.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-03-24
 **************************************************/

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"

Inkplate display;

// Display dimensions at rotation=1
#define DISPLAY_W 1600
#define DISPLAY_H 1200

// Square size and grid dimensions
#define SQUARE_SIZE 200
#define GRID_COLS   (DISPLAY_W / SQUARE_SIZE) // 8
#define GRID_ROWS   (DISPLAY_H / SQUARE_SIZE) // 6

// Available colours (indices 0-5 map to: black, white, yellow, red, blue, green)
static const uint8_t COLORS[] = {0, 1, 2, 3, 4, 5};
#define COLOR_COUNT 6

// Tracks the current colour of every square so we can pick a different one
uint8_t squareColor[GRID_COLS][GRID_ROWS];

void setup()
{
    Serial.begin(115200);

    display.begin();
    display.clearDisplay();

    // Fill each square with an initial colour, cycling through the palette
    for (int col = 0; col < GRID_COLS; col++)
    {
        for (int row = 0; row < GRID_ROWS; row++)
        {
            uint8_t color = COLORS[(col + row) % COLOR_COUNT];
            squareColor[col][row] = color;
            display.fillRect(col * SQUARE_SIZE, row * SQUARE_SIZE,
                             SQUARE_SIZE, SQUARE_SIZE, color);
        }
    }

    display.display();

    Serial.println("Initial grid drawn. Waiting 5 seconds before partial updates begin.");
    delay(5000);
}

void loop()
{
    // Pick a random square
    int col   = random(GRID_COLS);
    int row   = random(GRID_ROWS);

    // Pick a different colour than the current one
    uint8_t current = squareColor[col][row];
    uint8_t newColor;
    do
    {
        newColor = COLORS[random(COLOR_COUNT)];
    } while (newColor == current);

    squareColor[col][row] = newColor;

    // Update the framebuffer for just this square
    display.fillRect(col * SQUARE_SIZE, row * SQUARE_SIZE,
                     SQUARE_SIZE, SQUARE_SIZE, newColor);

    // Refresh only the changed square; pass true to keep panel on for next call.
    display.displayPartial(col * SQUARE_SIZE, row * SQUARE_SIZE,
                           SQUARE_SIZE, SQUARE_SIZE, true);

    Serial.print("Updated square (");
    Serial.print(col);
    Serial.print(", ");
    Serial.print(row);
    Serial.print(") to colour ");
    Serial.println(newColor);

    // 3 second delay between refreshes
    delay(3000);
}
