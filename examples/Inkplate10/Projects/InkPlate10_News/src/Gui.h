#pragma once

#include <Inkplate.h>

class Gui
{
public:
    Gui(Inkplate &display);
    void wifiError();
    void showError(String message);
    void showNews(String newsData);

private:
    Inkplate &display;
};