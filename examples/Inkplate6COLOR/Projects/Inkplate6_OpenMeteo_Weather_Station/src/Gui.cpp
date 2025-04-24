#include "Gui.h"
#include "Network.h"
#include "WeatherData.h"
#include <Arduino.h>
#include <Inkplate.h>

// all the weather icons
#include "binary_Icons/icon_s_clear_sky.h"
#include "binary_Icons/icon_s_fog.h"
#include "binary_Icons/icon_s_gray.h"
#include "binary_Icons/icon_s_moon.h"
#include "binary_Icons/icon_s_partly_cloudy.h"
#include "binary_Icons/icon_s_rain.h"
#include "binary_Icons/icon_s_snow.h"
#include "binary_Icons/icon_s_storm.h"
#include "binary_Icons/icon_s_thermometer.h"

// all the battery icons
#include "binary_Icons/icon_s_full_battery.h"
#include "binary_Icons/icon_s_half_battery.h"
#include "binary_Icons/icon_s_high_battery.h"
#include "binary_Icons/icon_s_low_battery.h"

// font
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans18pt7b.h"
#include "fonts/FreeSans24pt7b.h"
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans6pt7b.h"
#include "fonts/FreeSans32pt7b.h"

Gui::Gui(Inkplate &inkplate) : inkplate(inkplate)
{
}

void Gui::drawBackground()
{
    //main info
    inkplate.fillRect(0, 0, 600, 150, 7);

    // user info
    //inkplate.fillRect(530, 0, 270, 155, 0);

    // add info
    //inkplate.fillRect(0, 155, 200, 295, 0);
    //inkplate.drawLine(0, 449, 200, 449, 7);

    //graph
    //inkplate.fillRect(200, 155, 600, 295, 7);

    // weekly
    inkplate.fillRect(0, 310, 600, 140, 0);
}

void Gui::wifiError()
{
    inkplate.clearDisplay();
    inkplate.setTextColor(0);
    inkplate.setFont(&FreeSans18pt7b);
    inkplate.setCursor(50, 150);
    inkplate.print("WiFi connection failed.");
    inkplate.setCursor(50, 200);
    inkplate.print("Check credentials or try again.");
    inkplate.display();
}

void Gui::apiError()
{
    inkplate.clearDisplay();
    inkplate.setTextColor(0);
    inkplate.setFont(&FreeSans18pt7b);
    inkplate.setCursor(50, 150);
    inkplate.print("HTTP request failed.");
    inkplate.setCursor(50, 200);
    inkplate.print("Check API URL or try again.");
    inkplate.display();
}

// Weather Icons based on open-meteo api code
const uint8_t *Gui::getWeatherIcon(int code)
{
    switch (code)
    {
    case 0:
        return icon_s_clear_sky;
    case 1:
    case 2:
    case 3:
        return icon_s_partly_cloudy;
    case 45:
    case 48:
        return icon_s_fog;
    case 51:
    case 53:
    case 55:
    case 56:
    case 57:
    case 61:
    case 63:
    case 65:
    case 66:
    case 67:
    case 80:
    case 81:
    case 82:
        return icon_s_rain;
    case 71:
    case 73:
    case 75:
    case 77:
    case 85:
    case 86:
        return icon_s_snow;
    case 95:
    case 96:
    case 99:
        return icon_s_storm;
    default:
        return icon_s_gray;
    }
}

const uint8_t *Gui::getBatteryIcon(int percentage)
{
    if (percentage >= 75)
        return icon_s_full_battery;
    else if (percentage >= 50)
        return icon_s_high_battery;
    else if (percentage >= 25)
        return icon_s_half_battery;
    else
        return icon_s_low_battery;
}

