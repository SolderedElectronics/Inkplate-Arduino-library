/*
   Game_Of_Life_by_claud9999 example for e-radionica Inkplate 6
   For this example you will need only USB cable and Inkplate 6
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   Our first community created example, made by: https://github.com/claud9999
   To run it, jut upload the code and watch Conways game of life animation!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   24 September 2020 by e-radionica.com
*/


#include <Inkplate.h>

Inkplate display(INKPLATE_1BIT);

#define FULLREFRESH 40

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

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

    display.clean();
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

            if (cell_next != 0 && cell_curr == 0 || cell_next == 0 && cell_curr != 0)
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
