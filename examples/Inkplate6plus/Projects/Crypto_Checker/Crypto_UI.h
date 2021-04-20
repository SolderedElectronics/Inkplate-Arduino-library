#include "Arduino.h"
#include "Inkplate.h"
#include "Fonts/FreeSansBold24pt7b.h"

extern Inkplate display;

int rect0_a_x = 600;
int rect0_a_y = 10;
int rect0_b_x = 1000;
int rect0_b_y = 90;
int rect0_fill = -1;
int rect0_radius = -1;
int rect0_color = BLACK;

int rect1_a_x = 600;
int rect1_a_y = 110;
int rect1_b_x = 1000;
int rect1_b_y = 190;
int rect1_fill = -1;
int rect1_radius = -1;
int rect1_color = BLACK;

int rect2_a_x = 600;
int rect2_a_y = 210;
int rect2_b_x = 1000;
int rect2_b_y = 290;
int rect2_fill = -1;
int rect2_radius = -1;
int rect2_color = BLACK;

int rect3_a_x = 600;
int rect3_a_y = 310;
int rect3_b_x = 1000;
int rect3_b_y = 390;
int rect3_fill = -1;
int rect3_radius = -1;
int rect3_color = BLACK;

int rect4_a_x = 600;
int rect4_a_y = 410;
int rect4_b_x = 1000;
int rect4_b_y = 490;
int rect4_fill = -1;
int rect4_radius = -1;
int rect4_color = BLACK;

int rect5_a_x = 600;
int rect5_a_y = 510;
int rect5_b_x = 1000;
int rect5_b_y = 590;
int rect5_fill = -1;
int rect5_radius = -1;
int rect5_color = BLACK;

int rect6_a_x = 600;
int rect6_a_y = 610;
int rect6_b_x = 1000;
int rect6_b_y = 690;
int rect6_fill = -1;
int rect6_radius = -1;
int rect6_color = BLACK;

String text1_content = "Bitcoin";
int text1_cursor_x = 741;
int text1_cursor_y = 61;
const GFXfont *text1_font = &FreeSansBold24pt7b;

String text2_content = "Ethereum";
int text2_cursor_x = 720;
int text2_cursor_y = 170;
const GFXfont *text2_font = &FreeSansBold24pt7b;

String text3_content = "Binance";
int text3_cursor_x = 741;
int text3_cursor_y = 265;
const GFXfont *text3_font = &FreeSansBold24pt7b;

String text4_content = "XRP";
int text4_cursor_x = 763;
int text4_cursor_y = 373;
const GFXfont *text4_font = &FreeSansBold24pt7b;

String text5_content = "Dogecoin";
int text5_cursor_x = 731;
int text5_cursor_y = 462;
const GFXfont *text5_font = &FreeSansBold24pt7b;

String text6_content = "Tether";
int text6_cursor_x = 744;
int text6_cursor_y = 562;
const GFXfont *text6_font = &FreeSansBold24pt7b;

String text7_content = "BTC";
int text7_cursor_x = 32;
int text7_cursor_y = 100;
const GFXfont *text7_font = &FreeSansBold24pt7b;

String text8_content = "Current:";
int text8_cursor_x = 36;
int text8_cursor_y = 174;
const GFXfont *text8_font = &FreeSansBold24pt7b;

String text9_content = "56288.94$";
int text9_cursor_x = 316;
int text9_cursor_y = 176;
const GFXfont *text9_font = &FreeSansBold24pt7b;

String text10_content = "Minimum:";
int text10_cursor_x = 42;
int text10_cursor_y = 269;
const GFXfont *text10_font = &FreeSansBold24pt7b;

String text11_content = "50000.00$";
int text11_cursor_x = 320;
int text11_cursor_y = 275;
const GFXfont *text11_font = &FreeSansBold24pt7b;

String text12_content = "Maximum:";
int text12_cursor_x = 36;
int text12_cursor_y = 371;
const GFXfont *text12_font = &FreeSansBold24pt7b;

String text13_content = "60000.00$";
int text13_cursor_x = 314;
int text13_cursor_y = 372;
const GFXfont *text13_font = &FreeSansBold24pt7b;

String text14_content = "Refresh";
int text14_cursor_x = 731;
int text14_cursor_y = 665;
const GFXfont *text14_font = &FreeSansBold24pt7b;

int widget1_n = 32;

int widget1_corner1_x = 47;
int widget1_corner2_y = 418;

int widget1_corner2_x = 478;
int widget1_corner1_y = 732;

double widget1_data[128] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

