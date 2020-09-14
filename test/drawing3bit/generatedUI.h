#include "Arduino.h"
#include "Inkplate.h"
#include "../res/Fonts/FreeSansBold24pt7b.h"

extern Inkplate display;

String text0_content = "Hello there!";
int text0_cursor_x = 15;
int text0_cursor_y = 33;
const GFXfont *text0_font = &FreeSansBold24pt7b;

int pixel0_x = 17;
int pixel0_y = 56;
int pixel0_color = 0;

int line0_start_x = 17;
int line0_start_y = 75;
int line0_end_x = 171;
int line0_end_y = 72;
int line0_color = 0;
int line0_thickness = 1;
int line0_gradient = 0;

int rect0_a_x = 18;
int rect0_a_y = 91;
int rect0_b_x = 170;
int rect0_b_y = 136;
int rect0_fill = -1;
int rect0_radius = -1;
int rect0_color = 0;

int rect1_a_x = 20;
int rect1_a_y = 153;
int rect1_b_x = 171;
int rect1_b_y = 201;
int rect1_fill = -1;
int rect1_radius = 15;
int rect1_color = 0;

int rect2_a_x = 22;
int rect2_a_y = 220;
int rect2_b_x = 174;
int rect2_b_y = 269;
int rect2_fill = 1;
int rect2_radius = -1;
int rect2_color = 3;

int rect3_a_x = 22;
int rect3_a_y = 290;
int rect3_b_x = 175;
int rect3_b_y = 336;
int rect3_fill = 1;
int rect3_radius = 20;
int rect3_color = 2;

int line1_start_x = 197;
int line1_start_y = 10;
int line1_end_x = 200;
int line1_end_y = 334;
int line1_color = 4;
int line1_thickness = 5;
int line1_gradient = 0;

int line2_start_x = 222;
int line2_start_y = 12;
int line2_end_x = 228;
int line2_end_y = 410;
int line2_color = 0;
int line2_thickness = 10;
int line2_gradient = 7;

int circle0_center_x = 110;
int circle0_center_y = 455;
int circle0_fill = -1;
int circle0_radius = 100;
int circle0_color = 0;

int circle1_center_x = 109;
int circle1_center_y = 454;
int circle1_fill = 1;
int circle1_radius = 50;
int circle1_color = 3;

int triangle0_a_x = 226;
int triangle0_a_y = 424;
int triangle0_b_x = 361;
int triangle0_b_y = 491;
int triangle0_c_x = 228;
int triangle0_c_y = 581;
int triangle0_fill = 1;
int triangle0_radius = -1;
int triangle0_color = 4;

int triangle1_a_x = 257;
int triangle1_a_y = 409;
int triangle1_b_x = 374;
int triangle1_b_y = 479;
int triangle1_c_x = 252;
int triangle1_c_y = 194;
int triangle1_fill = -1;
int triangle1_radius = -1;
int triangle1_color = 0;

int digital_clock0_h = 9;
int digital_clock0_m = 41;
int digital_clock0_location_x = 248;
int digital_clock0_location_y = 12;
int digital_clock0_size = 64;
int digital_clock0_bitmask[] = {119, 48, 93, 121, 58, 107, 111, 49, 127, 59};
int digital_clock0_triangleX[] = {83, 101, 108, 101, 108, 277, 101, 108, 277, 257, 277, 108, 257, 277, 286, 76, 60, 98, 60, 98, 80, 80, 39, 60, 80, 39, 55, 31, 55, 73, 31, 73, 52, 31, 9, 52, 9, 52, 20, 61, 86, 80, 86, 80, 233, 233, 227, 80, 233, 227, 252, 260, 292, 305, 305, 260, 240, 305, 281, 240, 240, 281, 260, 259, 234, 276, 234, 276, 256, 256, 214, 234, 214, 256, 237, 38, 27, 60, 38, 60, 207, 207, 38, 212, 212, 207, 230};
int digital_clock0_triangleY[] = {30, 13, 60, 13, 60, 14, 13, 60, 14, 57, 14, 60, 57, 14, 29, 36, 47, 61, 47, 61, 198, 198, 201, 47, 198, 201, 219, 252, 232, 253, 252, 253, 390, 252, 406, 390, 406, 390, 416, 227, 202, 249, 202, 249, 203, 203, 247, 249, 203, 247, 224, 60, 35, 49, 49, 60, 200, 50, 201, 200, 200, 201, 220, 231, 252, 252, 252, 252, 403, 403, 390, 252, 390, 403, 415, 439, 424, 392, 439, 392, 394, 394, 439, 439, 439, 394, 424};
int digital_clock0_maxX = 310;
int digital_clock0_maxY = 440;

