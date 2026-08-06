/**
 **************************************************
 * @file        Inkplate6FLICK_Maze_Generator.ino
 * @brief       Generate and draw a random maze on Inkplate 6FLICK in 1-bit (BW)
 *              mode.
 *
 * @details     This example creates a random maze using a simple recursive
 *              backtracking-style carving routine on a grid and renders the
 *              resulting corridors as line segments on the e-paper display.
 *              The maze is generated once at boot and then shown as a static
 *              image.
 *
 *              The maze grid resolution is controlled by cellSize (smaller =
 *              more detail, slower drawing). Internally, the maze is stored as a
 *              1D array where each cell is either wall (1) or passage (0).
 *              During drawing, the sketch connects adjacent passage cells with
 *              short line segments to visualize the maze.
 *
 *              The display runs in 1-bit (BW) mode (INKPLATE_1BIT) and performs
 *              a full refresh (display()) after the maze is drawn; partial
 *              updates, touchscreen, WiFi and deep sleep are not used. The random
 *              seed is initialized at boot, so repeated resets may sometimes
 *              generate similar mazes depending on the platform time source. If
 *              you physically mark the e-paper to solve the maze, use only
 *              dry-erase or soft graphite and avoid permanent markers to prevent
 *              panel damage.
 *
 *              Expected output: a black line maze with a defined entry and exit;
 *              Serial has no mandatory output (it is initialized only).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Extra:      none
 * - Serial:     115200 baud (optional; not required for operation)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6FLICK"
 *    from Tools -> Board.
 * 2) Optionally adjust cellSize to change maze density.
 * 3) Upload the sketch to Inkplate 6FLICK.
 * 4) After boot, a new random maze is generated and displayed.
 * 5) To get a different maze, reset or power-cycle the board.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2024-03-15
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
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