void mainDraw() {
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


    display.setFont(text1_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text1_cursor_x, text1_cursor_y);
    display.print(text1_content);

    display.setFont(text2_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text2_cursor_x, text2_cursor_y);
    display.print(text2_content);

    display.setFont(text3_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text3_cursor_x, text3_cursor_y);
    display.print(text3_content);

    display.setFont(text4_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text4_cursor_x, text4_cursor_y);
    display.print(text4_content);

    display.setFont(text5_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text5_cursor_x, text5_cursor_y);
    display.print(text5_content);

    display.setFont(text6_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text6_cursor_x, text6_cursor_y);
    display.print(text6_content);

    display.setFont(text7_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text7_cursor_x, text7_cursor_y);
    display.print(text7_content);

    display.setFont(text8_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text8_cursor_x, text8_cursor_y);
    display.print(text8_content);

    display.setFont(text9_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text9_cursor_x, text9_cursor_y);
    display.print(text9_content);

    display.setFont(text10_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text10_cursor_x, text10_cursor_y);
    display.print(text10_content);

    display.setFont(text11_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text11_cursor_x, text11_cursor_y);
    display.print(text11_content);

    display.setFont(text12_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text12_cursor_x, text12_cursor_y);
    display.print(text12_content);

    display.setFont(text13_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text13_cursor_x, text13_cursor_y);
    display.print(text13_content);

    display.setFont(text14_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text14_cursor_x, text14_cursor_y);
    display.print(text14_content);

   int widget1_textMargin = 68;
   double widget1_minData = 1e9F;
   double widget1_maxData = -1e9F;

   for (int i = 0; i < widget1_n; ++i)
   {
       widget1_minData = min(widget1_minData, widget1_data[i]);
       widget1_maxData = max(widget1_maxData, widget1_data[i]);
   }

   double widget1_span = max(0.3D, fabs(widget1_maxData - widget1_minData));
   int widget1_prev_x = -1;
   int widget1_prev_y = -1;

   for (int i = 0; i < widget1_n; ++i)
   {
       int tx = widget1_corner1_x + i * (widget1_corner2_x - widget1_corner1_x - widget1_textMargin) / widget1_n;
       double v = widget1_data[i];
       int h = ((v - widget1_minData) * abs(widget1_corner1_y - widget1_corner2_y) / widget1_span);
       int ty = widget1_corner1_y - h;

       if (i)
       {
           double dy = (ty - widget1_prev_y) / ((widget1_corner2_x - widget1_corner1_x - widget1_textMargin) / widget1_n);

           for (int j = 0; j < (widget1_corner2_x - widget1_corner1_x) / widget1_n + 1; ++j)
               display.drawGradientLine(widget1_prev_x + j,
                                        round(widget1_prev_y + dy * j),
                                        widget1_prev_x + j,
                                        widget1_corner1_y, 3, 7);
       }

       widget1_prev_x = tx;
       widget1_prev_y = ty;
   }

   for (int i = 0; i < widget1_n; ++i)
   {
       int tx = widget1_corner1_x + i * (widget1_corner2_x - widget1_corner1_x - widget1_textMargin) / widget1_n;
       double v = widget1_data[i];
       int h = ((v - widget1_minData) * abs(widget1_corner1_y - widget1_corner2_y) / widget1_span);
       int ty = widget1_corner1_y - h;

       if (i)
       {
           display.drawThickLine(widget1_prev_x, widget1_prev_y, tx, ty, 0, 5.0);
       }

       widget1_prev_x = tx;
       widget1_prev_y = ty;
   }

   for (int i = 0; i < 4; ++i) { display.setFont();
       display.drawFastHLine(widget1_corner1_x, widget1_corner2_y + i * (widget1_corner1_y - widget1_corner2_y) / 4, widget1_corner2_x - widget1_corner1_x, 4);
       display.setCursor(widget1_corner2_x - widget1_textMargin + 10, widget1_corner1_y + (4 - i) * (widget1_corner2_y - widget1_corner1_y) / 4 + 23);
       display.setTextColor(0, 7);
       display.setTextSize(3);
       display.print(String((widget1_minData + (widget1_maxData - widget1_minData) * (4 - i) / 4)));
   }
   for (int i = 0; i < 5; ++i)
       display.drawFastVLine(widget1_corner1_x + i * (widget1_corner2_x - widget1_corner1_x) / 5, widget1_corner2_y, widget1_corner1_y - widget1_corner2_y, 4);

   display.drawFastVLine(widget1_corner2_x - widget1_textMargin + 2, widget1_corner2_y, widget1_corner1_y - widget1_corner2_y, 4);
   display.drawThickLine(widget1_corner1_x, widget1_corner1_y, widget1_corner2_x, widget1_corner1_y, 0, 3);

}
