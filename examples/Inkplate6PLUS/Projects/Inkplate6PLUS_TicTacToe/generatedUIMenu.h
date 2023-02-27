#include "Arduino.h"
#include "Fonts/FreeSerifBold24pt7b.h"
#include "Inkplate.h"

extern Inkplate display;

String text1_content = "Tic tac toe";
int text1_cursor_x = 383;
int text1_cursor_y = 133;
const GFXfont *text1_font = &FreeSerifBold24pt7b;

String text2_content = "Human first";
int text2_cursor_x = 600;
int text2_cursor_y = 259;
const GFXfont *text2_font = &FreeSerifBold24pt7b;

String text3_content = "Computer first";
int text3_cursor_x = 595;
int text3_cursor_y = 347;
const GFXfont *text3_font = &FreeSerifBold24pt7b;

int circle2_center_x = 59;
int circle2_center_y = 243;
int circle2_fill = -1;
int circle2_radius = 10;
int circle2_color = BLACK;

String text6_content = "Easy";
int text6_cursor_x = 104;
int text6_cursor_y = 257;
const GFXfont *text6_font = &FreeSerifBold24pt7b;

int circle3_center_x = 59;
int circle3_center_y = 340;
int circle3_fill = -1;
int circle3_radius = 10;
int circle3_color = BLACK;

String text7_content = "Medium";
int text7_cursor_x = 107;
int text7_cursor_y = 351;
const GFXfont *text7_font = &FreeSerifBold24pt7b;

int circle4_center_x = 61;
int circle4_center_y = 443;
int circle4_fill = -1;
int circle4_radius = 10;
int circle4_color = BLACK;

String text8_content = "Hard";
int text8_cursor_x = 108;
int text8_cursor_y = 453;
const GFXfont *text8_font = &FreeSerifBold24pt7b;

int rect1_a_x = 25;
int rect1_a_y = 192;
int rect1_b_x = 355;
int rect1_b_y = 640;
int rect1_fill = -1;
int rect1_radius = 10;
int rect1_color = BLACK;

int circle5_center_x = 62;
int circle5_center_y = 533;
int circle5_fill = -1;
int circle5_radius = 10;
int circle5_color = BLACK;

int circle6_center_x = 562;
int circle6_center_y = 334;
int circle6_fill = -1;
int circle6_radius = 10;
int circle6_color = BLACK;

int line4_start_x = 89;
int line4_start_y = 275;
int line4_end_x = 309;
int line4_end_y = 275;
int line4_color = BLACK;
int line4_thickness = 1;
int line4_gradient = 0;

int line5_start_x = 89;
int line5_start_y = 371;
int line5_end_x = 309;
int line5_end_y = 371;
int line5_color = BLACK;
int line5_thickness = 1;
int line5_gradient = 0;

int line6_start_x = 89;
int line6_start_y = 470;
int line6_end_x = 309;
int line6_end_y = 470;
int line6_color = BLACK;
int line6_thickness = 1;
int line6_gradient = 0;

int line7_start_x = 603;
int line7_start_y = 276;
int line7_end_x = 885;
int line7_end_y = 276;
int line7_color = BLACK;
int line7_thickness = 1;
int line7_gradient = 0;

int line8_start_x = 603;
int line8_start_y = 366;
int line8_end_x = 885;
int line8_end_y = 366;
int line8_color = BLACK;
int line8_thickness = 1;
int line8_gradient = 0;

int line9_start_x = 344;
int line9_start_y = 148;
int line9_end_x = 624;
int line9_end_y = 148;
int line9_color = BLACK;
int line9_thickness = 1;
int line9_gradient = 0;

int rect5_a_x = 585;
int rect5_a_y = 627;
int rect5_b_x = 886;
int rect5_b_y = 706;
int rect5_fill = -1;
int rect5_radius = 15;
int rect5_color = BLACK;

String text12_content = "Start";
int text12_cursor_x = 689;
int text12_cursor_y = 682;
const GFXfont *text12_font = &FreeSerifBold24pt7b;

String text13_content = "";
int text13_cursor_x = 264;
int text13_cursor_y = 47;
const GFXfont *text13_font = &FreeSerifBold24pt7b;

String text14_content = "2 player";
int text14_cursor_x = 111;
int text14_cursor_y = 545;
const GFXfont *text14_font = &FreeSerifBold24pt7b;

int line10_start_x = 89;
int line10_start_y = 566;
int line10_end_x = 309;
int line10_end_y = 566;
int line10_color = BLACK;
int line10_thickness = 1;
int line10_gradient = 0;

int circle7_center_x = 565;
int circle7_center_y = 251;
int circle7_fill = -1;
int circle7_radius = 10;
int circle7_color = BLACK;

