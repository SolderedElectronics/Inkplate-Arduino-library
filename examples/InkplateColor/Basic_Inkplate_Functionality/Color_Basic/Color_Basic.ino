/*
   Inkplate_Maze_Color_basic sketch for e-radionica.com Inkplate 6COLOR
   Select "Inkplate 6COLOR(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6COLOR(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   Simple Inkplate example showing drawing functionalities of the Inkplate 6COLOR library.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   8 July 2021 by e-radionica.com
*/

#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Inkplate color in the boards menu."
#endif

#include "Inkplate.h"
#include "logoImg.h"

Inkplate display;

void setup()
{
    Serial.begin(115200);
    display.begin();
    display.clearDisplay();
    display.display();

    //Draw many rectangles
    display.fillRect(0, 0, 50, 50, INKPLATE_BLACK);
    display.fillRect(0, 50, 50, 50, INKPLATE_WHITE);
    display.fillRect(0, 100, 50, 50, INKPLATE_GREEN);
    display.fillRect(0, 150, 50, 50, INKPLATE_BLUE);
    display.fillRect(0, 200, 50, 50, INKPLATE_RED);
    display.fillRect(0, 250, 50, 50, INKPLATE_YELLOW);
    display.fillRect(0, 300, 50, 50, INKPLATE_ORANGE);

    display.drawRect(55, 0, 50, 50, INKPLATE_BLACK);
    display.drawRect(55, 50, 50, 50, INKPLATE_WHITE);
    display.drawRect(55, 100, 50, 50, INKPLATE_GREEN);
    display.drawRect(55, 150, 50, 50, INKPLATE_BLUE);
    display.drawRect(55, 200, 50, 50, INKPLATE_RED);
    display.drawRect(55, 250, 50, 50, INKPLATE_YELLOW);
    display.drawRect(55, 300, 50, 50, INKPLATE_ORANGE);

    //Draw many circles
    display.fillCircle(135, 25, 24, INKPLATE_BLACK);
    display.fillCircle(135, 75, 24, INKPLATE_WHITE);
    display.fillCircle(135, 125, 24, INKPLATE_GREEN);
    display.fillCircle(135, 175, 24, INKPLATE_BLUE);
    display.fillCircle(135, 225, 24, INKPLATE_RED);
    display.fillCircle(135, 275, 24, INKPLATE_YELLOW);
    display.fillCircle(135, 325, 24, INKPLATE_ORANGE);

    display.drawCircle(190, 25, 24, INKPLATE_BLACK);
    display.drawCircle(190, 75, 24, INKPLATE_WHITE);
    display.drawCircle(190, 125, 24, INKPLATE_GREEN);
    display.drawCircle(190, 175, 24, INKPLATE_BLUE);
    display.drawCircle(190, 225, 24, INKPLATE_RED);
    display.drawCircle(190, 275, 24, INKPLATE_YELLOW);
    display.drawCircle(190, 325, 24, INKPLATE_ORANGE);

    //Draw many triangles
    display.fillTriangle(210, 50, 260, 50, 235, 0, INKPLATE_BLACK);
    display.fillTriangle(210, 100, 260, 100, 235, 50, INKPLATE_WHITE);
    display.fillTriangle(210, 150, 260, 150, 235, 100, INKPLATE_GREEN);
    display.fillTriangle(210, 200, 260, 200, 235, 150, INKPLATE_BLUE);
    display.fillTriangle(210, 250, 260, 250, 235, 200, INKPLATE_RED);
    display.fillTriangle(210, 300, 260, 300, 235, 250, INKPLATE_YELLOW);
    display.fillTriangle(210, 350, 260, 350, 235, 300, INKPLATE_ORANGE);

    display.drawTriangle(210, 50, 260, 50, 235, 0, INKPLATE_BLACK);
    display.drawTriangle(210, 100, 260, 100, 235, 50, INKPLATE_WHITE);
    display.drawTriangle(210, 150, 260, 150, 235, 100, INKPLATE_GREEN);
    display.drawTriangle(210, 200, 260, 200, 235, 150, INKPLATE_BLUE);
    display.drawTriangle(210, 250, 260, 250, 235, 200, INKPLATE_RED);
    display.drawTriangle(210, 300, 260, 300, 235, 250, INKPLATE_YELLOW);
    display.drawTriangle(210, 350, 260, 350, 235, 300, INKPLATE_ORANGE);

    //Show some pretty text
    display.setTextColor(INKPLATE_BLACK);
    display.setCursor(265, 0);
    display.setTextSize(2);
    display.print("Welcome to Inkplate Color!");

    display.setTextColor(INKPLATE_WHITE);
    display.setCursor(265, 50);
    display.setTextSize(2);
    display.print("Welcome to Inkplate Color!");

    display.setTextColor(INKPLATE_GREEN);
    display.setCursor(265, 100);
    display.setTextSize(2);
    display.print("Welcome to Inkplate Color!");

    display.setTextColor(INKPLATE_BLUE);
    display.setCursor(265, 150);
    display.setTextSize(2);
    display.print("Welcome to Inkplate Color!");

    display.setTextColor(INKPLATE_RED);
    display.setCursor(265, 200);
    display.setTextSize(2);
    display.print("Welcome to Inkplate Color!");

    display.setTextColor(INKPLATE_YELLOW);
    display.setCursor(265, 250);
    display.setTextSize(2);
    display.print("Welcome to Inkplate Color!");

    display.setTextColor(INKPLATE_ORANGE);
    display.setCursor(265, 300);
    display.setTextSize(2);
    display.print("Welcome to Inkplate Color!");

    //Draw logo
    display.drawBitmap(0, 350, logo, logo_w, logo_h, INKPLATE_BLACK);
    display.drawBitmap(100, 350, logo, logo_w, logo_h, INKPLATE_WHITE);
    display.drawBitmap(200, 350, logo, logo_w, logo_h, INKPLATE_GREEN);
    display.drawBitmap(300, 350, logo, logo_w, logo_h, INKPLATE_BLUE);
    display.drawBitmap(400, 350, logo, logo_w, logo_h, INKPLATE_RED);
    display.drawBitmap(500, 350, logo, logo_w, logo_h, INKPLATE_YELLOW);

    display.display();
}

void loop()
{
}