#include "Arduino.h"
#include "Inkplate.h"
#include "Fonts/FreeMonoBold24pt7b.h"
#include "Fonts/FreeMonoBold18pt7b.h"
#include "Fonts/FreeSans24pt7b.h"
#include "Fonts/FreeSansBold12pt7b.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeMonoOblique18pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"

extern Inkplate display;

int rect0_a_x = 27;
int rect0_a_y = 383;
int rect0_b_x = 200;
int rect0_b_y = 476;
int rect0_fill = -1;
int rect0_radius = -1;
int rect0_color = BLACK;

int rect1_a_x = 213;
int rect1_a_y = 383;
int rect1_b_x = 386;
int rect1_b_y = 476;
int rect1_fill = -1;
int rect1_radius = -1;
int rect1_color = BLACK;

int rect2_a_x = 399;
int rect2_a_y = 383;
int rect2_b_x = 572;
int rect2_b_y = 476;
int rect2_fill = -1;
int rect2_radius = -1;
int rect2_color = BLACK;

int rect3_a_x = 27;
int rect3_a_y = 483;
int rect3_b_x = 200;
int rect3_b_y = 576;
int rect3_fill = -1;
int rect3_radius = -1;
int rect3_color = BLACK;

int rect4_a_x = 213;
int rect4_a_y = 483;
int rect4_b_x = 386;
int rect4_b_y = 576;
int rect4_fill = -1;
int rect4_radius = -1;
int rect4_color = BLACK;

int rect5_a_x = 399;
int rect5_a_y = 483;
int rect5_b_x = 572;
int rect5_b_y = 576;
int rect5_fill = -1;
int rect5_radius = -1;
int rect5_color = BLACK;

int rect6_a_x = 33;
int rect6_a_y = 309;
int rect6_b_x = 289;
int rect6_b_y = 377;
int rect6_fill = -1;
int rect6_radius = -1;
int rect6_color = BLACK;

int rect7_a_x = 313;
int rect7_a_y = 309;
int rect7_b_x = 569;
int rect7_b_y = 377;
int rect7_fill = -1;
int rect7_radius = -1;
int rect7_color = BLACK;

String text1_content = "Bitcoin";
int text1_cursor_x = 72;
int text1_cursor_y = 436;
int text1_color_text = BLACK;
int text1_color_bg = WHITE;
const GFXfont *text1_font = &FreeSansBold12pt7b;

String text2_content = "Ethereum";
int text2_cursor_x = 242;
int text2_cursor_y = 436;
int text2_color_text = BLACK;
int text2_color_bg = WHITE;
const GFXfont *text2_font = &FreeSansBold12pt7b;

String text3_content = "Binance";
int text3_cursor_x = 438;
int text3_cursor_y = 436;
int text3_color_text = BLACK;
int text3_color_bg = WHITE;
const GFXfont *text3_font = &FreeSansBold12pt7b;

String text4_content = "Ripple";
int text4_cursor_x = 76;
int text4_cursor_y = 536;
int text4_color_text = BLACK;
int text4_color_bg = WHITE;
const GFXfont *text4_font = &FreeSansBold12pt7b;

String text5_content = "Dogecoin";
int text5_cursor_x = 246;
int text5_cursor_y = 536;
int text5_color_text = BLACK;
int text5_color_bg = WHITE;
const GFXfont *text5_font = &FreeSansBold12pt7b;

String text6_content = "Tether";
int text6_cursor_x = 448;
int text6_cursor_y = 536;
int text6_color_text = BLACK;
int text6_color_bg = WHITE;
const GFXfont *text6_font = &FreeSansBold12pt7b;

String text7_content = "BTC";
int text7_cursor_x = 27;
int text7_cursor_y = 72;
const GFXfont *text7_font = &FreeMonoBold18pt7b;

String text8_content = "Prices";
int text8_cursor_x = 411;
int text8_cursor_y = 75;
const GFXfont *text8_font = &FreeSansBold9pt7b;

String text10_content = "Dollar:";
int text10_cursor_x = 224;
int text10_cursor_y = 133;
const GFXfont *text10_font = &FreeSansBold12pt7b;

String text9_content = "";
int text9_cursor_x = 354;
int text9_cursor_y = 137;
const GFXfont *text9_font = &FreeMonoBold24pt7b;

String text12_content = "Euro:";
int text12_cursor_x = 224;
int text12_cursor_y = 197;
const GFXfont *text12_font = &FreeSansBold12pt7b;

String text11_content = "";
int text11_cursor_x = 354;
int text11_cursor_y = 201;
const GFXfont *text11_font = &FreeMonoBold24pt7b;

String text15_content = "GBP:";
int text15_cursor_x = 224;
int text15_cursor_y = 261;
const GFXfont *text15_font = &FreeSansBold12pt7b;

String text13_content = "";
int text13_cursor_x = 354;
int text13_cursor_y = 265;
const GFXfont *text13_font = &FreeMonoBold24pt7b;

String text14_content = "Refresh Screen";
int text14_cursor_x = 40;
int text14_cursor_y = 355;
const GFXfont *text14_font = &FreeSans18pt7b;

String text16_content = "Refresh Price";
int text16_cursor_x = 338;
int text16_cursor_y = 355;
const GFXfont *text16_font = &FreeSans18pt7b;

String text17_content = "";
int text17_cursor_x = 240;
int text17_cursor_y = 45;
const GFXfont *text17_font = &FreeSansBold9pt7b;

String text18_content = "";
int text18_cursor_x = 446;
int text18_cursor_y = 45;
const GFXfont *text18_font = &FreeSansBold9pt7b;

