#include "Arduino.h"
#include "Fonts/Open_Sans_SemiBold_16.h"
#include "Fonts/Open_Sans_SemiBold_24.h"
#include "Fonts/Open_Sans_SemiBold_32.h"
#include "Fonts/Open_Sans_SemiBold_56.h"
#include "Inkplate.h"

extern Inkplate display;

int rect0_a_x = 12;
int rect0_a_y = 9;
int rect0_b_x = 948;
int rect0_b_y = 532;
int rect0_fill = -1;
int rect0_radius = -1;
int rect0_color = BLACK;

String text1_content;
int text1_cursor_x = 28;
int text1_cursor_y = 50;
const GFXfont *text1_font = &Open_Sans_SemiBold_56;

String text2_content = "Soldered";
int text2_cursor_x = 790;
int text2_cursor_y = 38;
const GFXfont *text2_font = &Open_Sans_SemiBold_32;

String text3_content;
int text3_cursor_x = 120;
int text3_cursor_y = 90;
const GFXfont *text3_font = &Open_Sans_SemiBold_24;

String text4_content;
int text4_cursor_x = 84;
int text4_cursor_y = 274;
const GFXfont *text4_font = &Open_Sans_SemiBold_56;

String text5_content = "$";
int text5_cursor_x = 60;
int text5_cursor_y = 270;
const GFXfont *text5_font = &Open_Sans_SemiBold_32;

String text6_content = "raised";
int text6_cursor_x = 305;
int text6_cursor_y = 265;
const GFXfont *text6_font = &Open_Sans_SemiBold_32;

String text7_content;
int text7_cursor_x = 84;
int text7_cursor_y = 345;
const GFXfont *text7_font = &Open_Sans_SemiBold_56;

String text8_content = "$";
int text8_cursor_x = 60;
int text8_cursor_y = 345;
const GFXfont *text8_font = &Open_Sans_SemiBold_32;

String text9_content = "goal";
int text9_cursor_x = 305;
int text9_cursor_y = 336;
const GFXfont *text9_font = &Open_Sans_SemiBold_32;

int rect1_a_x = 48;
int rect1_a_y = 225;
int rect1_b_x = 420;
int rect1_b_y = 472;
int rect1_fill = -1;
int rect1_radius = -1;
int rect1_color = BLACK;

int rect2_a_x = 540;
int rect2_a_y = 225;
int rect2_b_x = 912;
int rect2_b_y = 472;
int rect2_fill = -1;
int rect2_radius = -1;
int rect2_color = BLACK;

int rect3_a_x = 60;
int rect3_a_y = 405;
int rect3_b_x = 408;
int rect3_b_y = 429;
int rect3_fill = -1;
int rect3_radius = -1;
int rect3_color = BLACK;

int line0_start_x = 60;
int line0_start_y = 416;
int line0_end_x = 408;
int line0_end_y = 416;
int line0_color = BLACK;
int line0_thickness = 26;
int line0_gradient = 0;

String text11_content = "100%";
int text11_cursor_x = 210;
int text11_cursor_y = 395;
const GFXfont *text11_font = &Open_Sans_SemiBold_24;

String text12_content = "Funded!";
int text12_cursor_x = 180;
int text12_cursor_y = 455;
const GFXfont *text12_font = &Open_Sans_SemiBold_24;

String text13_content;
int text13_cursor_x = 552;
int text13_cursor_y = 274;
const GFXfont *text13_font = &Open_Sans_SemiBold_56;

String text14_content;
int text14_cursor_x = 552;
int text14_cursor_y = 351;
const GFXfont *text14_font = &Open_Sans_SemiBold_32;

String text15_content;
int text15_cursor_x = 720;
int text15_cursor_y = 365;
const GFXfont *text15_font = &Open_Sans_SemiBold_56;

String text16_content = "updates";
int text16_cursor_x = 720;
int text16_cursor_y = 265;
const GFXfont *text16_font = &Open_Sans_SemiBold_32;

String text17_content;
int text17_cursor_x = 552;
int text17_cursor_y = 453;
const GFXfont *text17_font = &Open_Sans_SemiBold_56;

String text18_content = "backers";
int text18_cursor_x = 720;
int text18_cursor_y = 450;
const GFXfont *text18_font = &Open_Sans_SemiBold_32;

String text19_content = "Last update posted ";
int text19_cursor_x = 290;
int text19_cursor_y = 513;
const GFXfont *text19_font = &Open_Sans_SemiBold_24;

