/*
    Inkplate4_Black_White_Red example for Soldered Inkplate 4
    For this example you will need only a USB-C cable and Inkplate 4.
    Select "Soldered Inkplate4" from Tools -> Board menu.
    Don't have "Soldered Inkplate4" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This example will show you how you can draw some simple graphics using
    Adafruit GFX functions. Yes, Inkplate library is 100% compatible with GFX lib!
    Learn more about Adafruit GFX: https://learn.adafruit.com/adafruit-gfx-graphics-library )

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: https://forum.soldered.com/
    6 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4
#error "Wrong board selection for this example, please select Soldered Inkplate4 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch
Inkplate display;     // Create object on Inkplate library

// Define the delay between drawing
#define DELAY_MS 5000

void setup()
{
    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).
    display.setTextSize(3); // Set text size to be 3 times bigger than original (5x7 pix)
    display.drawTextWithShadow(105, 125, "Welcome to", INKPLATE_BLACK, INKPLATE_RED);  // Draw a text with shadow
    display.drawTextWithShadow(105, 150, "Inkplate 4!", INKPLATE_BLACK, INKPLATE_RED); // Draw a text with shadow
    display.setTextColor(INKPLATE_BLACK, INKPLATE_WHITE); // Set text color to black and background to white
    display.display(); // This line actually drawing on the Inkplate screen, previous lines just drawing into the frame
                       // buffer
    delay(DELAY_MS);   // Wait a little bit

    // Example will demostrate funcionality one by one. You always first set everything in the frame buffer and
    // afterwards you show it on the screen using display.display().

    // Let's start by drawing pixels
    display.clearDisplay(); // Clear everytning that is inside frame buffer in ESP32
    displayCurrentAction("Drawing two pixels in different colors"); // Function which writes small text at bottom left
                                                                    // indicating what's currently done
    // NOTE: you do not need displayCurrentAction function to use Inkplate!
    display.drawPixel(200, 150, INKPLATE_BLACK); // Draw one black pixel at X = 200, Y = 150 position in black color

    display.drawPixel(150, 200, INKPLATE_RED); // Draw one black pixel at X = 150, Y = 200 position in red color

    display.display(); // Send image to display. You need to call this one each time you want to transfer frame buffer
    // to the screen.
    delay(DELAY_MS); // Wait a little bit

    // Now, let's draw some random pixels!
    display.clearDisplay(); // Clear everything that is inside frame buffer in ESP32
    for (int i = 0; i < 500; i++)
    {
        // Write 500 random pixels at random locations
        display.drawPixel(random(0, E_INK_WIDTH), random(0, E_INK_HEIGHT), random(1, 3));
    }
    displayCurrentAction("Drawing 500 random pixels");
    display.display(); // Write everything from frame buffer to screen
    delay(DELAY_MS);   // Wait

    // Draw two diagonal lines accros screen
    display.clearDisplay();

    // All of those drawing fuctions originate from Adafruit GFX library, so maybe you are already familiar with those.
    // Arguments are: start X, start Y, ending X, ending Y, color.
    display.drawLine(0, 0, E_INK_WIDTH, E_INK_HEIGHT, INKPLATE_BLACK);
    display.drawLine(E_INK_WIDTH, 0, 0, E_INK_HEIGHT, INKPLATE_RED);
    displayCurrentAction("Drawing two diagonal lines");
    display.display();
    delay(DELAY_MS);

    // And again, let's draw some random lines on screen!
    display.clearDisplay();
    for (int i = 0; i < 30; i++)
    {
        display.drawLine(random(0, E_INK_WIDTH), random(0, E_INK_HEIGHT), random(0, E_INK_WIDTH),
                         random(0, E_INK_HEIGHT), random(1, 3));
    }
    displayCurrentAction("Drawing 30 random lines");
    display.display();
    delay(DELAY_MS);

    // Let's draw some random thick lines on screen!
    display.clearDisplay();
    for (int i = 0; i < 20; i++)
    {
        display.drawThickLine(random(0, E_INK_WIDTH), random(0, E_INK_HEIGHT), random(0, E_INK_WIDTH),
                              random(0, E_INK_HEIGHT), random(1, 3), (float)random(1, 20));
    }
    displayCurrentAction("Drawing 20 thick lines");
    display.display();
    delay(DELAY_MS);

    // Now draw one horizontal...
    display.clearDisplay();
    display.drawFastHLine(75, 150, 250, INKPLATE_BLACK); // Arguments are: starting X, starting Y, length, color
    displayCurrentAction("Drawing one horizontal line");
    display.display();
    delay(DELAY_MS);

    //... and one vertical line
    display.clearDisplay();
    display.drawFastVLine(200, 50, 200, INKPLATE_RED); // Arguments are: starting X, starting Y, length, color
    displayCurrentAction("Drawing one vertical line");
    display.display();
    delay(DELAY_MS);

    // Now, let's make a grid using only horizontal and vertical lines
    display.clearDisplay();
    for (int i = 0; i < E_INK_WIDTH; i += 8)
    {
        display.drawFastVLine(i, 0, E_INK_HEIGHT, INKPLATE_BLACK);
    }
    for (int i = 0; i < E_INK_HEIGHT; i += 8)
    {
        display.drawFastHLine(0, i, E_INK_WIDTH, INKPLATE_RED);
    }
    displayCurrentAction("Drawing a grid using horizontal and vertical lines");
    display.display();
    delay(DELAY_MS);

    // Draw rectangle at X = 75, Y = 75 and size of 250x150 pixels
    display.clearDisplay();
    display.drawRect(75, 75, 250, 150, INKPLATE_BLACK); // Arguments are: start X, start Y, size X, size Y, color
    displayCurrentAction("Drawing rectangle");
    display.display();
    delay(DELAY_MS);

    // Draw rectangles on random location, size 20x30 pixels
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.drawRect(random(0, E_INK_WIDTH), random(0, E_INK_HEIGHT), 20, 30, random(1, 3));
    }
    displayCurrentAction("Drawing many rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rectangle at X = 75, Y = 75, size of 250x150 pixels
    display.clearDisplay();
    display.fillRect(75, 75, 250, 150, INKPLATE_BLACK); // Arguments are: start X, start Y, size X, size Y, color
    displayCurrentAction("Drawing black rectangle");
    display.display();
    delay(DELAY_MS);

    // Draw filled rectangles on random location, size of 30x20 pixels in different colors
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.fillRect(random(0, E_INK_WIDTH), random(0, E_INK_HEIGHT), 30, 20, random(1, 3));
    }
    displayCurrentAction("Drawing many filled rectangles randomly");
    display.display();
    delay(DELAY_MS);

    // Draw a red circle at center of a screen with radius of 75 pixels
    display.clearDisplay();
    display.drawCircle(E_INK_WIDTH / 2, E_INK_HEIGHT / 2, 75,
                       INKPLATE_RED); // Arguments are: start X, start Y, radius, color
    displayCurrentAction("Drawing a circle");
    display.display();
    delay(DELAY_MS);

    // Draw some circles at random location with radius of 20 pixels
    display.clearDisplay();
    for (int i = 0; i < 30; i++)
    {
        display.drawCircle(random(0, E_INK_WIDTH), random(0, E_INK_HEIGHT), 20, random(1, 3));
    }
    displayCurrentAction("Drawing many circles randomly");
    display.display();
    delay(DELAY_MS);

    // Draw a black filled circle at center of a screen with radius of 75 pixels
    display.clearDisplay();
    display.fillCircle(E_INK_WIDTH / 2, E_INK_HEIGHT / 2, 75,
                       INKPLATE_BLACK); // Arguments are: start X, start Y, radius, color
    displayCurrentAction("Drawing black-filled circle");
    display.display();
    delay(DELAY_MS);

    // Draw some filled circles at random location with radius of 10 pixels
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.fillCircle(random(0, E_INK_WIDTH), random(0, E_INK_HEIGHT), 10, random(1, 3));
    }
    displayCurrentAction("Drawing many filled circles randomly");
    display.display(); // To show stuff on screen, you always need to call display.display();
    delay(DELAY_MS);

    // Draw rounded rectangle at X = 75, Y = 75 and size of 250x150 pixels and radius of 10 pixels
    display.clearDisplay();
    display.drawRoundRect(75, 75, 250, 150, 10,
                          INKPLATE_RED); // Arguments are: start X, start Y, size X, size Y, radius, color
    displayCurrentAction("Drawing rectangle with rounded edges");
    display.display();
    delay(DELAY_MS);

    // Draw rounded rectangles on random location, size 30x20 pixels, radius of 5 pixels
    display.clearDisplay();
    for (int i = 0; i < 40; i++)
    {
        display.drawRoundRect(random(0, E_INK_WIDTH), random(0, E_INK_HEIGHT), 30, 20, 5, random(1, 3));
    }
    displayCurrentAction("Drawing many rounded edges rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rect at X = 75, Y = 75, size of 250x150 pixels and radius of 10 pixels
    display.clearDisplay();
    display.fillRoundRect(75, 75, 250, 150, 10,
                          INKPLATE_BLACK); // Arguments are: start X, start Y, size X, size Y, radius, color
    displayCurrentAction("This is filled rectangle with rounded edges");
    display.display();
    delay(DELAY_MS);

    // Draw filled rects on random location, size of 20x20 pixels, radius of 5 pixels with random color
    display.clearDisplay();
    for (int i = 0; i < 40; i++)
    {
        display.fillRoundRect(random(0, E_INK_WIDTH), random(0, E_INK_HEIGHT), 20, 20, 5, random(1, 3));
    }
    displayCurrentAction("Random rounded edge filled rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw simple triangle
    display.clearDisplay();
    display.drawTriangle(130, 210, 200, 70, 270, 210, INKPLATE_RED); // Arguments are: X1, Y1, X2, Y2, X3, Y3, color
    displayCurrentAction("Drawing a triangle");
    display.display();
    delay(DELAY_MS);

    // Draw filled triangle inside simple triangle (so no display.clearDisplay() this time)
    display.fillTriangle(160, 190, 200, 110, 240, 190, INKPLATE_BLACK); // Arguments are: X1, Y1, X2, Y2, X3, Y3, color
    displayCurrentAction("Drawing a triangle inside exsisting one"); // This will be written over the existing text so
                                                                     // that the previous text will not be seen
    display.display();
    delay(DELAY_MS);

    // Draws an elipse with x radius, y radius, center x, center y and color
    display.clearDisplay();
    display.drawElipse(125, 75, E_INK_WIDTH / 2, E_INK_HEIGHT / 2, INKPLATE_RED);
    displayCurrentAction("Drawing an elipse");
    display.display();
    delay(DELAY_MS);

    // Fills an elipse with x radius, y radius, center x, center y and color
    display.clearDisplay();
    display.fillElipse(125, 75, E_INK_WIDTH / 2, E_INK_HEIGHT / 2, INKPLATE_RED);
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
        xt[i] = random(0, E_INK_WIDTH);
        yt[i] = random(0, E_INK_HEIGHT);
    }
    int k;
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (atan2(yt[j] - 150, xt[j] - 200) < atan2(yt[i] - 150, xt[i] - 200))
            {
                k = xt[i], xt[i] = xt[j], xt[j] = k;
                k = yt[i], yt[i] = yt[j], yt[j] = k;
            }


    // Draws a polygon, from x and y coordinate arrays of n points in color c
    display.clearDisplay();
    display.drawPolygon(xt, yt, n, INKPLATE_BLACK);
    displayCurrentAction("Drawing a polygon");
    display.display();
    delay(DELAY_MS);

    // Fills a polygon, from x and y coordinate arrays of n points in color c,
    // Points need to be counter clockwise sorted
    // Method can be quite slow for now, probably will improve
    display.clearDisplay();
    display.fillPolygon(xt, yt, n, INKPLATE_RED);
    displayCurrentAction("Drawing a filled polygon");
    display.display();
    delay(DELAY_MS);

    // Write text and rotate it by 90 deg. forever
    int r = 0;
    display.setTextSize(4);
    display.setTextColor(INKPLATE_WHITE, INKPLATE_BLACK);
    while (true)
    {
        display.setRotation(r % 4);
        display.setCursor(40, 40);
        display.clearDisplay();
        display.print("INKPLATE4");
        display.display();
        r++;
        delay(DELAY_MS);
    }
}


void loop()
{
    // Never here...
}

// Small function that will write on the screen what function is currently in demonstration.
void displayCurrentAction(String text)
{
    display.setTextSize(1);
    display.setCursor(2, 291);
    display.print(text);
}