int widget1_h = 9;
int widget1_m = 41;
int widget1_center_x = 290;
int widget1_center_y = 126;
int widget1_size = 64;
int widget1_r0 = (double)widget1_size / 2 * 0.55;
int widget1_r1 = (double)widget1_size / 2 * 0.65;
int widget1_r2 = (double)widget1_size / 2 * 0.9;
int widget1_r3 = (double)widget1_size / 2 * 1.0;

int widget2_h = 9;
int widget2_m = 41;
int widget2_center_x = 386;
int widget2_center_y = 231;
int widget2_size = 151;
int widget2_r0 = (double)widget2_size / 2 * 0.55;
int widget2_r1 = (double)widget2_size / 2 * 0.65;
int widget2_r2 = (double)widget2_size / 2 * 0.9;
int widget2_r3 = (double)widget2_size / 2 * 1.0;

void mainDraw()
{
    display.setFont(text0_font);
    display.setTextColor(0, 7);
    display.setTextSize(1);
    display.setCursor(text0_cursor_x, text0_cursor_y);
    display.print(text0_content);

    display.drawPixel(pixel0_x, pixel0_y, pixel0_color);

    if (line0_gradient <= line0_color && line0_thickness == 1)
        display.drawLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color);
    else if (line0_gradient <= line0_color && line0_thickness != 1)
        display.drawThickLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color, line0_thickness);
    else if (line0_gradient > line0_color && line0_thickness == 1)
        display.drawGradientLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color, line0_gradient, 1);
    else if (line0_gradient > line0_color && line0_thickness != 1)
        display.drawGradientLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color, line0_gradient, line0_thickness);

    if (rect0_radius != -1 && rect0_fill != -1)
        display.fillRoundRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_radius, rect0_color);
    else if (rect0_radius != -1 && rect0_fill == -1)
        display.drawRoundRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_radius, rect0_color);
    else if (rect0_radius == -1 && rect0_fill != -1)
        display.fillRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_color);
    else if (rect0_radius == -1 && rect0_fill == -1)
        display.drawRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_color);

    if (rect1_radius != -1 && rect1_fill != -1)
        display.fillRoundRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_radius, rect1_color);
    else if (rect1_radius != -1 && rect1_fill == -1)
        display.drawRoundRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_radius, rect1_color);
    else if (rect1_radius == -1 && rect1_fill != -1)
        display.fillRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_color);
    else if (rect1_radius == -1 && rect1_fill == -1)
        display.drawRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_color);

    if (rect2_radius != -1 && rect2_fill != -1)
        display.fillRoundRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_radius, rect2_color);
    else if (rect2_radius != -1 && rect2_fill == -1)
        display.drawRoundRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_radius, rect2_color);
    else if (rect2_radius == -1 && rect2_fill != -1)
        display.fillRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_color);
    else if (rect2_radius == -1 && rect2_fill == -1)
        display.drawRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_color);

    if (rect3_radius != -1 && rect3_fill != -1)
        display.fillRoundRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_radius, rect3_color);
    else if (rect3_radius != -1 && rect3_fill == -1)
        display.drawRoundRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_radius, rect3_color);
    else if (rect3_radius == -1 && rect3_fill != -1)
        display.fillRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_color);
    else if (rect3_radius == -1 && rect3_fill == -1)
        display.drawRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_color);

    if (line1_gradient <= line1_color && line1_thickness == 1)
        display.drawLine(line1_start_x, line1_start_y, line1_end_x, line1_end_y, line1_color);
    else if (line1_gradient <= line1_color && line1_thickness != 1)
        display.drawThickLine(line1_start_x, line1_start_y, line1_end_x, line1_end_y, line1_color, line1_thickness);
    else if (line1_gradient > line1_color && line1_thickness == 1)
        display.drawGradientLine(line1_start_x, line1_start_y, line1_end_x, line1_end_y, line1_color, line1_gradient, 1);
    else if (line1_gradient > line1_color && line1_thickness != 1)
        display.drawGradientLine(line1_start_x, line1_start_y, line1_end_x, line1_end_y, line1_color, line1_gradient, line1_thickness);

    if (line2_gradient <= line2_color && line2_thickness == 1)
        display.drawLine(line2_start_x, line2_start_y, line2_end_x, line2_end_y, line2_color);
    else if (line2_gradient <= line2_color && line2_thickness != 1)
        display.drawThickLine(line2_start_x, line2_start_y, line2_end_x, line2_end_y, line2_color, line2_thickness);
    else if (line2_gradient > line2_color && line2_thickness == 1)
        display.drawGradientLine(line2_start_x, line2_start_y, line2_end_x, line2_end_y, line2_color, line2_gradient, 1);
    else if (line2_gradient > line2_color && line2_thickness != 1)
        display.drawGradientLine(line2_start_x, line2_start_y, line2_end_x, line2_end_y, line2_color, line2_gradient, line2_thickness);

    if (circle0_fill != -1)
        display.fillCircle(circle0_center_x, circle0_center_y, circle0_radius, circle0_color);
    else
        display.drawCircle(circle0_center_x, circle0_center_y, circle0_radius, circle0_color);

    if (circle1_fill != -1)
        display.fillCircle(circle1_center_x, circle1_center_y, circle1_radius, circle1_color);
    else
        display.drawCircle(circle1_center_x, circle1_center_y, circle1_radius, circle1_color);

    if (triangle0_fill != -1)
        display.fillTriangle(triangle0_a_x, triangle0_a_y, triangle0_b_x, triangle0_b_y, triangle0_c_x, triangle0_c_y, triangle0_color);
    else
        display.drawTriangle(triangle0_a_x, triangle0_a_y, triangle0_b_x, triangle0_b_y, triangle0_c_x, triangle0_c_y, triangle0_color);

    if (triangle1_fill != -1)
        display.fillTriangle(triangle1_a_x, triangle1_a_y, triangle1_b_x, triangle1_b_y, triangle1_c_x, triangle1_c_y, triangle1_color);
    else
        display.drawTriangle(triangle1_a_x, triangle1_a_y, triangle1_b_x, triangle1_b_y, triangle1_c_x, triangle1_c_y, triangle1_color);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < sizeof(digital_clock0_triangleX) / sizeof(digital_clock0_triangleX[0]); j += 3)
        {
            int temp[4] = {digital_clock0_h / 10 % 10, digital_clock0_h % 10, digital_clock0_m / 10 % 10, digital_clock0_m % 10};
            int b = digital_clock0_bitmask[temp[i]];
            if (b & (1 << ((j - 1) / (3 * 4))))
            {
                display.fillTriangle(
                    (int)((float)i * (float)digital_clock0_maxX / (float)digital_clock0_maxY * (float)digital_clock0_size * 1.1 + (float)digital_clock0_location_x + (float)digital_clock0_maxX / (float)digital_clock0_maxY * (float)digital_clock0_size * (float)digital_clock0_triangleX[j + 0] / (float)digital_clock0_maxX),
                    (int)((float)digital_clock0_location_y + (float)digital_clock0_size * (float)digital_clock0_triangleY[j + 0] / (float)digital_clock0_maxY),

                    (int)((float)i * (float)digital_clock0_maxX / (float)digital_clock0_maxY * (float)digital_clock0_size * 1.1 + (float)digital_clock0_location_x + (float)digital_clock0_maxX / (float)digital_clock0_maxY * (float)digital_clock0_size * (float)digital_clock0_triangleX[j + 1] / (float)digital_clock0_maxX),
                    (int)((float)digital_clock0_location_y + (float)digital_clock0_size * (float)digital_clock0_triangleY[j + 1] / (float)digital_clock0_maxY),

                    (int)((float)i * (float)digital_clock0_maxX / (float)digital_clock0_maxY * (float)digital_clock0_size * 1.1 + (float)digital_clock0_location_x + (float)digital_clock0_maxX / (float)digital_clock0_maxY * (float)digital_clock0_size * (float)digital_clock0_triangleX[j + 2] / (float)digital_clock0_maxX),
                    (int)((float)digital_clock0_location_y + (float)digital_clock0_size * (float)digital_clock0_triangleY[j + 2] / (float)digital_clock0_maxY),

                    0);
            }
        }
    }

    int digital_clock0_r = 0.05 * (float)digital_clock0_size;

    display.fillCircle((int)((float)digital_clock0_location_x + 4.0 * (float)digital_clock0_maxX / (float)digital_clock0_maxY * (float)digital_clock0_size * 1.075 / 2.0), (int)((float)digital_clock0_location_y + (float)digital_clock0_size * 0.4), digital_clock0_r, 0);
    display.fillCircle((int)((float)digital_clock0_location_x + 4.0 * (float)digital_clock0_maxX / (float)digital_clock0_maxY * (float)digital_clock0_size * 1.075 / 2.0), (int)((float)digital_clock0_location_y + (float)digital_clock0_size * 0.6), digital_clock0_r, 0);
    for (int i = 0; i < 60; ++i)
    {
        if (i % 5 == 0)
            display.drawThickLine(widget1_center_x + widget1_r1 * cos((double)i / 60.0 * 2.0 * 3.14159265),
                                  widget1_center_y + widget1_r1 * sin((double)i / 60.0 * 2.0 * 3.14159265),
                                  widget1_center_x + widget1_r3 * cos((double)i / 60.0 * 2.0 * 3.14159265),
                                  widget1_center_y + widget1_r3 * sin((double)i / 60.0 * 2.0 * 3.14159265), 0, 3);
        else if (widget1_size > 150)
            display.drawLine(widget1_center_x + widget1_r1 * cos((double)i / 60.0 * 2.0 * 3.14159265),
                             widget1_center_y + widget1_r1 * sin((double)i / 60.0 * 2.0 * 3.14159265),
                             widget1_center_x + widget1_r2 * cos((double)i / 60.0 * 2.0 * 3.14159265),
                             widget1_center_y + widget1_r2 * sin((double)i / 60.0 * 2.0 * 3.14159265), 2);
    }
    display.drawThickLine(widget1_center_x,
                          widget1_center_y,
                          widget1_center_x + widget1_r0 * cos((double)(widget1_h - 3.0 + widget1_m / 60.0) / 12.0 * 2.0 * 3.14159265),
                          widget1_center_y + widget1_r0 * sin((double)(widget1_h - 3.0 + widget1_m / 60.0) / 12.0 * 2.0 * 3.14159265), 2, 2);

    display.drawThickLine(widget1_center_x,
                          widget1_center_y,
                          widget1_center_x + widget1_r2 * cos((double)(widget1_m - 15.0) / 60.0 * 2.0 * 3.14159265),
                          widget1_center_y + widget1_r2 * sin((double)(widget1_m - 15.0) / 60.0 * 2.0 * 3.14159265), 2, 2);

    for (int i = 0; i < 60; ++i)
    {
        if (i % 5 == 0)
            display.drawThickLine(widget2_center_x + widget2_r1 * cos((double)i / 60.0 * 2.0 * 3.14159265),
                                  widget2_center_y + widget2_r1 * sin((double)i / 60.0 * 2.0 * 3.14159265),
                                  widget2_center_x + widget2_r3 * cos((double)i / 60.0 * 2.0 * 3.14159265),
                                  widget2_center_y + widget2_r3 * sin((double)i / 60.0 * 2.0 * 3.14159265), 0, 3);
        else if (widget2_size > 150)
            display.drawLine(widget2_center_x + widget2_r1 * cos((double)i / 60.0 * 2.0 * 3.14159265),
                             widget2_center_y + widget2_r1 * sin((double)i / 60.0 * 2.0 * 3.14159265),
                             widget2_center_x + widget2_r2 * cos((double)i / 60.0 * 2.0 * 3.14159265),
                             widget2_center_y + widget2_r2 * sin((double)i / 60.0 * 2.0 * 3.14159265), 2);
    }
    display.drawThickLine(widget2_center_x,
                          widget2_center_y,
                          widget2_center_x + widget2_r0 * cos((double)(widget2_h - 3.0 + widget2_m / 60.0) / 12.0 * 2.0 * 3.14159265),
                          widget2_center_y + widget2_r0 * sin((double)(widget2_h - 3.0 + widget2_m / 60.0) / 12.0 * 2.0 * 3.14159265), 2, 2);

    display.drawThickLine(widget2_center_x,
                          widget2_center_y,
                          widget2_center_x + widget2_r2 * cos((double)(widget2_m - 15.0) / 60.0 * 2.0 * 3.14159265),
                          widget2_center_y + widget2_r2 * sin((double)(widget2_m - 15.0) / 60.0 * 2.0 * 3.14159265), 2, 2);
}