int rect6_a_x = 526;
int rect6_a_y = 196;
int rect6_b_x = 957;
int rect6_b_y = 396;
int rect6_fill = -1;
int rect6_radius = 10;
int rect6_color = BLACK;

int circle8_center_x = 562;
int circle8_center_y = 450;
int circle8_fill = -1;
int circle8_radius = 10;
int circle8_color = BLACK;

int circle9_center_x = 563;
int circle9_center_y = 521;
int circle9_fill = -1;
int circle9_radius = 10;
int circle9_color = BLACK;

String text15_content = "X first";
int text15_cursor_x = 610;
int text15_cursor_y = 466;
const GFXfont *text15_font = &FreeSerifBold24pt7b;

String text16_content = "O first";
int text16_cursor_x = 607;
int text16_cursor_y = 536;
const GFXfont *text16_font = &FreeSerifBold24pt7b;

int rect7_a_x = 527;
int rect7_a_y = 415;
int rect7_b_x = 962;
int rect7_b_y = 574;
int rect7_fill = -1;
int rect7_radius = 10;
int rect7_color = BLACK;

int line11_start_x = 603;
int line11_start_y = 487;
int line11_end_x = 885;
int line11_end_y = 487;
int line11_color = BLACK;
int line11_thickness = 1;
int line11_gradient = 0;

int line12_start_x = 603;
int line12_start_y = 552;
int line12_end_x = 885;
int line12_end_y = 552;
int line12_color = BLACK;
int line12_thickness = 1;
int line12_gradient = 0;

