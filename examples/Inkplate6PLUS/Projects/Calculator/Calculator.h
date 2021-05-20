#include "Arduino.h"
#include "Fonts/FreeSansBold24pt7b.h"
#include "Inkplate.h"

extern Inkplate display;

int rect0_a_x = 900;
int rect0_a_y = 650;
int rect0_b_x = 1000;
int rect0_b_y = 750;
int rect0_fill = -1;
int rect0_radius = -1;
int rect0_color = BLACK;

int rect1_a_x = 800;
int rect1_a_y = 650;
int rect1_b_x = 900;
int rect1_b_y = 750;
int rect1_fill = -1;
int rect1_radius = -1;
int rect1_color = BLACK;

int rect2_a_x = 700;
int rect2_a_y = 650;
int rect2_b_x = 800;
int rect2_b_y = 750;
int rect2_fill = -1;
int rect2_radius = -1;
int rect2_color = BLACK;

int rect3_a_x = 600;
int rect3_a_y = 650;
int rect3_b_x = 700;
int rect3_b_y = 750;
int rect3_fill = -1;
int rect3_radius = -1;
int rect3_color = BLACK;

int rect4_a_x = 900;
int rect4_a_y = 550;
int rect4_b_x = 1000;
int rect4_b_y = 650;
int rect4_fill = -1;
int rect4_radius = -1;
int rect4_color = BLACK;

int rect5_a_x = 800;
int rect5_a_y = 550;
int rect5_b_x = 900;
int rect5_b_y = 650;
int rect5_fill = -1;
int rect5_radius = -1;
int rect5_color = BLACK;

int rect6_a_x = 700;
int rect6_a_y = 550;
int rect6_b_x = 800;
int rect6_b_y = 650;
int rect6_fill = -1;
int rect6_radius = -1;
int rect6_color = BLACK;

int rect7_a_x = 600;
int rect7_a_y = 550;
int rect7_b_x = 700;
int rect7_b_y = 650;
int rect7_fill = -1;
int rect7_radius = -1;
int rect7_color = BLACK;

int rect8_a_x = 900;
int rect8_a_y = 450;
int rect8_b_x = 1000;
int rect8_b_y = 550;
int rect8_fill = -1;
int rect8_radius = -1;
int rect8_color = BLACK;

int rect9_a_x = 800;
int rect9_a_y = 450;
int rect9_b_x = 900;
int rect9_b_y = 550;
int rect9_fill = -1;
int rect9_radius = -1;
int rect9_color = BLACK;

int rect10_a_x = 700;
int rect10_a_y = 450;
int rect10_b_x = 800;
int rect10_b_y = 550;
int rect10_fill = -1;
int rect10_radius = -1;
int rect10_color = BLACK;

int rect11_a_x = 600;
int rect11_a_y = 450;
int rect11_b_x = 700;
int rect11_b_y = 550;
int rect11_fill = -1;
int rect11_radius = -1;
int rect11_color = BLACK;

int rect12_a_x = 900;
int rect12_a_y = 350;
int rect12_b_x = 1000;
int rect12_b_y = 450;
int rect12_fill = -1;
int rect12_radius = -1;
int rect12_color = BLACK;

int rect13_a_x = 800;
int rect13_a_y = 350;
int rect13_b_x = 900;
int rect13_b_y = 450;
int rect13_fill = -1;
int rect13_radius = -1;
int rect13_color = BLACK;

int rect14_a_x = 700;
int rect14_a_y = 350;
int rect14_b_x = 800;
int rect14_b_y = 450;
int rect14_fill = -1;
int rect14_radius = -1;
int rect14_color = BLACK;

int rect15_a_x = 600;
int rect15_a_y = 350;
int rect15_b_x = 700;
int rect15_b_y = 450;
int rect15_fill = -1;
int rect15_radius = -1;
int rect15_color = BLACK;

int rect16_a_x = 600;
int rect16_a_y = 100;
int rect16_b_x = 1000;
int rect16_b_y = 300;
int rect16_fill = -1;
int rect16_radius = -1;
int rect16_color = BLACK;

int rect17_a_x = 50;
int rect17_a_y = 50;
int rect17_b_x = 550;
int rect17_b_y = 730;
int rect17_fill = -1;
int rect17_radius = -1;
int rect17_color = BLACK;