void mainDraw()
{
    if (rect0_radius != -1 && rect0_fill != -1)
        display.fillRoundRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_radius,
                              rect0_color);
    else if (rect0_radius != -1 && rect0_fill == -1)
        display.drawRoundRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_radius,
                              rect0_color);
    else if (rect0_radius == -1 && rect0_fill != -1)
        display.fillRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_color);
    else if (rect0_radius == -1 && rect0_fill == -1)
        display.drawRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_color);

    display.setFont(text1_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text1_cursor_x, text1_cursor_y);
    display.print(text1_content);
    //Serial.print("Text 1: ");
    //Serial.println(text1_content);

    display.setFont(text2_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text2_cursor_x, text2_cursor_y);
    display.print(text2_content);
    //Serial.print("Text 2: ");
    //Serial.println(text2_content);

    // Manual edit:
    GFXcanvas1 canvas(750, 150);
    canvas.setFont(text3_font);
    canvas.setTextColor(BLACK, WHITE);
    canvas.setTextSize(1);
    canvas.setCursor(0, 20);
    canvas.print(text3_content);
    // canvas.drawRect(0, 0, 100, 100, BLACK);
    //Serial.print("Text 3: ");
    //Serial.println(text3_content);

    display.drawBitmap(text3_cursor_x, text3_cursor_y, canvas.getBuffer(), 750, 150, BLACK, WHITE);

    display.setFont(text4_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text4_cursor_x, text4_cursor_y);
    display.print(text4_content);
    //Serial.print("Text 4: ");//
    //Serial.println(text4_content)//;

    display.setFont(text5_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text5_cursor_x, text5_cursor_y);
    display.print(text5_content);
    //Serial.print("Text 5: ");
    //Serial.println(text5_content);

    display.setFont(text6_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text6_cursor_x, text6_cursor_y);
    display.print(text6_content);
    //Serial.print("Text 6: ");
    //Serial.println(text6_content);

    display.setFont(text7_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text7_cursor_x, text7_cursor_y);
    display.print(text7_content);
    //Serial.print("Text 7: ");
    //Serial.println(text7_content);

    display.setFont(text8_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text8_cursor_x, text8_cursor_y);
    display.print(text8_content);
    //Serial.print("Text 8: ");
    //Serial.println(text8_content);

    display.setFont(text9_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text9_cursor_x, text9_cursor_y);
    display.print(text9_content);
    //Serial.print("Text 9: ");
    //Serial.println(text9_content);

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

    if (rect2_radius != -1 && rect2_fill != -1)
        display.fillRoundRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_radius,
                              rect2_color);
    else if (rect2_radius != -1 && rect2_fill == -1)
        display.drawRoundRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_radius,
                              rect2_color);
    else if (rect2_radius == -1 && rect2_fill != -1)
        display.fillRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_color);
    else if (rect2_radius == -1 && rect2_fill == -1)
        display.drawRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_color);

    if (rect3_radius != -1 && rect3_fill != -1)
        display.fillRoundRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_radius,
                              rect3_color);
    else if (rect3_radius != -1 && rect3_fill == -1)
        display.drawRoundRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_radius,
                              rect3_color);
    else if (rect3_radius == -1 && rect3_fill != -1)
        display.fillRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_color);
    else if (rect3_radius == -1 && rect3_fill == -1)
        display.drawRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_color);

    if (line0_gradient <= line0_color && line0_thickness == 1)
    {
        display.drawLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color);
    }
    else if (line0_gradient <= line0_color && line0_thickness != 1)
    {
        display.drawThickLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color, line0_thickness);
    }
    else if (line0_gradient > line0_color && line0_thickness == 1)
    {
        display.drawGradientLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color, line0_gradient,
                                 1);
    }
    else if (line0_gradient > line0_color && line0_thickness != 1)
    {
        display.drawGradientLine(line0_start_x, line0_start_y, line0_end_x, line0_end_y, line0_color, line0_gradient,
                                 line0_thickness);
    }
    display.setFont(text11_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text11_cursor_x, text11_cursor_y);
    display.print(text11_content);
    //Serial.print("Text 11: ");
    //Serial.println(text11_content);

    display.setFont(text12_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text12_cursor_x, text12_cursor_y);
    display.print(text12_content);
    //Serial.print("Text 12: ");
    //Serial.println(text12_content);

    display.setFont(text13_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text13_cursor_x, text13_cursor_y);
    display.print(text13_content);
    //Serial.print("Text 13: ");
    //Serial.println(text13_content);

    display.setFont(text14_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text14_cursor_x, text14_cursor_y);
    display.print(text14_content);
    //Serial.print("Text 14: ");
    //Serial.println(text14_content);

    display.setFont(text15_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text15_cursor_x, text15_cursor_y);
    display.print(text15_content);
    //Serial.print("Text 15: ");
    //Serial.println(text15_content);

    display.setFont(text16_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text16_cursor_x, text16_cursor_y);
    display.print(text16_content);
    //Serial.print("Text 16: ");
    //Serial.println(text16_content);

    display.setFont(text17_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text17_cursor_x, text17_cursor_y);
    display.print(text17_content);
    //Serial.print("Text 17: ");
    //Serial.println(text17_content);

    display.setFont(text18_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text18_cursor_x, text18_cursor_y);
    display.print(text18_content);
    //Serial.print("Text 18: ");
    //Serial.println(text18_content);

    display.setFont(text19_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text19_cursor_x, text19_cursor_y);
    display.print(text19_content);
    //Serial.print("Text 19: ");
    //Serial.println(text19_content);
}
