/**
 **************************************************
 * @file        Inkplate4TEMPERA_Game_Of_Life.ino
 * @brief       Conway's Game of Life animation using 1-bit partial updates on
 *              Inkplate 4 TEMPERA.
 *
 * @details     This community example implements Conway's Game of Life on the
 *              Inkplate 4 TEMPERA e-paper display. The screen is divided into a
 *              grid of square cells (randomized cell size each run), and each
 *              iteration updates the grid according to the classic Life rules.
 *
 *              To make the animation smoother on e-paper, the sketch draws only
 *              the changed regions into the frame buffer and uses partialUpdate()
 *              most frames. A full refresh is performed every FULLREFRESH frames
 *              to reduce ghosting and maintain contrast - periodic full refresh
 *              is a best practice for long-running animations. If the simulation
 *              stagnates (too little change over time), the grid is re-randomized
 *              to keep the animation interesting.
 *
 *              Display mode is 1-bit (BW), since partial updates are supported
 *              only in BW mode. The simulation uses two in-RAM grids sized for
 *              the minimum cell size, so RAM usage increases with display
 *              resolution and the chosen cell size range.
 *
 *              Expected output: a continuous Game of Life animation using
 *              black/white cells, where new cells appear as filled black squares
 *              and older cells are drawn with a shrinking white interior to
 *              indicate age.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     115200 baud (optional; not required for operation)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 4 TEMPERA.
 * 3) The Game of Life simulation starts automatically after boot.
 * 4) Watch the evolving patterns; the sketch will occasionally re-randomize
 *    when activity drops.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered (community example by Claud9999)
 * @date        2020-09-24
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include <Inkplate.h>

Inkplate display(INKPLATE_1BIT);

#define FULLREFRESH 40

#define SCREEN_WIDTH  E_INK_WIDTH
#define SCREEN_HEIGHT E_INK_HEIGHT

#define MIN_CELLSZ 8
#define MAX_CELLSZ 40

// Initialize dimensions to the minimum size cell, maximum size arrays
int cell_size = 0, cols = 0, rows = 0, cell_maxage = 0, offset_x = 0, offset_y = 0;

char grid_a[(SCREEN_WIDTH / MIN_CELLSZ) * (SCREEN_HEIGHT / MIN_CELLSZ)],
    grid_b[(SCREEN_WIDTH / MIN_CELLSZ) * (SCREEN_HEIGHT / MIN_CELLSZ)];

char *grid_curr = grid_a, *grid_next = grid_b, *grid_tmp = NULL, cell_curr, cell_next;

int dx = 0, dy = 0, nx = 0, ny = 0, neighbors = 0, cell_delta = 0, frame_count = 0;

void randomize()
{
    cell_size = random(MIN_CELLSZ, MAX_CELLSZ + 1);

    // Compute the (rounded-down) number of rows and columns
    cols = SCREEN_WIDTH / cell_size;
    rows = SCREEN_HEIGHT / cell_size;

    cell_maxage = (cell_size / 2) - 1;
    // Compute the "extra" space not covered by this grid, offset by half
    offset_x = (SCREEN_WIDTH - cols * cell_size) / 2;
    offset_y = (SCREEN_HEIGHT - rows * cell_size) / 2;

    display.display();

    display.clearDisplay();

    // Compute a random density...
    int density = random(5, 15);

    // And for that density, populate the initial grid
    for (int j = 0; j < cols; j++)
    {
        for (int i = 0; i < rows; i++)
            grid_curr[j + i * cols] = random(density) == 0;
    }

    frame_count = 0;
}

void setup()
{
    Serial.begin(115200);
    display.begin();
    randomize();
}

void loop()
{
    cell_delta = 0;
    for (int j = 0; j < cols; j++)
    {
        for (int i = 0; i < rows; i++)
        {
            // Count neighboring cells
            neighbors = 0;
            for (dx = -1; dx < 2; dx++)
            {
                for (dy = -1; dy < 2; dy++)
                {
                    if (dx == 0 && dy == 0)
                        continue; // Skip "me"

                    nx = j + dx;
                    if (nx < 0)
                        nx = cols - 1;
                    else if (nx >= cols)
                        nx = 0;
                    ny = i + dy;
                    if (ny < 0)
                        ny = rows - 1;
                    else if (ny >= rows)
                        ny = 0;

                    if (grid_curr[nx + ny * cols])
                        neighbors++;
                }
            }

            cell_curr = grid_curr[j + i * cols];
            cell_next = 0;
            switch (neighbors)
            {
            case 2: // Alive with 2 neighbors remains alive
                if (!cell_curr)
                    break;
                // Else cell is alive drop through
            case 3: // 3 neighbors == alive
                cell_next = cell_curr + 1;
                if (cell_next > cell_maxage)
                    cell_next = cell_maxage;
            }

            if ((cell_next != 0 && cell_curr == 0) || (cell_next == 0 && cell_curr != 0))
                cell_delta++;

            if (cell_next)
            {
                // If this is a new cell, paint it black
                if (cell_next == 1)
                    display.fillRect(j * cell_size + offset_x, i * cell_size + offset_y, cell_size, cell_size, 1);
                // Otherwise paint the inside white depending on how old it is
                else
                    display.fillRect(j * cell_size + cell_size / 2 - cell_next + offset_x,
                                     i * cell_size + cell_size / 2 - cell_next + offset_y, cell_next * 2, cell_next * 2,
                                     0);
            }
            else if (cell_curr)
            {
                // Otherwise it's died, paint the whole cell white
                display.fillRect(j * cell_size + offset_x, i * cell_size + offset_y, cell_size, cell_size, 0);
            }

            grid_next[j + i * cols] = cell_next;
        }
    }
    // Swap which grid is current
    grid_tmp = grid_next;
    grid_next = grid_curr;
    grid_curr = grid_tmp;

    // The longer this goes, the more cells this has,
    // The more change is required or we reset
    if (cell_delta * cell_size < frame_count)
        randomize();
    else
        frame_count++;

    // Update whole screen after FULLREFRESH partials
    if (frame_count % FULLREFRESH == 0)
        display.display();
    else
        display.partialUpdate();
}