String text2_content = "0";
int text2_cursor_x = 740;
int text2_cursor_y = 715;
const GFXfont *text2_font = &FreeSansBold24pt7b;

String text3_content = "1";
int text3_cursor_x = 640;
int text3_cursor_y = 620;
const GFXfont *text3_font = &FreeSansBold24pt7b;

String text4_content = "2";
int text4_cursor_x = 740;
int text4_cursor_y = 620;
const GFXfont *text4_font = &FreeSansBold24pt7b;

String text5_content = "3";
int text5_cursor_x = 840;
int text5_cursor_y = 620;
const GFXfont *text5_font = &FreeSansBold24pt7b;

String text6_content = "+";
int text6_cursor_x = 940;
int text6_cursor_y = 720;
const GFXfont *text6_font = &FreeSansBold24pt7b;

String text7_content = "4";
int text7_cursor_x = 640;
int text7_cursor_y = 520;
const GFXfont *text7_font = &FreeSansBold24pt7b;

String text8_content = "5";
int text8_cursor_x = 740;
int text8_cursor_y = 520;
const GFXfont *text8_font = &FreeSansBold24pt7b;

String text9_content = "6";
int text9_cursor_x = 840;
int text9_cursor_y = 520;
const GFXfont *text9_font = &FreeSansBold24pt7b;

String text10_content = "-";
int text10_cursor_x = 940;
int text10_cursor_y = 620;
const GFXfont *text10_font = &FreeSansBold24pt7b;

String text11_content = "7";
int text11_cursor_x = 640;
int text11_cursor_y = 420;
const GFXfont *text11_font = &FreeSansBold24pt7b;

String text12_content = "8";
int text12_cursor_x = 740;
int text12_cursor_y = 420;
const GFXfont *text12_font = &FreeSansBold24pt7b;

String text13_content = "9";
int text13_cursor_x = 840;
int text13_cursor_y = 420;
const GFXfont *text13_font = &FreeSansBold24pt7b;

String text14_content = "x";
int text14_cursor_x = 940;
int text14_cursor_y = 520;
const GFXfont *text14_font = &FreeSansBold24pt7b;

String text15_content = "/";
int text15_cursor_x = 940;
int text15_cursor_y = 420;
const GFXfont *text15_font = &FreeSansBold24pt7b;

String text16_content = "=";
int text16_cursor_x = 840;
int text16_cursor_y = 720;
const GFXfont *text16_font = &FreeSansBold24pt7b;

String text17_content = ".";
int text17_cursor_x = 640;
int text17_cursor_y = 720;
const GFXfont *text17_font = &FreeSansBold24pt7b;

String text18_content = "";
int text18_cursor_x = 800;
int text18_cursor_y = 260;
const GFXfont *text18_font = &FreeSansBold24pt7b;

String text19_content = "";
int text19_cursor_x = 50;
int text19_cursor_y = 700;
const GFXfont *text19_font = &FreeSansBold24pt7b;

int rect18_a_x = 800;
int rect18_a_y = 20;
int rect18_b_x = 1000;
int rect18_b_y = 73;
int rect18_fill = -1;
int rect18_radius = -1;
int rect18_color = BLACK;

String text20_content = "Refresh";
int text20_cursor_x = 820;
int text20_cursor_y = 60;
const GFXfont *text20_font = &FreeSansBold24pt7b;

int rect19_a_x = 600;
int rect19_a_y = 20;
int rect19_b_x = 800;
int rect19_b_y = 73;
int rect19_fill = -1;
int rect19_radius = -1;
int rect19_color = BLACK;

String text21_content = "Clear";
int text21_cursor_x = 620;
int text21_cursor_y = 60;
const GFXfont *text21_font = &FreeSansBold24pt7b;

int rect20_a_x = 50;
int rect20_a_y = 50;
int rect20_b_x = 200;
int rect20_b_y = 100;
int rect20_fill = -1;
int rect20_radius = -1;
int rect20_color = BLACK;

String text22_content = "Clear";
int text22_cursor_x = 60;
int text22_cursor_y = 95;
const GFXfont *text22_font = &FreeSansBold24pt7b;