// --- Draw Temperature & Precipitation Graph ---
void Gui::drawTemperaturePrecipGraph(WeatherData *weatherData, Network::UserInfo *userInfo)
{
    // Layout values for graph placement
    int graphX = 55;
    int graphY = 125;
    int graphWidth = 340;
    int graphHeight = 175;

    inkplate.setCursor(50, 125);
    inkplate.setFont(&FreeSans9pt7b);
    inkplate.setTextColor(0);
    inkplate.print("Hourly temperature and precipitation");

    int marginX = 20;
    int marginY = 20;
    int chartLeft = graphX + marginX;
    int chartBottom = graphY + graphHeight - marginY;
    int chartTop = graphY + marginY;
    float actualTempMin = 100, actualTempMax = -100;
    float precipMax = 100;

    // Find actual min/max for temperature and precipitation
    for (int i = 0; i < 6; i++)
    {
        if (weatherData->hourlyTemps[i] < actualTempMin)
            actualTempMin = weatherData->hourlyTemps[i];
        if (weatherData->hourlyTemps[i] > actualTempMax)
            actualTempMax = weatherData->hourlyTemps[i];
    }

    // Add padding but ensure min temperature doesn't go below zero (or another reasonable value)
    float paddedTempMin = actualTempMin - 2;
    float paddedTempMax = actualTempMax + 2;

    // Calculate the mid temperature as the average of min and max
    float paddedTempMid = (paddedTempMin + paddedTempMax) / 2;

    float tempRange = paddedTempMax - paddedTempMin;
    if (tempRange == 0)
        tempRange = 1; // Avoid division by zero
    float xStep = (graphWidth - 2 * marginX) / 5.0;

    // Draw axes
    inkplate.drawLine(chartLeft, chartTop, chartLeft, chartBottom, 0);                   // Y-axis
    inkplate.drawLine(chartLeft, chartBottom, graphX + graphWidth + 20, chartBottom, 0); // X-axis

    // Calculate Y positions for min, mid, and max temperatures
    int yMin = chartBottom - ((paddedTempMin - paddedTempMin) / tempRange) * (graphHeight - 2 * marginY);
    int yMax = chartBottom - ((paddedTempMax - paddedTempMin) / tempRange) * (graphHeight - 2 * marginY);
    int yMid = chartBottom - ((paddedTempMid - paddedTempMin) / tempRange) * (graphHeight - 2 * marginY);

    // Ensure at least 20px gap between the min, mid, and max temperature labels
    if (abs(yMax - yMin) < 20)
    {
        int offset = 20 - abs(yMax - yMin); // Calculate the gap required
        yMax = yMin + offset;               // Adjust the max label position to create the gap
    }

    if (abs(yMid - yMin) < 20)
    {
        int offset = 20 - abs(yMid - yMin); // Calculate the gap required
        yMid = yMin + offset;               // Adjust the mid label position to create the gap
    }

    if (abs(yMax - yMid) < 20)
    {
        int offset = 20 - abs(yMax - yMid); // Calculate the gap required
        yMax = yMid + offset;               // Adjust the max label position to create the gap
    }

    // Draw the temperature labels (Min, Mid, and Max)
    inkplate.setFont(&FreeSans9pt7b);
    inkplate.setTextColor(0);

    // Draw Min temperature label
    inkplate.setCursor(chartLeft - 70, yMin - 20);
    inkplate.print(paddedTempMin, 1); // Show temperature with 1 decimal place
    inkplate.print(userInfo->temperatureLabel);

    // Draw Mid temperature label
    inkplate.setCursor(chartLeft - 70, yMid);
    inkplate.print(paddedTempMid, 1); // Show temperature with 1 decimal place
    inkplate.print(userInfo->temperatureLabel);

    // Draw Max temperature label
    inkplate.setCursor(chartLeft - 70, yMax + 20);
    inkplate.print(paddedTempMax, 1); // Show temperature with 1 decimal place
    inkplate.print(userInfo->temperatureLabel);

    // Draw precipitation bars
    for (int i = 0; i < 6; i++)
    {
        int x = chartLeft + i * xStep;
        int barHeight = (precipMax > 0) ? (weatherData->hourlyPrecip[i] / precipMax) * (graphHeight - 2 * marginY) : 0;
        int y = chartBottom - barHeight;

        // Draw the precipitation bar
        inkplate.fillRect(x + 5, y + 10, 10, barHeight - 10, 3);

        // Draw precipitation value on top of the bar
        inkplate.setCursor(x + 5, y - 5);
        inkplate.print(weatherData->hourlyPrecip[i], 0); // Show precipitation with 1 decimal place
        inkplate.print("%");
    }

    // Draw temperature line
    for (int i = 0; i < 5; i++)
    {
        int x1 = chartLeft + i * xStep;
        int x2 = chartLeft + (i + 1) * xStep;

        int y1 =
            chartBottom - ((weatherData->hourlyTemps[i] - paddedTempMin) / tempRange) * (graphHeight - 2 * marginY);
        int y2 =
            chartBottom - ((weatherData->hourlyTemps[i + 1] - paddedTempMin) / tempRange) * (graphHeight - 2 * marginY);

        inkplate.drawLine(x1, y1, x2, y2, 0);
    }

    // Time labels under X-axis
    inkplate.setTextColor(0);
    for (int i = 0; i < 6; i++)
    {
        int x = chartLeft + i * xStep;
        inkplate.setCursor(x + 3, chartBottom + 20);
        inkplate.print(weatherData->hourlyTimes[i]);
    }
}

