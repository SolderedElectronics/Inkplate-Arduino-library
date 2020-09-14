#include "Inkplate.h"
#include "SdFat.h"

const int n = 500;

Inkplate display(INKPLATE_1BIT);

void setup()
{
    Serial.begin(115200);

    display.begin();

    display.joinAP("e-radionica.com", "croduino");

    Serial.println("aaaa");
    delay(500);
}

void loop()
{
    display.clearDisplay();

    // Code block for generating random points and sorting them in a counter
    // clockwise direction.
    int xt[n];
    int yt[n];

    for (int i = 0; i < n; ++i)
    {
        xt[i] = random(100, 700);
        yt[i] = random(100, 500);
    }

    int k;
    for (int i = 0; i < n - 1; ++i)

        for (int j = i + 1; j < n; ++j)
            if (atan2(yt[j] - 300, xt[j] - 400) < atan2(yt[i] - 300, xt[i] - 400))
            {
                k = xt[i], xt[i] = xt[j], xt[j] = k;
                k = yt[i], yt[i] = yt[j], yt[j] = k;
            }

    display.clearDisplay();
    display.drawPolygon(xt, yt, n, 1);
    display.display();
    delay(5000);

    display.clearDisplay();
    display.fillPolygon(xt, yt, n, 1);
    display.display();
    delay(5000);
}