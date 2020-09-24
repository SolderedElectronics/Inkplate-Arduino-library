/*
   Basic_gray example for e-radionica.com Inkplate 6
   For this example you will need only USB cable and Inkplate 6
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can draw some simple graphics using
   Adafruit GFX functions. Yes, Inkplate library is 100% compatible with GFX lib!
   Learn more about Adafruit GFX: https://learn.adafruit.com/adafruit-gfx-graphics-library
   Inkplate will be used in gray mode which is 3 bit, so you can have up to 8 different colors (black, 6 gray
   colors and white) Color is represented by number, where number 0 means black and number 7 means white, while
   everything in between are shades of gray.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   15 July 2020 by e-radionica.com
*/

#include "Inkplate.h" //Include Inkplate library to the sketch
#include "image.h"    //Include image file that holds gray image data. You can see it in next tab inside Arduino IDE.
Inkplate display(INKPLATE_3BIT); // Create object on Inkplate library and set library to work in gray mode (3-bit)
                                 // Other option is BW mode, which is demonstrated in next example
                                 // "Inkplate_basic_BW"

#define DELAY_MS                                                                                                       \
    5000 //Delay in milliseconds between screen refresh. Refreshing e-paper screens more often than 5s is not recommended \
                      //Want to refresh faster? Use partial update! Find example in "Inkplate-basic_partial_update"

void setup()
{
    display.begin();        // Init library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear any data that may have been in (software) frame buffer.
                            //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside
                            // ESP32).
    display.clean();        // Clear everything that has previously been on a screen
    display.setTextColor(0, 7);
    display.setCursor(150, 320);
    display.setTextSize(4);
    display.print("Welcome to Inkplate 6!");
    display.display(); // Write hello message
    delay(5000);       // Wait a little bit
}

