/** Example "Game of Life" program for the InkPlate 6 by Chris Knight
 *  https://github.com/claud9999
 */

#include <Inkplate.h>

Inkplate display(INKPLATE_1BIT);

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MIN_CELLSZ 8
#define MAX_CELLSZ 40

// initialize dimensions to the minimum size cell, maximum size arrays
int cell_size = 0, cols = 0, rows = 0,
    cell_maxage = 0, offset_x = 0, offset_y = 0;
  
char grid_a[(SCREEN_WIDTH / MIN_CELLSZ) * (SCREEN_HEIGHT / MIN_CELLSZ)],
  grid_b[(SCREEN_WIDTH / MIN_CELLSZ) * (SCREEN_HEIGHT / MIN_CELLSZ)];
char *grid_curr = grid_a, *grid_next = grid_b, *grid_tmp = NULL,
    cell_curr, cell_next;
int x = 0, y = 0, dx = 0, dy = 0, nx = 0, ny = 0,
    neighbors = 0, cell_delta = 0, frame_count = 0;

void randomize() {
  cell_size = random(MIN_CELLSZ, MAX_CELLSZ + 1);

  // compute the (rounded-down) number of rows and columns
  cols = SCREEN_WIDTH / cell_size;
  rows = SCREEN_HEIGHT / cell_size;

  cell_maxage = (cell_size / 2) - 1;
  // compute the "extra" space not covered by this grid, offset by half
  offset_x = (SCREEN_WIDTH - cols * cell_size) / 2;
  offset_y = (SCREEN_HEIGHT - rows * cell_size) / 2;

  display.clean();
  display.display();
  
  display.clearDisplay();

  // compute a random density...
  int density = random(5,15);

  // and for that density, populate the initial grid
  for(x = 0; x < cols; x++) {
    for (y = 0; y < rows; y++) grid_curr[x + y * cols] = random(density) == 0;
  }

  frame_count = 0;
}

void setup() {
  Serial.begin(115200);
  display.begin();
  randomize();
}

void loop() {
  cell_delta = 0;
  for(x = 0; x < cols; x++) {
    for(y = 0; y < rows; y++) {
      // count neighboring cells
      neighbors = 0;
      for(dx = -1; dx < 2; dx++) {
        for(dy = -1; dy < 2; dy++) {
          if(dx == 0 && dy == 0) continue; // skip "me"

          nx = x + dx; if(nx < 0) nx = cols - 1; else if(nx >= cols) nx = 0;
          ny = y + dy; if(ny < 0) ny = rows - 1; else if(ny >= rows) ny = 0;

          if(grid_curr[nx + ny * cols]) neighbors++;
        }// end for(dy)
      }// end for(dx)

      cell_curr = grid_curr[x + y * cols];
      cell_next = 0;
      switch(neighbors) {
        case 2: // alive with 2 neighbors remains alive
          if(!cell_curr) break;
          // else cell is alive drop through
        case 3: // 3 neighbors == alive
          cell_next = cell_curr + 1;
          if(cell_next > cell_maxage) cell_next = cell_maxage;
      }

      if(cell_next != 0 && cell_curr == 0
          || cell_next == 0 && cell_curr != 0)
        cell_delta++;
      
      if(cell_next) {
        // if this is a new cell, paint it black
        if(cell_next == 1)
          display.fillRect(x * cell_size + offset_x, y * cell_size + offset_y,
            cell_size, cell_size, 1);
        // otherwise paint the inside white depending on how old it is
        else display.fillRect(
          x * cell_size + cell_size / 2 - cell_next + offset_x,
          y * cell_size + cell_size / 2 - cell_next + offset_y,
          cell_next * 2, cell_next * 2, 0);
      } else if (cell_curr) {
        // otherwise it's died, paint the whole cell white
        display.fillRect(x * cell_size + offset_x, y * cell_size + offset_y,
          cell_size, cell_size, 0);
      }

      grid_next[x + y * cols] = cell_next;
    }//end for(y)
  }//end for(x)

  // swap which grid is current
  grid_tmp = grid_next; grid_next = grid_curr; grid_curr = grid_tmp;

  // the longer this goes, the more cells this has,
  //   the more change is required or we reset
  if(cell_delta * cell_size < frame_count) randomize();
  else frame_count++;

  display.partialUpdate();
}
