#include "Inkplate.h"
#include "img1.h"
#include "img2.h"
#include "img3.h"
#include "img4.h"
#include "img5.h"
#include "img6.h"

Inkplate display(INKPLATE_3BIT);
void setup()
{
    // put your setup code here, to run once:
    display.begin();
    drawGrad();
    display.display();
    delay(5000);
    display.drawBitmap3Bit(0, 0, lacage, lacage_w, lacage_h);
    display.display();
    delay(5000);
    display.drawBitmap3Bit(0, 0, lighthouse, lighthouse_w, lighthouse_h);
    display.display();
    delay(5000);
    display.drawBitmap3Bit(0, 0, building, building_w, building_h);
    display.display();
    delay(5000);
    display.drawBitmap3Bit(0, 0, car, car_w, car_h);
    display.display();
    delay(5000);
    display.drawBitmap3Bit(0, 0, car2, car2_w, car2_h);
    display.display();
    delay(5000);
    display.drawBitmap3Bit(0, 0, car3, car3_w, car3_h);
    display.display();
}

void loop()
{
    // put your main code here, to run repeatedly:
}

void drawGrad()
{
    display.clearDisplay();
    for (int i = 0; i < 8; i++)
    {
        display.fillRect(i * 100, 0, 100, 700, i);
    }
}
