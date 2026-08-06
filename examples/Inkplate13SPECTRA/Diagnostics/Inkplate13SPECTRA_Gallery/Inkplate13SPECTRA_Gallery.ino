/**
 **************************************************
 * @file        Inkplate13SPECTRA_Gallery.ino
 * @brief       Random SD card image gallery for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how you can use Inkplate 13SPECTRA to display random images
 *              from the root folder of the SD card.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable, microSD card with images in the
 *               root folder
 * - Extra:      None
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Copy images to the root folder of the SD card and insert it into the
 *    Inkplate.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) A random image from the SD card is shown on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-21
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate inkplate;     // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;          // Create SdFile object used for accessing files on SD card

#define DELAY_MS 5 * 60 * 1000

// Here we store possible image paths
String files[100];
int fileCount = 0;

// Generic helper function to see if null terminated c string ends with other string
// https://stackoverflow.com/questions/744766/how-to-compare-ends-of-strings-in-c
int endsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void setup()
{
    // Initialize serial
    Serial.begin(115200);

    inkplate.begin(); // Init Inkplate library (you should call this function ONLY ONCE)

    // Init SD card. Display if SD card is init propery or not.
    if (inkplate.sdCardInit())
    {
        // Temporary buffer for file name
        char a[50];
        FsFile f, root = inkplate.getSdFat().open("/");         // Currently open file and the directory we want images from e.g. root /
        while (f.openNext(&root, O_RDONLY) && f.getName(a, 40)) // Iterate over all files in directory
        {
            if (f.isFile() && !f.isHidden() && a[0] != '.' &&                        // Check if current file isn't a directory
                (endsWith(a, ".bmp") || endsWith(a, ".jpg") || endsWith(a, ".png"))) // Check if the file is an image
                Serial.println(files[fileCount++] = String(a));                      // Print the file name and add it to all files
            f.close();                                                               // Close the file
        }

        Serial.println(); // Blank print to separate all files from the chosen one

        if (fileCount != 0) // If there we're any files choose one and draw it
        {
            String s = files[random(0, fileCount)];
            inkplate.image.draw(s, 0, 0);
            Serial.println("Drawn image: ");
            Serial.println(s);
            inkplate.display();
        }
    }
    else
    {
      Serial.println("failed opening sd");
    }

    // Go to sleep for DELAY_MS
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);

    // Put SD card into deep sleep
    inkplate.sdCardSleep();

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}