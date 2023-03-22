/*
   Inkplate5_Black_And_White example for Soldered Inkplate 5
   For this example you will need only a USB-C cable and Inkplate 5.
   Select "Soldered Inkplate5" from Tools -> Board menu.
   Don't have "Soldered Inkplate5" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can draw some simple graphics using
   Adafruit GFX functions. Yes, Inkplate library is 100% compatible with GFX lib!
   Learn more about Adafruit GFX: https://learn.adafruit.com/adafruit-gfx-graphics-library )

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   20 March 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
#include "logo.h"                // Include header file for Soldered logo. You can see it in next tab inside Arduino IDE
Inkplate display(INKPLATE_1BIT); // Create object on Inkplate library and set library to work in monochorme mode
// Other option is gray mode, which is demonstrated in next example "Inkplate5_Grayscale"

// Height and width of e-paper
#define HEIGHT 540
#define WIDTH  960

// Delay in milliseconds between screen refresh. Refreshing e-paper screens more often than 5s is not recommended
#define DELAY_MS 5000
// Want to refresh faster? Use partial update! Find example in "Inkplate5_Partial_Update"


void setup()
{
    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
    //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).
    display.setCursor(200, 260);
    display.setTextColor(BLACK, WHITE); // Set black text with white background
    display.setTextSize(4);             // Set text size 4 times bigger than original (5 x 7 px)
    display.print("Welcome to Inkplate 5!");
    display.display(); // Write hello message on the screen
    delay(5000);       // Wait a little bit
}

void loop()
{
    // Example will demostrate funcionality one by one. You always first set everything in the frame buffer and
    // afterwards you show it on the screen using display.display().

    // Let's start by drawing a pixel at x = 100 and y = 50 location
    display.clearDisplay(); // Clear everytning that is inside frame buffer in ESP32
    displayCurrentAction(
        "Drawing a pixel"); // Function which writes small text at bottom left indicating what's currently done
                            // NOTE: you do not need displayCurrentAction function to use Inkplate!
    display.drawPixel(100, 50, BLACK); // Draw one black pixel at X = 100, Y = 50 position in BLACK color (must be black
                                       // since Inkplate is in BW mode)
    display.display(); // Send image to display. You need to call this one each time you want to transfer frame buffer
                       // to the screen.
    delay(DELAY_MS);   // Wait a little bit

    // Now, let's draw some random pixels!
    display.clearDisplay(); // Clear everything that is inside frame buffer in ESP32
    for (int i = 0; i < 600; i++)
    {
        // Write 600 black pixels at random locations
        display.drawPixel(random(0, WIDTH), random(0, HEIGHT), BLACK);
    }
    displayCurrentAction("Drawing 600 random pixels");
    display.display(); // Write everything from frame buffer to screen
    delay(DELAY_MS);   // Wait

    // Draw two diagonal lines accros screen
    display.clearDisplay();
    display.drawLine(
        0, 0, display.width(), display.height(),
        BLACK); // All of those drawing fuctions originate from Adafruit GFX library, so maybe you are already familiar
    display.drawLine(WIDTH, 0, 0, HEIGHT,
                     BLACK); // with those. Arguments are: start X, start Y, ending X, ending Y, color.
    displayCurrentAction("Drawing two diagonal lines");
    display.display();
    delay(DELAY_MS);

    // And again, let's draw some random lines on screen!
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.drawLine(random(0, WIDTH), random(0, HEIGHT), random(0, WIDTH), random(0, HEIGHT), BLACK);
    }
    displayCurrentAction("Drawing 50 random lines");
    display.display();
    delay(DELAY_MS);

    // Let's draw some random thick lines on screen!
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.drawThickLine(random(0, WIDTH), random(0, HEIGHT), random(0, WIDTH), random(0, HEIGHT), BLACK,
                              (float)random(1, 20));
    }
    displayCurrentAction("Drawing 50 random thick lines");
    display.display();
    delay(DELAY_MS);

    // Now draw one horizontal...
    display.clearDisplay();
    display.drawFastHLine(100, 100, 600, BLACK); // Arguments are: starting X, starting Y, length, color
    displayCurrentAction("Drawing one horizontal line");
    display.display();
    delay(DELAY_MS);

    //... and one vertical line
    display.clearDisplay();
    display.drawFastVLine(100, 100, 400, BLACK); // Arguments are: starting X, starting Y, length, color
    displayCurrentAction("Drawing one vertical line");
    display.display();
    delay(DELAY_MS);

    // Now, let' make a grid using only horizontal and vertical lines
    display.clearDisplay();
    for (int i = 0; i < WIDTH; i += 8)
    {
        display.drawFastVLine(i, 0, HEIGHT, BLACK);
    }
    for (int i = 0; i < HEIGHT; i += 8)
    {
        display.drawFastHLine(0, i, WIDTH, BLACK);
    }
    displayCurrentAction("Drawing a grid using horizontal and vertical lines");
    display.display();
    delay(DELAY_MS);

    // Draw rectangle at X = 250, Y = 140 and size of 400x300 pixels
    display.clearDisplay();
    display.drawRect(250, 140, 400, 300, BLACK); // Arguments are: start X, start Y, size X, size Y, color
    displayCurrentAction("Drawing rectangle");
    display.display();
    delay(DELAY_MS);

    // Draw rectangles on random location, size 100x150 pixels
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.drawRect(random(0, WIDTH), random(0, HEIGHT), 100, 150, BLACK);
    }
    displayCurrentAction("Drawing many rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rectangle at X = 250, Y = 140, size of 400x300 pixels
    display.clearDisplay();
    display.fillRect(250, 140, 400, 300, BLACK); // Arguments are: start X, start Y, size X, size Y, color
    displayCurrentAction("Drawing black rectangle");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rectangles on random location, size of 30x30 pixels
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.fillRect(random(0, WIDTH), random(0, HEIGHT), 30, 30, BLACK);
    }
    displayCurrentAction("Drawing many filled rectangles randomly");
    display.display();
    delay(DELAY_MS);

    // Draw circle at center of a screen with radius of 75 pixels
    display.clearDisplay();
    display.drawCircle(WIDTH / 2, HEIGHT / 2, 75, BLACK); // Arguments are: start X, start Y, radius, color
    displayCurrentAction("Drawing a circle");
    display.display();
    delay(DELAY_MS);

    // Draw some circles at random location with radius of 25 pixels
    display.clearDisplay();
    for (int i = 0; i < 40; i++)
    {
        display.drawCircle(random(0, WIDTH), random(0, HEIGHT), 25, BLACK);
    }
    displayCurrentAction("Drawing many circles randomly");
    display.display();
    delay(DELAY_MS);

    // Draw black filled circle at center of a screen with radius of 75 pixels
    display.clearDisplay();
    display.fillCircle(WIDTH / 2, HEIGHT / 2, 75, BLACK); // Arguments are: start X, start Y, radius, color
    displayCurrentAction("Drawing black-filled circle");
    display.display();
    delay(DELAY_MS);

    // Draw some black filled circles at random location with radius of 15 pixels
    display.clearDisplay();
    for (int i = 0; i < 40; i++)
    {
        display.fillCircle(random(0, WIDTH), random(0, HEIGHT), 15, BLACK);
    }
    displayCurrentAction("Drawing many filled circles randomly");
    display.display(); // To show stuff on screen, you always need to call display.display();
    delay(DELAY_MS);

    // Draw rounded rectangle at X = 250, Y = 140 and size of 400x300 pixels and radius of 10 pixels
    display.clearDisplay();
    display.drawRoundRect(250, 140, 400, 300, 10,
                          BLACK); // Arguments are: start X, start Y, size X, size Y, radius, color
    displayCurrentAction("Drawing rectangle with rounded edges");
    display.display();
    delay(DELAY_MS);

    // Draw rounded rectangles on random location, size 100x150 pixels, radius of 5 pixels
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.drawRoundRect(random(0, WIDTH), random(0, HEIGHT), 100, 150, 5, BLACK);
    }
    displayCurrentAction("Drawing many rounded edges rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rect at X = 250, Y = 140, size of 400x300 pixels and radius of 10 pixels
    display.clearDisplay();
    display.fillRoundRect(250, 140, 400, 300, 10,
                          BLACK); // Arguments are: start X, start Y, size X, size Y, radius, color
    displayCurrentAction("This is filled rectangle with rounded edges");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rects on random location, size of 30x30 pixels, radius of 3 pixels
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.fillRoundRect(random(0, WIDTH), random(0, HEIGHT), 30, 30, 3, BLACK);
    }
    displayCurrentAction("Random rounded edge filled rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw simple triangle
    display.clearDisplay();
    display.drawTriangle(250, 400, 550, 400, 400, 100, BLACK); // Arguments are: X1, Y1, X2, Y2, X3, Y3, color
    displayCurrentAction("Drawing triangle");
    display.display();
    delay(DELAY_MS);

    // Draw filled triangle inside simple triangle (so no display.clearDisplay() this time)
    display.fillTriangle(300, 350, 500, 350, 400, 150, BLACK); // Arguments are: X1, Y1, X2, Y2, X3, Y3, color
    displayCurrentAction("Drawing triangle inside exsisting one");
    display.display();
    delay(DELAY_MS);

    // Display some bitmap on screen. We are going to display Soldered logo on display at location X = 50, Y = 200
    // Image is 800x160 pixels and we want to every pixel of this bitmap to be black.
    display.clearDisplay();
    display.drawImage(logo, 50, 200, logo_w, logo_h,
                      BLACK); // Arguments are: array variable name, start X, start Y, size X, size Y, color
    displayCurrentAction("Drawing Soldered logo");
    display.display();
    delay(DELAY_MS);


    // Write some text on screen with different sizes
    display.clearDisplay();
    for (int i = 0; i < 6; i++)
    {
        display.setTextSize(i + 1);               // textSize parameter starts at 1 and goes up to 6
        display.setCursor(70, 130 + (i * i * 8)); // setCursor works as same as on LCD displays - sets "the cursor" at
                                                  // the place you want to write someting next
        display.print("INKPLATE 5!");             // The actual text you want to show on e-paper as String
    }
    displayCurrentAction("Text in different sizes and shadings");
    display.display(); // To show stuff on screen, you always need to call display.display();
    delay(DELAY_MS);

    // Write same text on different location, but now invert colors (text is white, text background is black), without
    // cleaning the previous text
    display.setTextColor(WHITE, BLACK); // First argument is text color, while second argument is background color. In
                                        // BW, there are only two options: BLACK & WHITE
    for (int i = 0; i < 6; i++)
    {
        display.setTextSize(i + 1);
        display.setCursor(500, 130 + (i * i * 8));
        display.print("INKPLATE 5!");
    }
    display.display();
    delay(DELAY_MS);

    // Draws an elipse with x radius, y radius, center x, center y and color
    display.clearDisplay();
    display.drawElipse(100, 200, WIDTH / 2, HEIGHT / 2, BLACK);
    displayCurrentAction("Drawing an elipse");
    display.display();

    delay(DELAY_MS);

    // Fills an elipse with x radius, y radius, center x, center y and color
    display.clearDisplay();
    display.fillElipse(100, 200, WIDTH / 2, HEIGHT / 2, BLACK);
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
        xt[i] = random(100, WIDTH);
        yt[i] = random(100, HEIGHT);
    }
    int k;
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (atan2(yt[j] - 300, xt[j] - 400) < atan2(yt[i] - 300, xt[i] - 400))
            {
                k = xt[i], xt[i] = xt[j], xt[j] = k;
                k = yt[i], yt[i] = yt[j], yt[j] = k;
            }

    // Draws a polygon, from x and y coordinate arrays of n points in color c
    display.clearDisplay();
    display.drawPolygon(xt, yt, n, BLACK);
    displayCurrentAction("Drawing a polygon");
    display.display();

    delay(DELAY_MS);

    // Fills a polygon, from x and y coordinate arrays of n points in color c,
    // Points need to be counter clockwise sorted
    // Method can be quite slow for now, probably will improve
    display.clearDisplay();
    display.fillPolygon(xt, yt, n, BLACK);
    displayCurrentAction("Drawing a filled polygon");
    display.display();

    delay(DELAY_MS);

    // Write text and rotate it by 90 deg. forever
    int r = 0;
    display.setTextSize(8);
    display.setTextColor(WHITE, BLACK);
    while (true)
    {
        display.setCursor(50, 50);
        display.clearDisplay();
        display.setRotation(
            r); // Set rotation will sent rotation for the entire display, so you can use it sideways or upside-down
        display.print("INKPLATE5");
        display.display();
        r++;
        delay(DELAY_MS);
    }

    // Did you know that you can change between BW and greyscale mode anytime?
    // Just call display.setDisplayMode(mode)
}

// Small function that will write on the screen what function is currently in demonstration.
void displayCurrentAction(String text)
{
    display.setTextSize(2);
    display.setCursor(2, 520);
    display.print(text);
}