void mainDraw()
{
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

    if (rect4_radius != -1 && rect4_fill != -1)
        display.fillRoundRect(rect4_a_x, rect4_a_y, rect4_b_x - rect4_a_x, rect4_b_y - rect4_a_y, rect4_radius, rect4_color);
    else if (rect4_radius != -1 && rect4_fill == -1)
        display.drawRoundRect(rect4_a_x, rect4_a_y, rect4_b_x - rect4_a_x, rect4_b_y - rect4_a_y, rect4_radius, rect4_color);
    else if (rect4_radius == -1 && rect4_fill != -1)
        display.fillRect(rect4_a_x, rect4_a_y, rect4_b_x - rect4_a_x, rect4_b_y - rect4_a_y, rect4_color);
    else if (rect4_radius == -1 && rect4_fill == -1)
        display.drawRect(rect4_a_x, rect4_a_y, rect4_b_x - rect4_a_x, rect4_b_y - rect4_a_y, rect4_color);

    if (rect5_radius != -1 && rect5_fill != -1)
        display.fillRoundRect(rect5_a_x, rect5_a_y, rect5_b_x - rect5_a_x, rect5_b_y - rect5_a_y, rect5_radius, rect5_color);
    else if (rect5_radius != -1 && rect5_fill == -1)
        display.drawRoundRect(rect5_a_x, rect5_a_y, rect5_b_x - rect5_a_x, rect5_b_y - rect5_a_y, rect5_radius, rect5_color);
    else if (rect5_radius == -1 && rect5_fill != -1)
        display.fillRect(rect5_a_x, rect5_a_y, rect5_b_x - rect5_a_x, rect5_b_y - rect5_a_y, rect5_color);
    else if (rect5_radius == -1 && rect5_fill == -1)
        display.drawRect(rect5_a_x, rect5_a_y, rect5_b_x - rect5_a_x, rect5_b_y - rect5_a_y, rect5_color);

    if (rect6_radius != -1 && rect6_fill != -1)
        display.fillRoundRect(rect6_a_x, rect6_a_y, rect6_b_x - rect6_a_x, rect6_b_y - rect6_a_y, rect6_radius, rect6_color);
    else if (rect6_radius != -1 && rect6_fill == -1)
        display.drawRoundRect(rect6_a_x, rect6_a_y, rect6_b_x - rect6_a_x, rect6_b_y - rect6_a_y, rect6_radius, rect6_color);
    else if (rect6_radius == -1 && rect6_fill != -1)
        display.fillRect(rect6_a_x, rect6_a_y, rect6_b_x - rect6_a_x, rect6_b_y - rect6_a_y, rect6_color);
    else if (rect6_radius == -1 && rect6_fill == -1)
        display.drawRect(rect6_a_x, rect6_a_y, rect6_b_x - rect6_a_x, rect6_b_y - rect6_a_y, rect6_color);

        if (rect7_radius != -1 && rect7_fill != -1)
        display.fillRoundRect(rect7_a_x, rect7_a_y, rect7_b_x - rect7_a_x, rect7_b_y - rect7_a_y, rect7_radius, rect7_color);
    else if (rect7_radius != -1 && rect7_fill == -1)
        display.drawRoundRect(rect7_a_x, rect7_a_y, rect7_b_x - rect7_a_x, rect7_b_y - rect7_a_y, rect7_radius, rect7_color);
    else if (rect7_radius == -1 && rect7_fill != -1)
        display.fillRect(rect7_a_x, rect7_a_y, rect7_b_x - rect7_a_x, rect7_b_y - rect7_a_y, rect7_color);
    else if (rect7_radius == -1 && rect7_fill == -1)
        display.drawRect(rect7_a_x, rect7_a_y, rect7_b_x - rect7_a_x, rect7_b_y - rect7_a_y, rect7_color);

    display.setFont(text1_font);
    display.setTextColor(text1_color_text, text1_color_bg);
    display.setTextSize(1);
    display.setCursor(text1_cursor_x, text1_cursor_y);
    display.print(text1_content);

    display.setFont(text2_font);
    display.setTextColor(text2_color_text, text2_color_bg);
    display.setTextSize(1);
    display.setCursor(text2_cursor_x, text2_cursor_y);
    display.print(text2_content);

    display.setFont(text3_font);
    display.setTextColor(text3_color_text, text3_color_bg);
    display.setTextSize(1);
    display.setCursor(text3_cursor_x, text3_cursor_y);
    display.print(text3_content);

    display.setFont(text4_font);
    display.setTextColor(text4_color_text, text4_color_bg);
    display.setTextSize(1);
    display.setCursor(text4_cursor_x, text4_cursor_y);
    display.print(text4_content);

    display.setFont(text5_font);
    display.setTextColor(text5_color_text, text5_color_bg);
    display.setTextSize(1);
    display.setCursor(text5_cursor_x, text5_cursor_y);
    display.print(text5_content);

    display.setFont(text6_font);
    display.setTextColor(text6_color_text, text6_color_bg);
    display.setTextSize(1);
    display.setCursor(text6_cursor_x, text6_cursor_y);
    display.print(text6_content);

    display.setFont(text7_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(2);
    display.setCursor(text7_cursor_x, text7_cursor_y);
    display.print(text7_content);

    display.setFont(text8_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text8_cursor_x, text8_cursor_y);
    display.print(text8_content);

    display.setFont(text9_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text9_cursor_x, text9_cursor_y);
    display.print(text9_content);

    display.setFont(text10_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text10_cursor_x, text10_cursor_y);
    display.print(text10_content);

    display.setFont(text11_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text11_cursor_x, text11_cursor_y);
    display.print(text11_content);

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

    display.setFont(text17_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text17_cursor_x, text17_cursor_y);
    display.print(text17_content);

    display.setFont(text18_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setCursor(text18_cursor_x, text18_cursor_y);
    display.print(text18_content);
}
