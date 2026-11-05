/**
 **************************************************
 * @file        Inkplate10_microSD_TXT_Write.ino
 * @brief       Write and read a log file on the microSD card with
 *              Soldered Inkplate 10.
 *
 * @details     Adds a line to a log file on the microSD card every
 *              time the board starts, then shows how many lines the
 *              file has, how big the card is and which files are on
 *              it. It shows how to initialize the card, append to and
 *              read a text file, and list the card contents.
 *
 *              The SD card must be formatted as FAT32. Always close
 *              files after writing to prevent corruption. Expected
 *              output is the line count, the card size and the file
 *              list on the Inkplate display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable, microSD card
 * - Extra:      SourceSans3_24.h font file in the sketch folder
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Insert a FAT32 formatted microSD card into the slot.
 * 3) Upload the sketch to Inkplate 10.
 * 4) The file information appears on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Borna Zelenika @ soldered.com
 * @date        2026-11-05
 * @license     GNU GPL V3
 **************************************************/

// Pull in the Inkplate library, then the font file that sits next to this sketch
#include "Inkplate.h"
#include "SourceSans3_24.h"

// Black and white mode: a colour is BLACK or WHITE
Inkplate display(INKPLATE_1BIT);

// One object for the log file, and one for the root folder of the card
SdFile file;
SdFile root;

// The name of the log file and the line added to it on every start
const char *fileName = "example.txt";
const char *logLine = "Hello from Inkplate!\n";

// Stop with a message on the screen, and put the card to sleep
void fail(const char *message) {
  display.println(message);
  display.display();
  display.sdCardSleep();
  while (true);
}

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(2);
  display.setCursor(50, 50);

  // Start the card. If it fails, say so, and stop.
  if (!display.sdCardInit()) {
    fail("SD card error!");
  }

  // Open the file for writing. O_APPEND adds to the end instead of starting over, and
  // O_CREAT creates the file the first time.
  if (!file.open(fileName, O_WRONLY | O_CREAT | O_APPEND)) {
    fail("Could not open the file!");
  }
  file.write(logLine);
  file.close();  // close it, so the data reaches the card

  // Open the file again for reading, and count the lines by counting the line ends
  if (!file.open(fileName, O_RDONLY)) {
    fail("Could not read the file!");
  }
  int lines = 0;
  while (file.available()) {
    if (file.read() == '\n') {
      lines++;
    }
  }
  int bytes = file.fileSize();
  file.close();

  // The size of the card: sectors of 512 bytes, shown in megabytes
  uint32_t cardMegabytes = display.getSdFat().card()->sectorCount() / 2048;

  // Show the results
  display.print("Card size: ");
  display.print(cardMegabytes);
  display.println(" MB");
  display.print(fileName);
  display.print(": ");
  display.print(lines);
  display.print(" lines, ");
  display.print(bytes);
  display.println(" bytes");
  display.println();
  display.println("Files on the card:");

  // Go through the root folder, one entry at a time
  root.open("/");
  SdFile entry;
  while (entry.openNext(&root, O_RDONLY)) {
    char name[40];
    entry.getName(name, sizeof(name));
    display.print("  ");
    display.print(name);
    display.print("  ");
    display.print(entry.fileSize());
    display.println(" bytes");
    entry.close();
  }
  root.close();

  // Send the frame buffer to the screen, then let the card sleep
  display.display();
  display.sdCardSleep();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}
