/*
   Inkplate5_Maze_Generator example for Soldered Inkplate 5.
   For this example you will need only a USB-C cable and Inkplate 5.
   Select "Soldered Inkplate5" from Tools -> Board menu.
   Don't have "Soldered Inkplate5" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example renders a random maze every time!
   You can write on it with a whiteboard marker or a graphite pen to solve it.
   Just be sure not to use pernament markers!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   28 March 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Create object on Inkplate library and set library to work in monochorme mode
Inkplate display(INKPLATE_1BIT);

// Here we define one cell size
const int cellSize = 10;

// Calculate screen width and height
const int w = (E_INK_WIDTH - 10) / cellSize, h = (E_INK_HEIGHT - 10) / cellSize;
char maze[w * h];

// Move direction difference array
int dx[] = {-1, 0, 0, 1};
int dy[] = {0, -1, 1, 0};

void setup()
{
    // Init library (you should call this function ONLY ONCE)
    display.begin();

    // Generate and display the maze
    generateMaze(maze, w, h);
    showMaze(maze, w, h);
}

void loop()
{
    // Nothing ...
}

// Display the maze
void showMaze(const char *maze, int width, int height)
{
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (maze[x + y * width] == 1)
                for (int i = 0; i < 4; ++i)
                {
                    int xx = x + dx[i];
                    int yy = y + dy[i];
                    if (0 <= xx && xx < w && 0 <= yy && yy < h && maze[yy * width + xx] == 1)
                        display.drawLine(3 + x * cellSize + cellSize / 2, 3 + y * cellSize + cellSize / 2,
                                         3 + x * cellSize + cellSize / 2 + (dx[i] * cellSize / 2),
                                         3 + y * cellSize + cellSize / 2 + (dy[i] * cellSize / 2), BLACK);
                }

    display.display();
}

// Carve the maze starting at x, y.
void carveMaze(char *maze, int width, int height, int x, int y)
{
    int x1, y1;
    int x2, y2;
    int dx, dy;
    int dir, count;

    dir = random(4);
    count = 0;
    while (count < 4)
    {
        dx = 0;
        dy = 0;
        switch (dir)
        {
        case 0:
            dx = 1;
            break;
        case 1:
            dy = 1;
            break;
        case 2:
            dx = -1;
            break;
        default:
            dy = -1;
            break;
        }
        x1 = x + dx;
        y1 = y + dy;
        x2 = x1 + dx;
        y2 = y1 + dy;
        if (x2 > 0 && x2 < width && y2 > 0 && y2 < height && maze[y1 * width + x1] == 1 && maze[y2 * width + x2] == 1)
        {
            maze[y1 * width + x1] = 0;
            maze[y2 * width + x2] = 0;
            x = x2;
            y = y2;
            dir = random(4);
            count = 0;
        }
        else
        {
            dir = (dir + 1) % 4;
            count += 1;
        }
    }
}

// Generate maze in matrix maze with size width, height.
void generateMaze(char *maze, int width, int height)
{
    int x, y;

    // Initialize the maze.
    for (x = 0; x < width * height; x++)
    {
        maze[x] = 1;
    }
    maze[1 * width + 1] = 0;

    // Seed the random number generator.
    srand(time(0));

    // Carve the maze.
    for (y = 1; y < height; y += 2)
    {
        for (x = 1; x < width; x += 2)
        {
            carveMaze(maze, width, height, x, y);
        }
    }

    // Set up the entry and exit.
    maze[0 * width + 1] = 0;
    maze[(height - 1) * width + (width - 2)] = 0;
}
