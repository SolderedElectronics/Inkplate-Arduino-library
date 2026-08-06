/**
 **************************************************
 * @file        Inkplate6_Maze_Generator.ino
 * @brief       Generates a random maze and draws it on Inkplate 6 in 1-bit
 *              black/white mode.
 *
 * @details     This example demonstrates simple procedural content generation
 *              on Inkplate 6 by creating a new random maze at startup and
 *              rendering it to the e-paper display. The maze is generated on a
 *              grid of cells (cellSize pixels per cell) using a carving
 *              algorithm: the grid starts filled, then corridors are carved by
 *              stepping in random directions and clearing cells when a valid
 *              two-step move is possible.
 *
 *              After generation, the sketch renders the maze by drawing short
 *              line segments between adjacent open cells, forming connected
 *              passages. An entry and exit are opened at the top and bottom of
 *              the maze. The result is displayed using a full refresh.
 *
 *              The maze is intended to be solved manually on the physical
 *              screen (e.g., with a whiteboard marker or soft pencil). Avoid
 *              permanent markers to prevent staining the panel surface.
 *
 *              Display mode is 1-bit BW (INKPLATE_1BIT); partial updates are
 *              possible in BW mode in general, but this example performs a full
 *              draw followed by a full refresh. Maze randomness is seeded at
 *              startup, so each reset produces a different result (depending on
 *              available time source/seed behavior). Larger cellSize values
 *              render faster but produce a less detailed maze; smaller values
 *              increase detail at the cost of more drawing operations. Expected
 *              output is a randomly generated maze drawn in black on a white
 *              background, with an entry at the top edge and an exit near the
 *              bottom edge.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      none
 * - Serial:     115200 baud (optional; no mandatory output)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Optionally adjust cellSize to change maze density and rendering speed.
 * 3) Upload the sketch to Inkplate 6.
 * 4) On boot, a new random maze is generated and shown on the display.
 * 5) Solve the maze directly on the screen using an erasable whiteboard marker
 *    or a non-permanent pencil/graphite tool, then wipe clean when finished.
 * 6) Reset the board to generate a new maze.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

#include "Inkplate.h"

// Initialise Inkplate object
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
    // Initialise Inkplate
    Serial.begin(115200);
    display.begin();

    pinMode(12, INPUT);
    pinMode(13, INPUT);

    // Generate and display the maze
    generateMaze(maze, w, h);
    showMaze(maze, w, h);
}

void loop()
{
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