void loop()
{
    // Example will demostrate funcionality one by one. You always first set everything in the frame buffer and
    // afterwards you show it on the screen using display.display().

    // Let'sstart by drawing pixel at x = 100 and y = 50 location
    display.clearDisplay();        // Clear everytning that is inside frame buffer in ESP32
    display.drawPixel(100, 50, 0); // Draw one black pixel at X = 100, Y = 50 position in 0 (BLACK) color
    displayCurrentAction(
        "Drawing a pixel"); // Function which writes small text at bottom left indicating what's currently done
                            // NOTE: you do not need displayCurrentAction function to use Inkplate!
    display.display(); // Send image to display. You need to call this one each time you want to transfer frame buffer
                       // to the screen.
    delay(DELAY_MS);   // Wait a little bit

    // Now, let's draw some random pixels!
    display.clearDisplay(); // Clear everything that is inside frame buffer in ESP32
    for (int i = 0; i < 1000; i++)
    { // Write 1000 random colored pixels at random locations
        display.drawPixel(random(0, 799), random(0, 599),
                          random(0, 7)); // We are setting color of the pixels using numbers from 0 to 7,
    }                                    // where 0 mens black, 7 white and gray is in between
    displayCurrentAction("Drawing 600 random pixels in random colors");
    display.display(); // Write everything from frame buffer to screen
    delay(DELAY_MS);   // Wait

    // Draw two diagonal lines accros screen
    display.clearDisplay();
    display.drawLine(
        0, 0, 799, 599,
        0); // All of those drawing fuctions originate from Adafruit GFX library, so maybe you are already familiar
    display.drawLine(799, 0, 0, 599, 0); // with those. Arguments are: start X, start Y, ending X, ending Y, color.
    displayCurrentAction("Drawing two diagonal lines");
    display.display();
    delay(DELAY_MS);

    // And again, let's draw some random lines on screen!
    display.clearDisplay();
    for (int i = 0; i < 100; i++)
    {
        display.drawLine(random(0, 799), random(0, 599), random(0, 799), random(0, 599), random(0, 7));
    }
    displayCurrentAction("Drawing 50 random lines in random colors");
    display.display();
    delay(DELAY_MS);

    // Let's draw some random thick lines on screen!
    display.clearDisplay();
    for (int i = 0; i < 100; i++)
    {
        display.drawThickLine(random(0, 799), random(0, 599), random(0, 799), random(0, 599), random(0, 7),
                              (float)random(1, 20));
    }
    displayCurrentAction("Drawing 50 random lines in random colors and thickness");
    display.display();
    delay(DELAY_MS);

    // Let's draw some random gradient thick lines on screen!
    display.clearDisplay();
    for (int i = 0; i < 100; i++)
    {
        int startColor = random(0, 7);
        int endColor = random(startColor, 7);
        display.drawGradientLine(random(0, 799), random(0, 599), random(0, 799), random(0, 599), startColor, endColor,
                                 (float)random(1, 20));
    }
    displayCurrentAction("Drawing 50 random gradient lines in random colors and thickness");
    display.display();
    delay(DELAY_MS);

    // Now draw one horizontal...
    display.clearDisplay();
    display.drawFastHLine(100, 100, 600, 0); // Arguments are: starting X, starting Y, length, color
    displayCurrentAction("Drawing one horizontal line");
    display.display();
    delay(DELAY_MS);

    //... and one vertical line
    display.clearDisplay();
    display.drawFastVLine(100, 100, 400, 0); // Arguments are: starting X, starting Y, length, color
    displayCurrentAction("Drawing one vertical line");
    display.display();
    delay(DELAY_MS);

    // Now, let' make a grid using only horizontal and vertical lines in random colors!
    display.clearDisplay();
    for (int i = 0; i < 800; i += 8)
    {
        display.drawFastVLine(i, 0, 600, (i / 8) & 0x0F);
    }
    for (int i = 0; i < 600; i += 4)
    {
        display.drawFastHLine(0, i, 800, (i / 8) & 0x0F);
    }
    displayCurrentAction("Drawing a grid using horizontal and vertical lines in different colors");
    display.display();
    delay(DELAY_MS);

    // Draw rectangle at X = 200, Y = 200 and size of 400x300 pixels
    display.clearDisplay();
    display.drawRect(200, 200, 400, 300, 0); // Arguments are: start X, start Y, size X, size Y, color
    displayCurrentAction("Drawing rectangle");
    display.display();
    delay(DELAY_MS);

    // Draw rectangles on random location, size 100x150 pixels in random color
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.drawRect(random(0, 799), random(0, 599), 100, 150, random(0, 7));
    }
    displayCurrentAction("Drawing many rectangles in random colors");
    display.display();
    delay(DELAY_MS);

    // Draw filled black rectangle at X = 200, Y = 200, size of 400x300 pixels in gray color
    display.clearDisplay();
    display.fillRect(200, 200, 400, 300, 4); // Arguments are: start X, start Y, size X, size Y, color
    displayCurrentAction("Drawing gray rectangle");
    display.display();
    delay(DELAY_MS);

    // Draw filled random colored rectangles on random location, size of 30x30 pixels in radnom color
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.fillRect(random(0, 799), random(0, 599), 30, 30, random(0, 7));
    }
    displayCurrentAction("Drawing many filled rectangles randomly in random colors");
    display.display();
    delay(DELAY_MS);

    // Draw circle at center of a screen with radius of 75 pixels
    display.clearDisplay();
    display.drawCircle(400, 300, 75, 0); // Arguments are: start X, start Y, radius, color
    displayCurrentAction("Drawing a circle");
    display.display();
    delay(DELAY_MS);

    // Draw some random colored circles at random location with radius of 25 pixels in random color
    display.clearDisplay();
    for (int i = 0; i < 40; i++)
    {
        display.drawCircle(random(0, 799), random(0, 599), 25, random(0, 7));
    }
    displayCurrentAction("Drawing many circles randomly in random colors");
    display.display();
    delay(DELAY_MS);

    // Draw black filled circle at center of a screen with radius of 75 pixels
    display.clearDisplay();
    display.fillCircle(400, 300, 75, 0); // Arguments are: start X, start Y, radius, color
    displayCurrentAction("Drawing black-filled circle");
    display.display();
    delay(DELAY_MS);

    // Draw some random colored filled circles at random location with radius of 15 pixels
    display.clearDisplay();
    for (int i = 0; i < 40; i++)
    {
        display.fillCircle(random(0, 799), random(0, 599), 15, random(0, 7));
    }
    displayCurrentAction("Drawing many filled circles randomly in random colors");
    display.display(); // To show stuff on screen, you always need to call display.display();
    delay(DELAY_MS);

    // Draw rounded rectangle at X = 200, Y = 200 and size of 400x300 pixels and radius of 10 pixels
    display.clearDisplay();
    display.drawRoundRect(200, 200, 400, 300, 10, 0); // Arguments are: start X, start Y, size X, size Y, radius, color
    displayCurrentAction("Drawing rectangle with rounded edges");
    display.display();
    delay(DELAY_MS);

    // Draw rounded rectangles on random location, size 100x150 pixels, radius of 5 pixels in radnom color
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.drawRoundRect(random(0, 799), random(0, 599), 100, 150, 5, random(0, 7));
    }
    displayCurrentAction("Drawing many rounded edges rectangles");
    display.display();
    delay(DELAY_MS);

    // Draw filled random colored rectangle at X = 200, Y = 200, size of 400x300 pixels and radius of 10 pixels
    display.clearDisplay();
    display.fillRoundRect(200, 200, 400, 300, 10, 0); // Arguments are: start X, start Y, size X, size Y, radius, color
    displayCurrentAction("Drawing filled rectangle with rounded edges");
    display.display();
    delay(DELAY_MS);

    // Draw filled random colored rectangle on random location, size of 30x30 pixels, radius of 3 pixels in radnom color
    display.clearDisplay();
    for (int i = 0; i < 50; i++)
    {
        display.fillRoundRect(random(0, 799), random(0, 599), 30, 30, 3, random(0, 7));
    }
    displayCurrentAction("Drawing many filled rectangle with rounded edges in random colors");
    display.display();
    delay(DELAY_MS);

    // Draw simple triangle
    display.clearDisplay();
    display.drawTriangle(250, 400, 550, 400, 400, 100, 0); // Arguments are: X1, Y1, X2, Y2, X3, Y3, color
    display.display();
    delay(DELAY_MS);

    // Draw filled triangle inside simple triangle (so no display.clearDisplay() this time)
    display.fillTriangle(300, 350, 500, 350, 400, 150, 0); // Arguments are: X1, Y1, X2, Y2, X3, Y3, color
    displayCurrentAction("Drawing filled triangle inside exsisting one");
    display.display();
    delay(DELAY_MS);

    // Display some grayscale image on screen. We are going to display e-radionica logo on display at location X = 100,
    // Y = 100 Image size is 500x332 pixels.
    display.clearDisplay();
    display.drawImage(picture1, 100, 100, 500,
                      332); // Arguments are: array variable name, start X, start Y,  size X, size Y
    displayCurrentAction("Drawing a bitmap image");
    display.display();
    delay(DELAY_MS);

    // Write some text on screen with different sizes and color
    display.clearDisplay();
    for (int i = 0; i < 6; i++)
    {
        display.setTextColor(i);
        display.setTextSize(i +
                            1); // textSize parameter starts at 0 and goes up to 10 (larger won't fit Inkplate 6 screen)
        display.setCursor(200, (i * i * 8)); // setCursor works as same as on LCD displays - sets "the cursor" at the
                                             // place you want to write someting next
        display.print("INKPLATE6!");         // The actual text you want to show on e-paper as String
    }
    displayCurrentAction("Text in different sizes and shadings");
    display.display(); // To show stuff on screen, you always need to call display.display();
    delay(DELAY_MS);

    // Write same text on different location, but now invert colors (text is white, text background is black)
    display.setTextColor(7, 0); // First argument is text color, while second argument is background color. In
                                // greyscale,
    for (int i = 0; i < 6; i++)
    { // you are able to choose from 8 different colors (0-7)
        display.setTextSize(i + 1);
        display.setCursor(200, 300 + (i * i * 8));
        display.print("INKPLATE6!");
    }
    display.display();
    delay(DELAY_MS);

    // Draws an elipse with x radius, y radius, center x, center y and color
    display.clearDisplay();
    display.drawElipse(100, 200, 400, 300, 0);
    displayCurrentAction("Drawing an elipse");
    display.display();

    delay(DELAY_MS);

    // Fills an elipse with x radius, y radius, center x, center y and color
    display.clearDisplay();
    display.fillElipse(100, 200, 400, 300, 0);
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
        xt[i] = random(100, 700);
        yt[i] = random(100, 500);
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
    display.drawPolygon(xt, yt, n, 0);
    displayCurrentAction("Drawing a polygon");
    display.display();

    delay(DELAY_MS);

    // Fills a polygon, from x and y coordinate arrays of n points in color c,
    // Points need to be counter clockwise sorted
    // Method can be quite slow for now, probably will improve
    display.clearDisplay();
    display.fillPolygon(xt, yt, n, 0);
    displayCurrentAction("Drawing a filled polygon");
    display.display();

    delay(DELAY_MS);

    // Write text and rotate it by 90 deg. forever
    int r = 0;
    display.setTextSize(8);
    display.setTextColor(7, 0);
    while (true)
    {
        display.setCursor(100, 100);
        display.clearDisplay();
        display.setRotation(
            r); // Set rotation will sent rotation for the entire display, so you can use it sideways or upside-down
        display.print("INKPLATE6");
        display.display();
        r++;
        delay(DELAY_MS);
    }

    // Did you know that you can change between BW and greyscale mode anytime?
    // Just call Inkplate display(INKPLATE_1BIT) or Inkplate display(INKPLATE_3BIT)
}

// Small function that will write on the screen what function is currently in demonstration.
void displayCurrentAction(String text)
{
    display.setTextSize(2);
    display.setCursor(2, 580);
    display.print(text);
}