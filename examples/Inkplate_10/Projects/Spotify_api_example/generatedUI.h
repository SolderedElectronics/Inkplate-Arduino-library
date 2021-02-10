/*
generatedUI.cpp
Inkplate 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "Arduino.h"
#include "Fonts/Roboto_Condensed_24.h"
#include "Fonts/Roboto_Condensed_32.h"
#include "Inkplate.h"

// Not fully auto generated, tweaked a little for 1bit mode

extern Inkplate display;

int line0_start_x = 70;
int line0_start_y = 490;
int line0_end_x = 730;
int line0_end_y = 490;
int line0_color = BLACK;
int line0_thickness = 1;
int line0_gradient = 0;

int circle0_center_x = 70;
int circle0_center_y = 490;
int circle0_fill = 1;
int circle0_radius = 10;
int circle0_color = BLACK;

int circle1_center_x = 730;
int circle1_center_y = 490;
int circle1_fill = 1;
int circle1_radius = 10;
int circle1_color = BLACK;

int circle2_center_x = 70;
int circle2_center_y = 490;
int circle2_fill = -1;
int circle2_radius = 14;
int circle2_color = BLACK;

int digital_clock2_h = 9;
int digital_clock2_m = 41;
int digital_clock2_location_x = 35;
int digital_clock2_location_y = 444;
int digital_clock2_size = 32;
int digital_clock2_bitmask[] = {119, 48, 93, 121, 58, 107, 111, 49, 127, 59};
int digital_clock2_triangleX[] = {
    83,  101, 108, 101, 108, 277, 101, 108, 277, 257, 277, 108, 257, 277, 286, 76,  60,  98,  60,  98,  80,  80,
    39,  60,  80,  39,  55,  31,  55,  73,  31,  73,  52,  31,  9,   52,  9,   52,  20,  61,  86,  80,  86,  80,
    233, 233, 227, 80,  233, 227, 252, 260, 292, 305, 305, 260, 240, 305, 281, 240, 240, 281, 260, 259, 234, 276,
    234, 276, 256, 256, 214, 234, 214, 256, 237, 38,  27,  60,  38,  60,  207, 207, 38,  212, 212, 207, 230};
int digital_clock2_triangleY[] = {
    30,  13,  60,  13,  60,  14,  13,  60,  14,  57,  14,  60,  57,  14,  29,  36,  47,  61,  47,  61,  198, 198,
    201, 47,  198, 201, 219, 252, 232, 253, 252, 253, 390, 252, 406, 390, 406, 390, 416, 227, 202, 249, 202, 249,
    203, 203, 247, 249, 203, 247, 224, 60,  35,  49,  49,  60,  200, 50,  201, 200, 200, 201, 220, 231, 252, 252,
    252, 252, 403, 403, 390, 252, 390, 403, 415, 439, 424, 392, 439, 392, 394, 394, 439, 439, 439, 394, 424};
int digital_clock2_maxX = 310;
int digital_clock2_maxY = 440;

int digital_clock4_h = 9;
int digital_clock4_m = 41;
int digital_clock4_location_x = 675;
int digital_clock4_location_y = 21;
int digital_clock4_size = 32;
int digital_clock4_bitmask[] = {119, 48, 93, 121, 58, 107, 111, 49, 127, 59};
int digital_clock4_triangleX[] = {
    83,  101, 108, 101, 108, 277, 101, 108, 277, 257, 277, 108, 257, 277, 286, 76,  60,  98,  60,  98,  80,  80,
    39,  60,  80,  39,  55,  31,  55,  73,  31,  73,  52,  31,  9,   52,  9,   52,  20,  61,  86,  80,  86,  80,
    233, 233, 227, 80,  233, 227, 252, 260, 292, 305, 305, 260, 240, 305, 281, 240, 240, 281, 260, 259, 234, 276,
    234, 276, 256, 256, 214, 234, 214, 256, 237, 38,  27,  60,  38,  60,  207, 207, 38,  212, 212, 207, 230};
int digital_clock4_triangleY[] = {
    30,  13,  60,  13,  60,  14,  13,  60,  14,  57,  14,  60,  57,  14,  29,  36,  47,  61,  47,  61,  198, 198,
    201, 47,  198, 201, 219, 252, 232, 253, 252, 253, 390, 252, 406, 390, 406, 390, 416, 227, 202, 249, 202, 249,
    203, 203, 247, 249, 203, 247, 224, 60,  35,  49,  49,  60,  200, 50,  201, 200, 200, 201, 220, 231, 252, 252,
    252, 252, 403, 403, 390, 252, 390, 403, 415, 439, 424, 392, 439, 392, 394, 394, 439, 439, 439, 394, 424};
int digital_clock4_maxX = 310;
int digital_clock4_maxY = 440;

int circle3_center_x = 400;
int circle3_center_y = 560;
int circle3_fill = -1;
int circle3_radius = 30;
int circle3_color = BLACK;

int line1_start_x = 390;
int line1_start_y = 570;
int line1_end_x = 390;
int line1_end_y = 550;
int line1_color = BLACK;
int line1_thickness = 4;
int line1_gradient = 0;

int line2_start_x = 410;
int line2_start_y = 550;
int line2_end_x = 410;
int line2_end_y = 570;
int line2_color = BLACK;
int line2_thickness = 4;
int line2_gradient = 0;

char text1_content[128] = "Rick Astley ";
int text1_cursor_x = 325;
int text1_cursor_y = 49;
const GFXfont *text1_font = &Roboto_Condensed_32;

char text2_content[128] = "Never Gonna Give You Up";
int text2_cursor_x = 254;
int text2_cursor_y = 87;
const GFXfont *text2_font = &Roboto_Condensed_32;

char text3_content[128] = "from Whenever You Need Somebody";
int text3_cursor_x = 305;
int text3_cursor_y = 424;
const GFXfont *text3_font = &Roboto_Condensed_24;

int digital_clock5_h = 9;
int digital_clock5_m = 41;
int digital_clock5_location_x = 675;
int digital_clock5_location_y = 444;
int digital_clock5_size = 32;
int digital_clock5_bitmask[] = {119, 48, 93, 121, 58, 107, 111, 49, 127, 59};
int digital_clock5_triangleX[] = {
    83,  101, 108, 101, 108, 277, 101, 108, 277, 257, 277, 108, 257, 277, 286, 76,  60,  98,  60,  98,  80,  80,
    39,  60,  80,  39,  55,  31,  55,  73,  31,  73,  52,  31,  9,   52,  9,   52,  20,  61,  86,  80,  86,  80,
    233, 233, 227, 80,  233, 227, 252, 260, 292, 305, 305, 260, 240, 305, 281, 240, 240, 281, 260, 259, 234, 276,
    234, 276, 256, 256, 214, 234, 214, 256, 237, 38,  27,  60,  38,  60,  207, 207, 38,  212, 212, 207, 230};
int digital_clock5_triangleY[] = {
    30,  13,  60,  13,  60,  14,  13,  60,  14,  57,  14,  60,  57,  14,  29,  36,  47,  61,  47,  61,  198, 198,
    201, 47,  198, 201, 219, 252, 232, 253, 252, 253, 390, 252, 406, 390, 406, 390, 416, 227, 202, 249, 202, 249,
    203, 203, 247, 249, 203, 247, 224, 60,  35,  49,  49,  60,  200, 50,  201, 200, 200, 201, 220, 231, 252, 252,
    252, 252, 403, 403, 390, 252, 390, 403, 415, 439, 424, 392, 439, 392, 394, 394, 439, 439, 439, 394, 424};
int digital_clock5_maxX = 310;
int digital_clock5_maxY = 440;

int triangle0_a_x = 500;
int triangle0_a_y = 570;
int triangle0_b_x = 530;
int triangle0_b_y = 560;
int triangle0_c_x = 500;
int triangle0_c_y = 550;
int triangle0_fill = 1;
int triangle0_radius = -1;
int triangle0_color = BLACK;

int line3_start_x = 528;
int line3_start_y = 550;
int line3_end_x = 528;
int line3_end_y = 570;
int line3_color = BLACK;
int line3_thickness = 4;
int line3_gradient = 0;

int triangle1_a_x = 270;
int triangle1_a_y = 560;
int triangle1_b_x = 300;
int triangle1_b_y = 550;
int triangle1_c_x = 300;
int triangle1_c_y = 570;
int triangle1_fill = 1;
int triangle1_radius = -1;
int triangle1_color = BLACK;

int line4_start_x = 272;
int line4_start_y = 550;
int line4_end_x = 272;
int line4_end_y = 570;
int line4_color = BLACK;
int line4_thickness = 4;
int line4_gradient = 0;

int triangle2_a_x = 390;
int triangle2_a_y = 573;
int triangle2_b_x = 417;
int triangle2_b_y = 560;
int triangle2_c_x = 390;
int triangle2_c_y = 548;
int triangle2_fill = 1;
int triangle2_radius = -1;
int triangle2_color = BLACK;

void mainDraw()
{
    if (line0_gradient <= line0_color && line0_thickness == 1)
        display.drawLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color);
    else if (line0_gradient <= line0_color && line0_thickness != 1)
        display.drawThickLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color, line0_thickness);
    else if (line0_gradient > line0_color && line0_thickness == 1)
        display.drawGradientLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color, line0_gradient,
                                 1);
    else if (line0_gradient > line0_color && line0_thickness != 1)
        display.drawGradientLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color, line0_gradient,
                                 line0_thickness);

    if (circle0_fill != -1)
        display.fillCircle(circle0_center_x, circle0_center_y, circle0_radius, circle0_color);
    else
        display.drawCircle(circle0_center_x, circle0_center_y, circle0_radius, circle0_color);

    if (circle1_fill != -1)
        display.fillCircle(circle1_center_x, circle1_center_y, circle1_radius, circle1_color);
    else
        display.drawCircle(circle1_center_x, circle1_center_y, circle1_radius, circle1_color);

    if (circle2_fill != -1)
        display.fillCircle(circle2_center_x, circle2_center_y, circle2_radius, circle2_color);
    else
        display.drawCircle(circle2_center_x, circle2_center_y, circle2_radius, circle2_color);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < sizeof(digital_clock2_triangleX) / sizeof(digital_clock2_triangleX[0]); j += 3)
        {
            int temp[4] = {digital_clock2_h / 10 % 10, digital_clock2_h % 10, digital_clock2_m / 10 % 10,
                           digital_clock2_m % 10};
            int b = digital_clock2_bitmask[temp[i]];
            if (b & (1 << ((j - 1) / (3 * 4))))
            {
                display.fillTriangle(
                    (int)((float)i * (float)digital_clock2_maxX / (float)digital_clock2_maxY *
                              (float)digital_clock2_size * 1.1 +
                          (float)digital_clock2_location_x +
                          (float)digital_clock2_maxX / (float)digital_clock2_maxY * (float)digital_clock2_size *
                              (float)digital_clock2_triangleX[j + 0] / (float)digital_clock2_maxX),
                    (int)((float)digital_clock2_location_y + (float)digital_clock2_size *
                                                                 (float)digital_clock2_triangleY[j + 0] /
                                                                 (float)digital_clock2_maxY),

                    (int)((float)i * (float)digital_clock2_maxX / (float)digital_clock2_maxY *
                              (float)digital_clock2_size * 1.1 +
                          (float)digital_clock2_location_x +
                          (float)digital_clock2_maxX / (float)digital_clock2_maxY * (float)digital_clock2_size *
                              (float)digital_clock2_triangleX[j + 1] / (float)digital_clock2_maxX),
                    (int)((float)digital_clock2_location_y + (float)digital_clock2_size *
                                                                 (float)digital_clock2_triangleY[j + 1] /
                                                                 (float)digital_clock2_maxY),

                    (int)((float)i * (float)digital_clock2_maxX / (float)digital_clock2_maxY *
                              (float)digital_clock2_size * 1.1 +
                          (float)digital_clock2_location_x +
                          (float)digital_clock2_maxX / (float)digital_clock2_maxY * (float)digital_clock2_size *
                              (float)digital_clock2_triangleX[j + 2] / (float)digital_clock2_maxX),
                    (int)((float)digital_clock2_location_y + (float)digital_clock2_size *
                                                                 (float)digital_clock2_triangleY[j + 2] /
                                                                 (float)digital_clock2_maxY),

                    BLACK);
            }
        }
    }

    int digital_clock2_r = 0.05 * (float)digital_clock2_size;

    display.fillCircle(
        (int)((float)digital_clock2_location_x +
              4.0 * (float)digital_clock2_maxX / (float)digital_clock2_maxY * (float)digital_clock2_size * 1.075 / 2.0),
        (int)((float)digital_clock2_location_y + (float)digital_clock2_size * 0.4), digital_clock2_r, BLACK);
    display.fillCircle(
        (int)((float)digital_clock2_location_x +
              4.0 * (float)digital_clock2_maxX / (float)digital_clock2_maxY * (float)digital_clock2_size * 1.075 / 2.0),
        (int)((float)digital_clock2_location_y + (float)digital_clock2_size * 0.6), digital_clock2_r, BLACK);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < sizeof(digital_clock4_triangleX) / sizeof(digital_clock4_triangleX[0]); j += 3)
        {
            int temp[4] = {digital_clock4_h / 10 % 10, digital_clock4_h % 10, digital_clock4_m / 10 % 10,
                           digital_clock4_m % 10};
            int b = digital_clock4_bitmask[temp[i]];
            if (b & (1 << ((j - 1) / (3 * 4))))
            {
                display.fillTriangle(
                    (int)((float)i * (float)digital_clock4_maxX / (float)digital_clock4_maxY *
                              (float)digital_clock4_size * 1.1 +
                          (float)digital_clock4_location_x +
                          (float)digital_clock4_maxX / (float)digital_clock4_maxY * (float)digital_clock4_size *
                              (float)digital_clock4_triangleX[j + 0] / (float)digital_clock4_maxX),
                    (int)((float)digital_clock4_location_y + (float)digital_clock4_size *
                                                                 (float)digital_clock4_triangleY[j + 0] /
                                                                 (float)digital_clock4_maxY),

                    (int)((float)i * (float)digital_clock4_maxX / (float)digital_clock4_maxY *
                              (float)digital_clock4_size * 1.1 +
                          (float)digital_clock4_location_x +
                          (float)digital_clock4_maxX / (float)digital_clock4_maxY * (float)digital_clock4_size *
                              (float)digital_clock4_triangleX[j + 1] / (float)digital_clock4_maxX),
                    (int)((float)digital_clock4_location_y + (float)digital_clock4_size *
                                                                 (float)digital_clock4_triangleY[j + 1] /
                                                                 (float)digital_clock4_maxY),

                    (int)((float)i * (float)digital_clock4_maxX / (float)digital_clock4_maxY *
                              (float)digital_clock4_size * 1.1 +
                          (float)digital_clock4_location_x +
                          (float)digital_clock4_maxX / (float)digital_clock4_maxY * (float)digital_clock4_size *
                              (float)digital_clock4_triangleX[j + 2] / (float)digital_clock4_maxX),
                    (int)((float)digital_clock4_location_y + (float)digital_clock4_size *
                                                                 (float)digital_clock4_triangleY[j + 2] /
                                                                 (float)digital_clock4_maxY),

                    BLACK);
            }
        }
    }

    int digital_clock4_r = 0.05 * (float)digital_clock4_size;

    display.fillCircle(
        (int)((float)digital_clock4_location_x +
              4.0 * (float)digital_clock4_maxX / (float)digital_clock4_maxY * (float)digital_clock4_size * 1.075 / 2.0),
        (int)((float)digital_clock4_location_y + (float)digital_clock4_size * 0.4), digital_clock4_r, BLACK);
    display.fillCircle(
        (int)((float)digital_clock4_location_x +
              4.0 * (float)digital_clock4_maxX / (float)digital_clock4_maxY * (float)digital_clock4_size * 1.075 / 2.0),
        (int)((float)digital_clock4_location_y + (float)digital_clock4_size * 0.6), digital_clock4_r, BLACK);

    if (circle3_fill != -1)
        display.fillCircle(circle3_center_x, circle3_center_y, circle3_radius, circle3_color);
    else
        display.drawCircle(circle3_center_x, circle3_center_y, circle3_radius, circle3_color);

    if (line1_color == BLACK)
    {
        if (line1_gradient <= line1_color && line1_thickness == 1)
            display.drawLine(line1_start_x, line1_start_y, line1_end_x, line1_end_y, line1_color);
        else if (line1_gradient <= line1_color && line1_thickness != 1)
            display.drawThickLine(line1_start_x, line1_start_y, line1_end_x, line1_end_y, line1_color, line1_thickness);
        else if (line1_gradient > line1_color && line1_thickness == 1)
            display.drawGradientLine(line1_start_x, line1_start_y, line1_end_x, line1_end_y, line1_color,
                                     line1_gradient, 1);
        else if (line1_gradient > line1_color && line1_thickness != 1)
            display.drawGradientLine(line1_start_x, line1_start_y, line1_end_x, line1_end_y, line1_color,
                                     line1_gradient, line1_thickness);
    }
    if (line2_color == BLACK)
    {
        if (line2_gradient <= line2_color && line2_thickness == 1)
            display.drawLine(line2_start_x, line2_start_y, line2_end_x, line2_end_y, line2_color);
        else if (line2_gradient <= line2_color && line2_thickness != 1)
            display.drawThickLine(line2_start_x, line2_start_y, line2_end_x, line2_end_y, line2_color, line2_thickness);
        else if (line2_gradient > line2_color && line2_thickness == 1)
            display.drawGradientLine(line2_start_x, line2_start_y, line2_end_x, line2_end_y, line2_color,
                                     line2_gradient, 1);
        else if (line2_gradient > line2_color && line2_thickness != 1)
            display.drawGradientLine(line2_start_x, line2_start_y, line2_end_x, line2_end_y, line2_color,
                                     line2_gradient, line2_thickness);
    }

    display.setFont(text1_font);
    display.setTextSize(1);
    display.setCursor(text1_cursor_x, text1_cursor_y);
    display.print(text1_content);

    display.setFont(text2_font);
    display.setTextSize(1);
    display.setCursor(text2_cursor_x, text2_cursor_y);
    display.print(text2_content);

    display.setFont(text3_font);
    display.setTextSize(1);
    display.setCursor(text3_cursor_x, text3_cursor_y);
    display.print(text3_content);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < sizeof(digital_clock5_triangleX) / sizeof(digital_clock5_triangleX[0]); j += 3)
        {
            int temp[4] = {digital_clock5_h / 10 % 10, digital_clock5_h % 10, digital_clock5_m / 10 % 10,
                           digital_clock5_m % 10};
            int b = digital_clock5_bitmask[temp[i]];
            if (b & (1 << ((j - 1) / (3 * 4))))
            {
                display.fillTriangle(
                    (int)((float)i * (float)digital_clock5_maxX / (float)digital_clock5_maxY *
                              (float)digital_clock5_size * 1.1 +
                          (float)digital_clock5_location_x +
                          (float)digital_clock5_maxX / (float)digital_clock5_maxY * (float)digital_clock5_size *
                              (float)digital_clock5_triangleX[j + 0] / (float)digital_clock5_maxX),
                    (int)((float)digital_clock5_location_y + (float)digital_clock5_size *
                                                                 (float)digital_clock5_triangleY[j + 0] /
                                                                 (float)digital_clock5_maxY),

                    (int)((float)i * (float)digital_clock5_maxX / (float)digital_clock5_maxY *
                              (float)digital_clock5_size * 1.1 +
                          (float)digital_clock5_location_x +
                          (float)digital_clock5_maxX / (float)digital_clock5_maxY * (float)digital_clock5_size *
                              (float)digital_clock5_triangleX[j + 1] / (float)digital_clock5_maxX),
                    (int)((float)digital_clock5_location_y + (float)digital_clock5_size *
                                                                 (float)digital_clock5_triangleY[j + 1] /
                                                                 (float)digital_clock5_maxY),

                    (int)((float)i * (float)digital_clock5_maxX / (float)digital_clock5_maxY *
                              (float)digital_clock5_size * 1.1 +
                          (float)digital_clock5_location_x +
                          (float)digital_clock5_maxX / (float)digital_clock5_maxY * (float)digital_clock5_size *
                              (float)digital_clock5_triangleX[j + 2] / (float)digital_clock5_maxX),
                    (int)((float)digital_clock5_location_y + (float)digital_clock5_size *
                                                                 (float)digital_clock5_triangleY[j + 2] /
                                                                 (float)digital_clock5_maxY),

                    BLACK);
            }
        }
    }

    int digital_clock5_r = 0.05 * (float)digital_clock5_size;

    display.fillCircle(
        (int)((float)digital_clock5_location_x +
              4.0 * (float)digital_clock5_maxX / (float)digital_clock5_maxY * (float)digital_clock5_size * 1.075 / 2.0),
        (int)((float)digital_clock5_location_y + (float)digital_clock5_size * 0.4), digital_clock5_r, BLACK);
    display.fillCircle(
        (int)((float)digital_clock5_location_x +
              4.0 * (float)digital_clock5_maxX / (float)digital_clock5_maxY * (float)digital_clock5_size * 1.075 / 2.0),
        (int)((float)digital_clock5_location_y + (float)digital_clock5_size * 0.6), digital_clock5_r, BLACK);
    if (triangle0_fill != -1)
        display.fillTriangle(triangle0_a_x, triangle0_a_y, triangle0_b_x, triangle0_b_y, triangle0_c_x, triangle0_c_y,
                             triangle0_color);
    else
        display.drawTriangle(triangle0_a_x, triangle0_a_y, triangle0_b_x, triangle0_b_y, triangle0_c_x, triangle0_c_y,
                             triangle0_color);

    if (line3_gradient <= line3_color && line3_thickness == 1)
        display.drawLine(line3_start_x, line3_start_y, line3_end_x, line3_end_y, line3_color);
    else if (line3_gradient <= line3_color && line3_thickness != 1)
        display.drawThickLine(line3_start_x, line3_start_y, line3_end_x, line3_end_y, line3_color, line3_thickness);
    else if (line3_gradient > line3_color && line3_thickness == 1)
        display.drawGradientLine(line3_start_x, line3_start_y, line3_end_x, line3_end_y, line3_color, line3_gradient,
                                 1);
    else if (line3_gradient > line3_color && line3_thickness != 1)
        display.drawGradientLine(line3_start_x, line3_start_y, line3_end_x, line3_end_y, line3_color, line3_gradient,
                                 line3_thickness);

    if (triangle1_fill != -1)
        display.fillTriangle(triangle1_a_x, triangle1_a_y, triangle1_b_x, triangle1_b_y, triangle1_c_x, triangle1_c_y,
                             triangle1_color);
    else
        display.drawTriangle(triangle1_a_x, triangle1_a_y, triangle1_b_x, triangle1_b_y, triangle1_c_x, triangle1_c_y,
                             triangle1_color);

    if (line4_gradient <= line4_color && line4_thickness == 1)
        display.drawLine(line4_start_x, line4_start_y, line4_end_x, line4_end_y, line4_color);
    else if (line4_gradient <= line4_color && line4_thickness != 1)
        display.drawThickLine(line4_start_x, line4_start_y, line4_end_x, line4_end_y, line4_color, line4_thickness);
    else if (line4_gradient > line4_color && line4_thickness == 1)
        display.drawGradientLine(line4_start_x, line4_start_y, line4_end_x, line4_end_y, line4_color, line4_gradient,
                                 1);
    else if (line4_gradient > line4_color && line4_thickness != 1)
        display.drawGradientLine(line4_start_x, line4_start_y, line4_end_x, line4_end_y, line4_color, line4_gradient,
                                 line4_thickness);

    if (triangle2_color == BLACK)
    {
        if (triangle2_fill != -1)
            display.fillTriangle(triangle2_a_x, triangle2_a_y, triangle2_b_x, triangle2_b_y, triangle2_c_x,
                                 triangle2_c_y, triangle2_color);
        else
            display.drawTriangle(triangle2_a_x, triangle2_a_y, triangle2_b_x, triangle2_b_y, triangle2_c_x,
                                 triangle2_c_y, triangle2_color);
    }
}