void mainDraw() {
    //Serial.println("Main draw");

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

    if (rect8_radius != -1 && rect8_fill != -1)
       display.fillRoundRect(rect8_a_x, rect8_a_y, rect8_b_x - rect8_a_x, rect8_b_y - rect8_a_y, rect8_radius, rect8_color);
   else if (rect8_radius != -1 && rect8_fill == -1)
       display.drawRoundRect(rect8_a_x, rect8_a_y, rect8_b_x - rect8_a_x, rect8_b_y - rect8_a_y, rect8_radius, rect8_color);
   else if (rect8_radius == -1 && rect8_fill != -1)
       display.fillRect(rect8_a_x, rect8_a_y, rect8_b_x - rect8_a_x, rect8_b_y - rect8_a_y, rect8_color);
   else if (rect8_radius == -1 && rect8_fill == -1)
       display.drawRect(rect8_a_x, rect8_a_y, rect8_b_x - rect8_a_x, rect8_b_y - rect8_a_y, rect8_color);

    if (rect9_radius != -1 && rect9_fill != -1)
       display.fillRoundRect(rect9_a_x, rect9_a_y, rect9_b_x - rect9_a_x, rect9_b_y - rect9_a_y, rect9_radius, rect9_color);
   else if (rect9_radius != -1 && rect9_fill == -1)
       display.drawRoundRect(rect9_a_x, rect9_a_y, rect9_b_x - rect9_a_x, rect9_b_y - rect9_a_y, rect9_radius, rect9_color);
   else if (rect9_radius == -1 && rect9_fill != -1)
       display.fillRect(rect9_a_x, rect9_a_y, rect9_b_x - rect9_a_x, rect9_b_y - rect9_a_y, rect9_color);
   else if (rect9_radius == -1 && rect9_fill == -1)
       display.drawRect(rect9_a_x, rect9_a_y, rect9_b_x - rect9_a_x, rect9_b_y - rect9_a_y, rect9_color);

    if (rect10_radius != -1 && rect10_fill != -1)
       display.fillRoundRect(rect10_a_x, rect10_a_y, rect10_b_x - rect10_a_x, rect10_b_y - rect10_a_y, rect10_radius, rect10_color);
   else if (rect10_radius != -1 && rect10_fill == -1)
       display.drawRoundRect(rect10_a_x, rect10_a_y, rect10_b_x - rect10_a_x, rect10_b_y - rect10_a_y, rect10_radius, rect10_color);
   else if (rect10_radius == -1 && rect10_fill != -1)
       display.fillRect(rect10_a_x, rect10_a_y, rect10_b_x - rect10_a_x, rect10_b_y - rect10_a_y, rect10_color);
   else if (rect10_radius == -1 && rect10_fill == -1)
       display.drawRect(rect10_a_x, rect10_a_y, rect10_b_x - rect10_a_x, rect10_b_y - rect10_a_y, rect10_color);

    if (rect11_radius != -1 && rect11_fill != -1)
       display.fillRoundRect(rect11_a_x, rect11_a_y, rect11_b_x - rect11_a_x, rect11_b_y - rect11_a_y, rect11_radius, rect11_color);
   else if (rect11_radius != -1 && rect11_fill == -1)
       display.drawRoundRect(rect11_a_x, rect11_a_y, rect11_b_x - rect11_a_x, rect11_b_y - rect11_a_y, rect11_radius, rect11_color);
   else if (rect11_radius == -1 && rect11_fill != -1)
       display.fillRect(rect11_a_x, rect11_a_y, rect11_b_x - rect11_a_x, rect11_b_y - rect11_a_y, rect11_color);
   else if (rect11_radius == -1 && rect11_fill == -1)
       display.drawRect(rect11_a_x, rect11_a_y, rect11_b_x - rect11_a_x, rect11_b_y - rect11_a_y, rect11_color);

    if (rect12_radius != -1 && rect12_fill != -1)
       display.fillRoundRect(rect12_a_x, rect12_a_y, rect12_b_x - rect12_a_x, rect12_b_y - rect12_a_y, rect12_radius, rect12_color);
   else if (rect12_radius != -1 && rect12_fill == -1)
       display.drawRoundRect(rect12_a_x, rect12_a_y, rect12_b_x - rect12_a_x, rect12_b_y - rect12_a_y, rect12_radius, rect12_color);
   else if (rect12_radius == -1 && rect12_fill != -1)
       display.fillRect(rect12_a_x, rect12_a_y, rect12_b_x - rect12_a_x, rect12_b_y - rect12_a_y, rect12_color);
   else if (rect12_radius == -1 && rect12_fill == -1)
       display.drawRect(rect12_a_x, rect12_a_y, rect12_b_x - rect12_a_x, rect12_b_y - rect12_a_y, rect12_color);

    if (rect13_radius != -1 && rect13_fill != -1)
       display.fillRoundRect(rect13_a_x, rect13_a_y, rect13_b_x - rect13_a_x, rect13_b_y - rect13_a_y, rect13_radius, rect13_color);
   else if (rect13_radius != -1 && rect13_fill == -1)
       display.drawRoundRect(rect13_a_x, rect13_a_y, rect13_b_x - rect13_a_x, rect13_b_y - rect13_a_y, rect13_radius, rect13_color);
   else if (rect13_radius == -1 && rect13_fill != -1)
       display.fillRect(rect13_a_x, rect13_a_y, rect13_b_x - rect13_a_x, rect13_b_y - rect13_a_y, rect13_color);
   else if (rect13_radius == -1 && rect13_fill == -1)
       display.drawRect(rect13_a_x, rect13_a_y, rect13_b_x - rect13_a_x, rect13_b_y - rect13_a_y, rect13_color);

    if (rect14_radius != -1 && rect14_fill != -1)
       display.fillRoundRect(rect14_a_x, rect14_a_y, rect14_b_x - rect14_a_x, rect14_b_y - rect14_a_y, rect14_radius, rect14_color);
   else if (rect14_radius != -1 && rect14_fill == -1)
       display.drawRoundRect(rect14_a_x, rect14_a_y, rect14_b_x - rect14_a_x, rect14_b_y - rect14_a_y, rect14_radius, rect14_color);
   else if (rect14_radius == -1 && rect14_fill != -1)
       display.fillRect(rect14_a_x, rect14_a_y, rect14_b_x - rect14_a_x, rect14_b_y - rect14_a_y, rect14_color);
   else if (rect14_radius == -1 && rect14_fill == -1)
       display.drawRect(rect14_a_x, rect14_a_y, rect14_b_x - rect14_a_x, rect14_b_y - rect14_a_y, rect14_color);

    if (rect15_radius != -1 && rect15_fill != -1)
       display.fillRoundRect(rect15_a_x, rect15_a_y, rect15_b_x - rect15_a_x, rect15_b_y - rect15_a_y, rect15_radius, rect15_color);
   else if (rect15_radius != -1 && rect15_fill == -1)
       display.drawRoundRect(rect15_a_x, rect15_a_y, rect15_b_x - rect15_a_x, rect15_b_y - rect15_a_y, rect15_radius, rect15_color);
   else if (rect15_radius == -1 && rect15_fill != -1)
       display.fillRect(rect15_a_x, rect15_a_y, rect15_b_x - rect15_a_x, rect15_b_y - rect15_a_y, rect15_color);
   else if (rect15_radius == -1 && rect15_fill == -1)
       display.drawRect(rect15_a_x, rect15_a_y, rect15_b_x - rect15_a_x, rect15_b_y - rect15_a_y, rect15_color);

    if (rect16_radius != -1 && rect16_fill != -1)
       display.fillRoundRect(rect16_a_x, rect16_a_y, rect16_b_x - rect16_a_x, rect16_b_y - rect16_a_y, rect16_radius, rect16_color);
   else if (rect16_radius != -1 && rect16_fill == -1)
       display.drawRoundRect(rect16_a_x, rect16_a_y, rect16_b_x - rect16_a_x, rect16_b_y - rect16_a_y, rect16_radius, rect16_color);
   else if (rect16_radius == -1 && rect16_fill != -1)
       display.fillRect(rect16_a_x, rect16_a_y, rect16_b_x - rect16_a_x, rect16_b_y - rect16_a_y, rect16_color);
   else if (rect16_radius == -1 && rect16_fill == -1)
       display.drawRect(rect16_a_x, rect16_a_y, rect16_b_x - rect16_a_x, rect16_b_y - rect16_a_y, rect16_color);

    if (rect17_radius != -1 && rect17_fill != -1)
       display.fillRoundRect(rect17_a_x, rect17_a_y, rect17_b_x - rect17_a_x, rect17_b_y - rect17_a_y, rect17_radius, rect17_color);
   else if (rect17_radius != -1 && rect17_fill == -1)
       display.drawRoundRect(rect17_a_x, rect17_a_y, rect17_b_x - rect17_a_x, rect17_b_y - rect17_a_y, rect17_radius, rect17_color);
   else if (rect17_radius == -1 && rect17_fill != -1)
       display.fillRect(rect17_a_x, rect17_a_y, rect17_b_x - rect17_a_x, rect17_b_y - rect17_a_y, rect17_color);
   else if (rect17_radius == -1 && rect17_fill == -1)
       display.drawRect(rect17_a_x, rect17_a_y, rect17_b_x - rect17_a_x, rect17_b_y - rect17_a_y, rect17_color);

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

    display.setFont(text15_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text15_cursor_x, text15_cursor_y);
    display.print(text15_content);

    display.setFont(text16_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text16_cursor_x, text16_cursor_y);
    display.print(text16_content);

    display.setFont(text17_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text17_cursor_x, text17_cursor_y);
    display.print(text17_content);

    display.setFont(text18_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text18_cursor_x, text18_cursor_y);
    display.print(text18_content);

    display.setFont(text19_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text19_cursor_x, text19_cursor_y);
    display.print(text19_content);

    if (rect18_radius != -1 && rect18_fill != -1)
       display.fillRoundRect(rect18_a_x, rect18_a_y, rect18_b_x - rect18_a_x, rect18_b_y - rect18_a_y, rect18_radius, rect18_color);
   else if (rect18_radius != -1 && rect18_fill == -1)
       display.drawRoundRect(rect18_a_x, rect18_a_y, rect18_b_x - rect18_a_x, rect18_b_y - rect18_a_y, rect18_radius, rect18_color);
   else if (rect18_radius == -1 && rect18_fill != -1)
       display.fillRect(rect18_a_x, rect18_a_y, rect18_b_x - rect18_a_x, rect18_b_y - rect18_a_y, rect18_color);
   else if (rect18_radius == -1 && rect18_fill == -1)
       display.drawRect(rect18_a_x, rect18_a_y, rect18_b_x - rect18_a_x, rect18_b_y - rect18_a_y, rect18_color);

    display.setFont(text20_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text20_cursor_x, text20_cursor_y);
    display.print(text20_content);


    if (rect19_radius != -1 && rect19_fill != -1)
       display.fillRoundRect(rect19_a_x, rect19_a_y, rect19_b_x - rect19_a_x, rect19_b_y - rect19_a_y, rect19_radius, rect19_color);
    else if (rect19_radius != -1 && rect19_fill == -1)
       display.drawRoundRect(rect19_a_x, rect19_a_y, rect19_b_x - rect19_a_x, rect19_b_y - rect19_a_y, rect19_radius, rect19_color);
    else if (rect19_radius == -1 && rect19_fill != -1)
       display.fillRect(rect19_a_x, rect19_a_y, rect19_b_x - rect19_a_x, rect19_b_y - rect19_a_y, rect19_color);
    else if (rect19_radius == -1 && rect19_fill == -1)
       display.drawRect(rect19_a_x, rect19_a_y, rect19_b_x - rect19_a_x, rect19_b_y - rect19_a_y, rect19_color);

    display.setFont(text21_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text21_cursor_x, text21_cursor_y);
    display.print(text21_content);

    if (rect20_radius != -1 && rect20_fill != -1)
       display.fillRoundRect(rect20_a_x, rect20_a_y, rect20_b_x - rect20_a_x, rect20_b_y - rect20_a_y, rect20_radius, rect20_color);
    else if (rect20_radius != -1 && rect20_fill == -1)
       display.drawRoundRect(rect20_a_x, rect20_a_y, rect20_b_x - rect20_a_x, rect20_b_y - rect20_a_y, rect20_radius, rect20_color);
    else if (rect20_radius == -1 && rect20_fill != -1)
       display.fillRect(rect20_a_x, rect20_a_y, rect20_b_x - rect20_a_x, rect20_b_y - rect20_a_y, rect20_color);
    else if (rect20_radius == -1 && rect20_fill == -1)
       display.drawRect(rect20_a_x, rect20_a_y, rect20_b_x - rect20_a_x, rect20_b_y - rect20_a_y, rect20_color);

    display.setFont(text22_font);
    display.setTextColor(BLACK, WHITE);    display.setTextSize(1);    display.setCursor(text22_cursor_x, text22_cursor_y);
    display.print(text22_content);
}