void mainDrawMenu()
{
    display.setFont(text1_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text1_cursor_x, text1_cursor_y);
    display.print(text1_content);

    display.setFont(text2_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text2_cursor_x, text2_cursor_y);
    display.print(text2_content);

    display.setFont(text3_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text3_cursor_x, text3_cursor_y);
    display.print(text3_content);

    if (circle2_fill != -1)
        display.fillCircle(circle2_center_x, circle2_center_y, circle2_radius, circle2_color);
    else
        display.drawCircle(circle2_center_x, circle2_center_y, circle2_radius, circle2_color);

    display.setFont(text6_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text6_cursor_x, text6_cursor_y);
    display.print(text6_content);

    if (circle3_fill != -1)
        display.fillCircle(circle3_center_x, circle3_center_y, circle3_radius, circle3_color);
    else
        display.drawCircle(circle3_center_x, circle3_center_y, circle3_radius, circle3_color);

    display.setFont(text7_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text7_cursor_x, text7_cursor_y);
    display.print(text7_content);

    if (circle4_fill != -1)
        display.fillCircle(circle4_center_x, circle4_center_y, circle4_radius, circle4_color);
    else
        display.drawCircle(circle4_center_x, circle4_center_y, circle4_radius, circle4_color);

    display.setFont(text8_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text8_cursor_x, text8_cursor_y);
    display.print(text8_content);

    if (rect1_radius != -1 && rect1_fill != -1)
        display.fillRoundRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_radius,
                              rect1_color);
    else if (rect1_radius != -1 && rect1_fill == -1)
        display.drawRoundRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_radius,
                              rect1_color);
    else if (rect1_radius == -1 && rect1_fill != -1)
        display.fillRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_color);
    else if (rect1_radius == -1 && rect1_fill == -1)
        display.drawRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_color);

    if (circle5_fill != -1)
        display.fillCircle(circle5_center_x, circle5_center_y, circle5_radius, circle5_color);
    else
        display.drawCircle(circle5_center_x, circle5_center_y, circle5_radius, circle5_color);

    if (circle6_fill != -1)
        display.fillCircle(circle6_center_x, circle6_center_y, circle6_radius, circle6_color);
    else
        display.drawCircle(circle6_center_x, circle6_center_y, circle6_radius, circle6_color);

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

    if (line5_gradient <= line5_color && line5_thickness == 1)
        display.drawLine(line5_start_x, line5_start_y, line5_end_x, line5_end_y, line5_color);
    else if (line5_gradient <= line5_color && line5_thickness != 1)
        display.drawThickLine(line5_start_x, line5_start_y, line5_end_x, line5_end_y, line5_color, line5_thickness);
    else if (line5_gradient > line5_color && line5_thickness == 1)
        display.drawGradientLine(line5_start_x, line5_start_y, line5_end_x, line5_end_y, line5_color, line5_gradient,
                                 1);
    else if (line5_gradient > line5_color && line5_thickness != 1)
        display.drawGradientLine(line5_start_x, line5_start_y, line5_end_x, line5_end_y, line5_color, line5_gradient,
                                 line5_thickness);

    if (line6_gradient <= line6_color && line6_thickness == 1)
        display.drawLine(line6_start_x, line6_start_y, line6_end_x, line6_end_y, line6_color);
    else if (line6_gradient <= line6_color && line6_thickness != 1)
        display.drawThickLine(line6_start_x, line6_start_y, line6_end_x, line6_end_y, line6_color, line6_thickness);
    else if (line6_gradient > line6_color && line6_thickness == 1)
        display.drawGradientLine(line6_start_x, line6_start_y, line6_end_x, line6_end_y, line6_color, line6_gradient,
                                 1);
    else if (line6_gradient > line6_color && line6_thickness != 1)
        display.drawGradientLine(line6_start_x, line6_start_y, line6_end_x, line6_end_y, line6_color, line6_gradient,
                                 line6_thickness);

    if (line7_gradient <= line7_color && line7_thickness == 1)
        display.drawLine(line7_start_x, line7_start_y, line7_end_x, line7_end_y, line7_color);
    else if (line7_gradient <= line7_color && line7_thickness != 1)
        display.drawThickLine(line7_start_x, line7_start_y, line7_end_x, line7_end_y, line7_color, line7_thickness);
    else if (line7_gradient > line7_color && line7_thickness == 1)
        display.drawGradientLine(line7_start_x, line7_start_y, line7_end_x, line7_end_y, line7_color, line7_gradient,
                                 1);
    else if (line7_gradient > line7_color && line7_thickness != 1)
        display.drawGradientLine(line7_start_x, line7_start_y, line7_end_x, line7_end_y, line7_color, line7_gradient,
                                 line7_thickness);

    if (line8_gradient <= line8_color && line8_thickness == 1)
        display.drawLine(line8_start_x, line8_start_y, line8_end_x, line8_end_y, line8_color);
    else if (line8_gradient <= line8_color && line8_thickness != 1)
        display.drawThickLine(line8_start_x, line8_start_y, line8_end_x, line8_end_y, line8_color, line8_thickness);
    else if (line8_gradient > line8_color && line8_thickness == 1)
        display.drawGradientLine(line8_start_x, line8_start_y, line8_end_x, line8_end_y, line8_color, line8_gradient,
                                 1);
    else if (line8_gradient > line8_color && line8_thickness != 1)
        display.drawGradientLine(line8_start_x, line8_start_y, line8_end_x, line8_end_y, line8_color, line8_gradient,
                                 line8_thickness);

    if (line9_gradient <= line9_color && line9_thickness == 1)
        display.drawLine(line9_start_x, line9_start_y, line9_end_x, line9_end_y, line9_color);
    else if (line9_gradient <= line9_color && line9_thickness != 1)
        display.drawThickLine(line9_start_x, line9_start_y, line9_end_x, line9_end_y, line9_color, line9_thickness);
    else if (line9_gradient > line9_color && line9_thickness == 1)
        display.drawGradientLine(line9_start_x, line9_start_y, line9_end_x, line9_end_y, line9_color, line9_gradient,
                                 1);
    else if (line9_gradient > line9_color && line9_thickness != 1)
        display.drawGradientLine(line9_start_x, line9_start_y, line9_end_x, line9_end_y, line9_color, line9_gradient,
                                 line9_thickness);

    if (rect5_radius != -1 && rect5_fill != -1)
        display.fillRoundRect(rect5_a_x, rect5_a_y, rect5_b_x - rect5_a_x, rect5_b_y - rect5_a_y, rect5_radius,
                              rect5_color);
    else if (rect5_radius != -1 && rect5_fill == -1)
        display.drawRoundRect(rect5_a_x, rect5_a_y, rect5_b_x - rect5_a_x, rect5_b_y - rect5_a_y, rect5_radius,
                              rect5_color);
    else if (rect5_radius == -1 && rect5_fill != -1)
        display.fillRect(rect5_a_x, rect5_a_y, rect5_b_x - rect5_a_x, rect5_b_y - rect5_a_y, rect5_color);
    else if (rect5_radius == -1 && rect5_fill == -1)
        display.drawRect(rect5_a_x, rect5_a_y, rect5_b_x - rect5_a_x, rect5_b_y - rect5_a_y, rect5_color);

    display.setFont(text12_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text12_cursor_x, text12_cursor_y);
    display.print(text12_content);

    display.setFont(text13_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text13_cursor_x, text13_cursor_y);
    display.print(text13_content);

    display.setFont(text14_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text14_cursor_x, text14_cursor_y);
    display.print(text14_content);

    if (line10_gradient <= line10_color && line10_thickness == 1)
        display.drawLine(line10_start_x, line10_start_y, line10_end_x, line10_end_y, line10_color);
    else if (line10_gradient <= line10_color && line10_thickness != 1)
        display.drawThickLine(line10_start_x, line10_start_y, line10_end_x, line10_end_y, line10_color,
                              line10_thickness);
    else if (line10_gradient > line10_color && line10_thickness == 1)
        display.drawGradientLine(line10_start_x, line10_start_y, line10_end_x, line10_end_y, line10_color,
                                 line10_gradient, 1);
    else if (line10_gradient > line10_color && line10_thickness != 1)
        display.drawGradientLine(line10_start_x, line10_start_y, line10_end_x, line10_end_y, line10_color,
                                 line10_gradient, line10_thickness);

    if (circle7_fill != -1)
        display.fillCircle(circle7_center_x, circle7_center_y, circle7_radius, circle7_color);
    else
        display.drawCircle(circle7_center_x, circle7_center_y, circle7_radius, circle7_color);

    if (rect6_radius != -1 && rect6_fill != -1)
        display.fillRoundRect(rect6_a_x, rect6_a_y, rect6_b_x - rect6_a_x, rect6_b_y - rect6_a_y, rect6_radius,
                              rect6_color);
    else if (rect6_radius != -1 && rect6_fill == -1)
        display.drawRoundRect(rect6_a_x, rect6_a_y, rect6_b_x - rect6_a_x, rect6_b_y - rect6_a_y, rect6_radius,
                              rect6_color);
    else if (rect6_radius == -1 && rect6_fill != -1)
        display.fillRect(rect6_a_x, rect6_a_y, rect6_b_x - rect6_a_x, rect6_b_y - rect6_a_y, rect6_color);
    else if (rect6_radius == -1 && rect6_fill == -1)
        display.drawRect(rect6_a_x, rect6_a_y, rect6_b_x - rect6_a_x, rect6_b_y - rect6_a_y, rect6_color);

    if (circle8_fill != -1)
        display.fillCircle(circle8_center_x, circle8_center_y, circle8_radius, circle8_color);
    else
        display.drawCircle(circle8_center_x, circle8_center_y, circle8_radius, circle8_color);

    if (circle9_fill != -1)
        display.fillCircle(circle9_center_x, circle9_center_y, circle9_radius, circle9_color);
    else
        display.drawCircle(circle9_center_x, circle9_center_y, circle9_radius, circle9_color);

    display.setFont(text15_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text15_cursor_x, text15_cursor_y);
    display.print(text15_content);

    display.setFont(text16_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text16_cursor_x, text16_cursor_y);
    display.print(text16_content);

    if (rect7_radius != -1 && rect7_fill != -1)
        display.fillRoundRect(rect7_a_x, rect7_a_y, rect7_b_x - rect7_a_x, rect7_b_y - rect7_a_y, rect7_radius,
                              rect7_color);
    else if (rect7_radius != -1 && rect7_fill == -1)
        display.drawRoundRect(rect7_a_x, rect7_a_y, rect7_b_x - rect7_a_x, rect7_b_y - rect7_a_y, rect7_radius,
                              rect7_color);
    else if (rect7_radius == -1 && rect7_fill != -1)
        display.fillRect(rect7_a_x, rect7_a_y, rect7_b_x - rect7_a_x, rect7_b_y - rect7_a_y, rect7_color);
    else if (rect7_radius == -1 && rect7_fill == -1)
        display.drawRect(rect7_a_x, rect7_a_y, rect7_b_x - rect7_a_x, rect7_b_y - rect7_a_y, rect7_color);

    if (line11_gradient <= line11_color && line11_thickness == 1)
        display.drawLine(line11_start_x, line11_start_y, line11_end_x, line11_end_y, line11_color);
    else if (line11_gradient <= line11_color && line11_thickness != 1)
        display.drawThickLine(line11_start_x, line11_start_y, line11_end_x, line11_end_y, line11_color,
                              line11_thickness);
    else if (line11_gradient > line11_color && line11_thickness == 1)
        display.drawGradientLine(line11_start_x, line11_start_y, line11_end_x, line11_end_y, line11_color,
                                 line11_gradient, 1);
    else if (line11_gradient > line11_color && line11_thickness != 1)
        display.drawGradientLine(line11_start_x, line11_start_y, line11_end_x, line11_end_y, line11_color,
                                 line11_gradient, line11_thickness);

    if (line12_gradient <= line12_color && line12_thickness == 1)
        display.drawLine(line12_start_x, line12_start_y, line12_end_x, line12_end_y, line12_color);
    else if (line12_gradient <= line12_color && line12_thickness != 1)
        display.drawThickLine(line12_start_x, line12_start_y, line12_end_x, line12_end_y, line12_color,
                              line12_thickness);
    else if (line12_gradient > line12_color && line12_thickness == 1)
        display.drawGradientLine(line12_start_x, line12_start_y, line12_end_x, line12_end_y, line12_color,
                                 line12_gradient, 1);
    else if (line12_gradient > line12_color && line12_thickness != 1)
        display.drawGradientLine(line12_start_x, line12_start_y, line12_end_x, line12_end_y, line12_color,
                                 line12_gradient, line12_thickness);
}
