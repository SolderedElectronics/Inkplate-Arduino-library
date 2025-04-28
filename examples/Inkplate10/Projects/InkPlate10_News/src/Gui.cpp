#include "Gui.h"

Gui::Gui(Inkplate &display) : display(display) {}

void Gui::wifiError()
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print("WiFi Connection Failed!");
    display.display();
}

void Gui::showError(String message)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print(message);
    display.display();
}

void Gui::showNews(String newsData)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 10);
    display.print("News Headlines:");
    display.setCursor(10, 30);
    display.print(newsData); // Simplified for now
    display.display();
}