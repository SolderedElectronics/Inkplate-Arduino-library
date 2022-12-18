#include "Arduino.h"
#include "Fonts/Inter8pt7b.h"
#include "Inkplate.h"


extern Inkplate display;

int rect0_a_x = 1;
int rect0_a_y = 1;
int rect0_b_x = 211;
int rect0_b_y = 103;
int rect0_fill = -1;
int rect0_radius = -1;
int rect0_color = INKPLATE2_BLACK;

String text1_content;
int text1_cursor_x = 5;
int text1_cursor_y = 20;
const GFXfont *text1_font = &Inter8pt7b;

String text2_content;
int text2_cursor_x = 5;
int text2_cursor_y = 40;
const GFXfont *text2_font = &Inter8pt7b;

String text3_content;
int text3_cursor_x = 5;
int text3_cursor_y = 60;
const GFXfont *text3_font = &Inter8pt7b;
//

String text4_content;
int text4_cursor_x = 15;
int text4_cursor_y = 60;
const GFXfont *text4_font = &Inter8pt7b;

String text5_content = "$";
int text5_cursor_x = 5;
int text5_cursor_y = 60;
const GFXfont *text5_font = &Inter8pt7b;

String text6_content = "raised";
int text6_cursor_x = 100;
int text6_cursor_y = 60;
const GFXfont *text6_font = &Inter8pt7b;

String text7_content;
int text7_cursor_x = 15;
int text7_cursor_y = 80;
const GFXfont *text7_font = &Inter8pt7b;

String text8_content = "$";
int text8_cursor_x = 5;
int text8_cursor_y = 80;
const GFXfont *text8_font = &Inter8pt7b;

String text9_content = "goal";
int text9_cursor_x = 100;
int text9_cursor_y = 80;
const GFXfont *text9_font = &Inter8pt7b;

String text17_content;
int text17_cursor_x = 5;
int text17_cursor_y = 100;
const GFXfont *text17_font = &Inter8pt7b;

String text18_content = "backers";
int text18_cursor_x = 65;
int text18_cursor_y = 100;
const GFXfont *text18_font = &Inter8pt7b;

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
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text1_cursor_x, text1_cursor_y);
    display.print(text1_content);

    display.setFont(text2_font);
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text2_cursor_x, text2_cursor_y);
    display.print(text2_content);

    // Manual edit:
    GFXcanvas1 canvas(750, 150);
    canvas.setFont(text3_font);
    canvas.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    canvas.setTextSize(1);
    canvas.setCursor(0, 20);
    canvas.print(text3_content);

    display.setFont(text4_font);
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text4_cursor_x, text4_cursor_y);
    display.print(text4_content);

    display.setFont(text5_font);
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text5_cursor_x, text5_cursor_y);
    display.print(text5_content);

    display.setFont(text6_font);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text6_cursor_x, text6_cursor_y);
    display.print(text6_content);

    display.setFont(text7_font);
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text7_cursor_x, text7_cursor_y);
    display.print(text7_content);

    display.setFont(text8_font);
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text8_cursor_x, text8_cursor_y);
    display.print(text8_content);

    display.setFont(text9_font);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text9_cursor_x, text9_cursor_y);
    display.print(text9_content);

    display.setFont(text17_font);
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text17_cursor_x, text17_cursor_y);
    display.print(text17_content);

    display.setFont(text18_font);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.setTextSize(1);
    display.setCursor(text18_cursor_x, text18_cursor_y);
    display.print(text18_content);

}
