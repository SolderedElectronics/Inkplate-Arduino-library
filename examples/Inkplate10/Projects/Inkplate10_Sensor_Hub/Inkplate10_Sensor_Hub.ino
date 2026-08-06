/**
 **************************************************
 * @file        Inkplate10_Sensor_Hub.ino
 * @brief       Multi-sensor dashboard for Inkplate 10 reading Soldered easyC
 *              sensors over I2C and refreshing a GUI with partial updates.
 *
 * @details     Turns Inkplate 10 into a sensor hub. On boot the sketch starts
 *              I2C, initializes the display in 1-bit black/white mode
 *              (INKPLATE_1BIT), draws a static GUI frame, and initializes every
 *              attached sensor. In the main loop, sensor values are polled every
 *              SENSOR_READ_INTERVAL_MS (250 ms) and the GUI is redrawn every
 *              GUI_REFRESH_INTERVAL_MS (400 ms) using partial updates. Every
 *              FULL_REFRESH_EVERY_N_UPDATES (40) refreshes a full display update
 *              is performed instead, to clear partial-update ghosting.
 *
 *              Sensor reading, GUI layout and drawing live in the src/ folder
 *              (includes.h, sensorReadings.h/.cpp, gui.h/.cpp). Sensors are
 *              addressed on the following I2C addresses: potentiometer 0x30,
 *              soil sensor 0x31, PIR 0x32, hall sensor 0x34, INA219 current
 *              sensor 0x40, LSM6DSO IMU 0x6B and BME280 environmental sensor
 *              0x76. At least one sensor must initialize for readings to be
 *              meaningful; initSensors() reports whether any sensor was found.
 *
 *              Expected output: a live sensor dashboard on the e-paper display
 *              that updates roughly every 400 ms, and Serial output stating
 *              "Inkplate Sensor Hub started." plus whether at least one sensor
 *              initialized.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable, easyC sensors (INA219, BME280,
 *               LSM6DSO, potentiometer, soil, PIR, hall)
 * - Libraries:  INA219-SOLDERED, BME280-SOLDERED, LSM6DSO-SOLDERED
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Install the INA219, BME280 and LSM6DSO Soldered libraries.
 * 3) Connect the easyC sensors to the Inkplate easyC/I2C connector.
 * 4) Upload the sketch and open Serial Monitor at 115200 baud.
 * 5) The dashboard appears on the display and refreshes continuously.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @license     GNU GPL V3
 **************************************************/

#include "src/includes.h"
#include "src/sensorReadings.h"
#include "src/gui.h"

Inkplate display(INKPLATE_1BIT);

SensorData sensorData;

static uint32_t lastSensorReadMs = 0;
static uint32_t lastGuiRefreshMs = 0;
static uint32_t updateCounter = 0;
static uint8_t partialRefreshCounter = 0;

void setup()
{
    Serial.begin(115200);
    delay(500);

    Wire.begin();

    display.begin();
    display.setRotation(0);

    guiInit();
    guiDrawStaticFrame();
    display.display();

    bool sensorInitOk = initSensors();

    Serial.println();
    Serial.println("Inkplate Sensor Hub started.");
    Serial.print("At least one sensor initialized: ");
    Serial.println(sensorInitOk ? "YES" : "NO");
}

void loop()
{
    uint32_t now = millis();

    if (now - lastSensorReadMs >= SENSOR_READ_INTERVAL_MS)
    {
        lastSensorReadMs = now;
        readAllSensors(sensorData);
    }

    if (now - lastGuiRefreshMs >= GUI_REFRESH_INTERVAL_MS)
    {
        lastGuiRefreshMs = now;

        bool doFullRefresh = false;
        partialRefreshCounter++;

        if (partialRefreshCounter >= FULL_REFRESH_EVERY_N_UPDATES)
        {
            partialRefreshCounter = 0;
            doFullRefresh = true;
        }

        guiDrawData(sensorData, updateCounter, doFullRefresh);

        if (doFullRefresh)
        {
            display.display();
        }
        else
        {
            display.partialUpdate();
        }

        updateCounter++;
    }
}