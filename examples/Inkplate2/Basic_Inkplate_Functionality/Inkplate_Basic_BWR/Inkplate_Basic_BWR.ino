/*
    Basic_monochorme Black Red and White example for soldered.com Inkplate 2
    For this example you will need only USB cable and Inkplate 2.
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can draw some simple graphics using
    Adafruit GFX functions. Yes, Inkplate library is 100% compatible with GFX lib!
    Learn more about Adafruit GFX: https://learn.adafruit.com/adafruit-gfx-graphics-library )

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    30 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display; // Create object on Inkplate library and set library to work in monochorme mode

#define DELAY_MS 5000

void setup()
{
    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).
    display.display(); // Clear everything that has previously been on a screen
    display.setCursor(10, 10);
    display.setTextSize(2);
    display.drawTextWithShadow(0, 30, "Welcome to", INKPLATE2_RED, INKPLATE2_BLACK); //Draw text with shadow
    display.drawTextWithShadow(0, 50, "Inkplate 2!", INKPLATE2_RED, INKPLATE2_BLACK); //Draw text with shadow
    display.display(); // Write hello message
    delay(5000);       // Wait a little bit

    // Example will demostrate funcionality one by one. You always first set everything in the frame buffer and
    // afterwards you show it on the screen using display.display().

    // Let's start by drawing a pixel at x = 100 and y = 50 location
    display.clearDisplay(); // Clear everytning that is inside frame buffer in ESP32
    displayCurrentAction(
        "Drawing two pixels in different colors"); // Function which writes small text at bottom left indicating what's currently done
    // NOTE: you do not need displayCurrentAction function to use Inkplate!
    display.drawPixel(100, 50, INKPLATE2_BLACK); // Draw one black pixel at X = 100, Y = 50 position in INKPLATE2_BLACK color (must be black
    // since Inkplate is in BW mode)
    display.drawPixel(50, 100, INKPLATE2_RED); // Draw one black pixel at X = 100, Y = 50 position in INKPLATE2_BLACK color (must be black
    // since Inkplate is in BW mode)
    display.display(); // Send image to display. You need to call this one each time you want to transfer frame buffer
    // to the screen.
    delay(DELAY_MS);   // Wait a little bit

    // Now, let's draw some random pixels!
    display.clearDisplay(); // Clear everything that is inside frame buffer in ESP32
    for (int i = 0; i < 300; i++)
    {   // Write 600 black pixels at random locations
        display.drawPixel(random(0, 211), random(0, 80), random(1, 3));
    }
    displayCurrentAction("Drawing 300 random pixels");
    display.display(); // Write everything from frame buffer to screen
    delay(DELAY_MS);   // Wait

    // Draw two diagonal lines accros screen
    display.clearDisplay();
    display.drawLine(
        0, 0, 211, 80,
        INKPLATE2_BLACK); // All of those drawing fuctions originate from Adafruit GFX library, so maybe you are already familiar
    display.drawLine(211, 0, 0, 80, INKPLATE2_BLACK); // with those. Arguments are: start X, start Y, ending X, ending Y, color.
    displayCurrentAction("Drawing two diagonal lines");
    display.display();
    delay(DELAY_MS);

    // And again, let's draw some random lines on screen!
    display.clearDisplay();
    for (int i = 0; i < 30; i++)
    {
        display.drawLine(random(0, 211), random(0, 80), random(0, 211), random(0, 80), random(1, 3));
    }
    displayCurrentAction("Drawing 30 random lines");
    display.display();
    delay(DELAY_MS);

    // Let's draw some random thick lines on screen!
    display.clearDisplay();
    for (int i = 0; i < 20; i++)
    {
        display.drawThickLine(random(0, 211), random(0, 80), random(0, 211), random(0, 80), INKPLATE2_BLACK,
                              (float)random(1, 20));
    }
    displayCurrentAction("Drawing 20 random lines");
    display.display();
    delay(DELAY_MS);

    // Now draw one horizontal...
    display.clearDisplay();
    display.drawFastHLine(100, 80, 104, random(1, 3)); // Arguments are: starting X, starting Y, length, color
    displayCurrentAction("Drawing one horizontal line");
    display.display();
    delay(DELAY_MS);

    //... and one vertical line
    display.clearDisplay();
    display.drawFastVLine(100, 10, 90, random(1, 3)); // Arguments are: starting X, starting Y, length, color
    displayCurrentAction("Drawing one vertical line");
    display.display();
    delay(DELAY_MS);

    // Now, let' make a grid using only horizontal and vertical lines
    display.clearDisplay();
    for (int i = 0; i < 212; i += 8)
    {
        display.drawFastVLine(i, 0, 80, INKPLATE2_BLACK);
    }
    for (int i = 0; i < 81; i += 4)
    {
        display.drawFastHLine(0, i, 212, INKPLATE2_RED);
    }
    displayCurrentAction("Drawing a grid using horizontal and vertical lines");
    display.display();
    delay(DELAY_MS);

    // Draw rectangle at X = 20, Y = 20 and size of 100x35 pixels
    display.clearDisplay();
    display.drawRect(20, 20, 100, 35, random(1, 3)); // Arguments are: start X, start Y, size X, size Y, color
    displayCurrentAction("Drawing rectangle");
    display.display();
    delay(DELAY_MS);

    // Draw rectangles on random location, size 10x15 pixels
    display.clearDisplay();
    for (int i = 0; i < 20; i++)
    {
        display.drawRect(random(0, 211), random(0, 60), 10, 15, random(1, 3));
    }
    displayCurrentAction("Drawing many rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rectangle at X = 20, Y = 20, size of 40x30 pixels
    display.clearDisplay();
    display.fillRect(20, 20, 40, 30, random(1, 3)); // Arguments are: start X, start Y, size X, size Y, color
    displayCurrentAction("Drawing black rectangle");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rectangles on random location, size of 30x30 pixels
    display.clearDisplay();
    for (int i = 0; i < 20; i++)
    {
        display.fillRect(random(0, 211), random(0, 60), 30, 20, random(1, 3));
    }
    displayCurrentAction("Drawing many filled rectangles randomly");
    display.display();
    delay(DELAY_MS);

    // Draw circle at center of a screen with radius of 35 pixels
    display.clearDisplay();
    display.drawCircle(80, 50, 35, random(1, 3)); // Arguments are: start X, start Y, radius, color
    displayCurrentAction("Drawing a circle");
    display.display();
    delay(DELAY_MS);

    // Draw some circles at random location with radius of 25 pixels
    display.clearDisplay();
    for (int i = 0; i < 15; i++)
    {
        display.drawCircle(random(0, 211), random(0, 60), 20, random(1, 3));
    }
    displayCurrentAction("Drawing many circles randomly");
    display.display();
    delay(DELAY_MS);

    // Draw black filled circle at center of a screen with radius of 35 pixels
    display.clearDisplay();
    display.fillCircle(80, 40, 35, random(1, 3)); // Arguments are: start X, start Y, radius, color
    displayCurrentAction("Drawing black-filled circle");
    display.display();
    delay(DELAY_MS);

    // Draw some black filled circles at random location with radius of 15 pixels
    display.clearDisplay();
    for (int i = 0; i < 40; i++)
    {
        display.fillCircle(random(0, 211), random(0, 65), 15, random(1, 3));
    }
    displayCurrentAction("Drawing many filled circles randomly");
    display.display(); // To show stuff on screen, you always need to call display.display();
    delay(DELAY_MS);

    // Draw rounded rectangle at X = 20, Y = 20 and size of 40x30 pixels and radius of 10 pixels
    display.clearDisplay();
    display.drawRoundRect(20, 20, 40, 30, 10,
                          random(1, 3)); // Arguments are: start X, start Y, size X, size Y, radius, color
    displayCurrentAction("Drawing rectangle with rounded edges");
    display.display();
    delay(DELAY_MS);

    // Draw rounded rectangles on random location, size 30x20 pixels, radius of 5 pixels
    display.clearDisplay();
    for (int i = 0; i < 20; i++)
    {
        display.drawRoundRect(random(0, 211), random(0, 60), 30, 20, 5, random(1, 3));
    }
    displayCurrentAction("Drawing many rounded edges rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rect at X = 20, Y = 20, size of 40x30 pixels and radius of 6 pixels
    display.clearDisplay();
    display.fillRoundRect(20, 20, 40, 30, 6,
                          random(1, 3)); // Arguments are: start X, start Y, size X, size Y, radius, color
    displayCurrentAction("This is filled rectangle with rounded edges");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rects on random location, size of 15x15 pixels, radius of 3 pixels
    display.clearDisplay();
    for (int i = 0; i < 20; i++)
    {
        display.fillRoundRect(random(0, 211), random(0, 80), 15, 15, 3, random(1, 3));
    }
    displayCurrentAction("Random rounded edge filled rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw simple triangle
    display.clearDisplay();
    display.drawTriangle(25, 40, 55, 40, 40, 10, random(1, 3)); // Arguments are: X1, Y1, X2, Y2, X3, Y3, color
    display.display();
    delay(DELAY_MS);

    // Draw filled triangle inside simple triangle (so no display.clearDisplay() this time)
    display.fillTriangle(30, 35, 50, 35, 40, 15, random(1, 3)); // Arguments are: X1, Y1, X2, Y2, X3, Y3, color
    displayCurrentAction("Drawing filled triangle inside exsisting one");
    display.display();
    delay(DELAY_MS);

    // Draws an elipse with x radius, y radius, center x, center y and color
    display.clearDisplay();
    display.drawElipse(50, 50, 40, 30, random(1, 3));
    displayCurrentAction("Drawing an elipse");
    display.display();

    delay(DELAY_MS);

    // Fills an elipse with x radius, y radius, center x, center y and color
    display.clearDisplay();
    display.fillElipse(50, 50, 40, 30, random(1, 3));
    displayCurrentAction("Drawing a filled elipse");
    display.display();

    delay(DELAY_MS);

    // Code block for generating random points and sorting them in a counter
    // clockwise direction.
    int xt[10];
    int yt[10];
    int n = 10;
    for (int i = 0; i < n; ++i)
    {
        xt[i] = random(10, 200);
        yt[i] = random(10, 80);
    }
    int k;
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (atan2(yt[j] - 35, xt[j] - 105) < atan2(yt[i] - 35, xt[i] - 105))
            {
                k = xt[i], xt[i] = xt[j], xt[j] = k;
                k = yt[i], yt[i] = yt[j], yt[j] = k;
            }

    // Draws a polygon, from x and y coordinate arrays of n points in color c
    display.clearDisplay();
    display.drawPolygon(xt, yt, n, INKPLATE2_BLACK);
    displayCurrentAction("Drawing a polygon");
    display.display();

    delay(DELAY_MS);

    // Fills a polygon, from x and y coordinate arrays of n points in color c,
    // Points need to be counter clockwise sorted
    // Method can be quite slow for now, probably will improve
    display.clearDisplay();
    display.fillPolygon(xt, yt, n, INKPLATE2_RED);
    displayCurrentAction("Drawing a filled polygon");
    display.display();

    delay(DELAY_MS);

    // Write text and rotate it by 90 deg. forever
    int r = 0;
    display.setTextSize(3);
    display.setTextColor(INKPLATE2_WHITE, INKPLATE2_BLACK);
    while (true)
    {
        display.setRotation(r % 4);
        display.setCursor(10, 10);
        display.clearDisplay();
        display.print("INKPLATE2");
        display.display();
        r++;
        delay(DELAY_MS);
    }
}


void loop()
{

}

// Small function that will write on the screen what function is currently in demonstration.
void displayCurrentAction(String text)
{
    display.setTextSize(1);
    display.setCursor(2, 85);
    display.print(text);
}