// --- Display All Weather Data ---
void Gui::displayWeatherData(WeatherData *weatherData, Network::UserInfo *userInfo)
{
    // Section 1: Main info
    inkplate.setFont(&FreeSans12pt7b);
    inkplate.setTextColor(0);

    inkplate.drawBitmap(10, 5, icon_s_gray, 48, 48, 0);
    inkplate.setCursor(70, 40);
    inkplate.print(userInfo->city);

    inkplate.setFont(&FreeSans12pt7b);
    // inkplate.drawBitmap(10, 55, icon_s_thermometer, 48, 48, 0);
    inkplate.setCursor(70, 90);
    inkplate.print(weatherData->currentTemp);
    inkplate.print(userInfo->temperatureLabel);

    inkplate.drawBitmap(10, 55, getWeatherIcon(weatherData->weatherCode), 48, 48, 0);
    inkplate.setCursor(165, 90);
    inkplate.println(weatherData->weatherDescription);

    // Section 2: User Info and Battery
    inkplate.setFont(&FreeSans12pt7b);
    inkplate.setTextColor(7);

    int yUser = 5;

    inkplate.drawBitmap(550, 5, getBatteryIcon(userInfo->batteryLevel), 48, 48, 7);

    yUser += 30;

    inkplate.setCursor(600, yUser + 2);
    inkplate.print(userInfo->batteryLevel);
    inkplate.println("%");

    yUser += 40;
    inkplate.setCursor(550, yUser);
    inkplate.println(userInfo->lastUpdatedDate);

    yUser += 30;
    inkplate.setCursor(550, yUser);
    inkplate.print("Last refresh: ");
    inkplate.println(userInfo->lastUpdatedTime);

    yUser += 30;
    inkplate.setCursor(550, yUser);
    inkplate.println(userInfo->username);

    // Section 3: Additional Info (feels like, wind, etc.)
    inkplate.setFont(&FreeSans9pt7b);
    inkplate.setTextColor(7);

    int y = 195;
    int xPos = 10;
    int yGap = 45;

    inkplate.setCursor(xPos, y);
    inkplate.print("Feels like: ");
    inkplate.print(weatherData->feelsLike);
    inkplate.print(userInfo->temperatureLabel);

    y += yGap;
    inkplate.setCursor(xPos, y);
    inkplate.print("Sunrise: ");
    inkplate.println(weatherData->sunrise);

    y += yGap;
    inkplate.setCursor(xPos, y);
    inkplate.print("Sunset: ");
    inkplate.println(weatherData->sunset);

    y += yGap;
    inkplate.setCursor(xPos, y);
    inkplate.print("UV index: ");
    inkplate.println(weatherData->uvIndex);

    y += yGap;
    inkplate.setCursor(xPos, y);
    inkplate.print("Wind: ");
    inkplate.print(weatherData->windSpeed);
    inkplate.print(userInfo->speedLabel);

    y += yGap;
    inkplate.setCursor(xPos, y);
    inkplate.print("Precipitation: ");
    inkplate.print(weatherData->precipitation);
    inkplate.print(" %");

    // Section 4: Weekly Forecast
    inkplate.setTextColor(1);

    int startX = 15;                      // Starting x-position for the weekly forecast
    int startY = 330;                      // Starting y-position for the weekly forecast
    int iconSize = 48;                     // Size of the icon
    int margin = 5;                        // Margin between elements
    int dayWidth = iconSize + margin + 30; // Space for icon + margin + text width

    // Loop through the 7-day forecast and display each day
    for (int i = 0; i < 7; i++)
    {
        inkplate.setFont(&FreeSans9pt7b);
        int xPos = startX + i * dayWidth;

        // Day name
        inkplate.setCursor(xPos + 15, startY);
        inkplate.setTextColor(1);
        inkplate.print(weatherData->dailyNames[i]);

        // Weather icon
        inkplate.setFont(&FreeSans6pt7b);
        inkplate.drawBitmap(xPos + 15, startY + 20, getWeatherIcon(weatherData->dailyWeatherCodes[i]), iconSize,
                            iconSize, 1);
        int tempYStart = startY + 20 + iconSize + margin + 5;

        // === Max Temp - Up Arrow Triangle ===
        int arrowX = xPos;
        int arrowY = tempYStart + 5;
        // Triangle pointing up
        inkplate.fillTriangle(arrowX, arrowY,         // bottom center
                              arrowX - 4, arrowY + 6, // bottom left
                              arrowX + 4, arrowY + 6, // bottom right
                              1                       // white color
        );
        // Max temp text next to it
        inkplate.setCursor(arrowX + 10, arrowY + 6);
        inkplate.print(weatherData->dailyMaxTemp[i]);
        inkplate.print(userInfo->temperatureLabel);
        // === Min Temp - Down Arrow Triangle ===
        arrowY += 20;
        // Triangle pointing down
        inkplate.fillTriangle(arrowX, arrowY + 6, // top center
                              arrowX - 4, arrowY, // bottom left
                              arrowX + 4, arrowY, // bottom right
                              1                   // white color
        );
        // Min temp text next to it
        inkplate.setCursor(arrowX + 10, arrowY + 6);
        inkplate.print(weatherData->dailyMinTemp[i]);
        inkplate.print(userInfo->temperatureLabel);
    }

    // Section 6: Graph info
    drawTemperaturePrecipGraph(weatherData, userInfo);

    // Finalize drawing
    inkplate.display();
}