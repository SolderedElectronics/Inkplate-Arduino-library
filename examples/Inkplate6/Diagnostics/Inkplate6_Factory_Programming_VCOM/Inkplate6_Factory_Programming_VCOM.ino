/**
 **************************************************
 * @file        Inkplate6_Factory_Programming_VCOM.ino
 * @brief       Factory utility for Inkplate 6: program panel VCOM, run hardware
 *              self-tests, and expose peripheral-mode Serial commands.
 *
 * @details     This sketch is intended for factory/production use on Inkplate 6
 *              (including Inkplate6V2). It performs a first-boot provisioning
 *              flow to validate hardware and program the e-paper VCOM voltage.
 *
 *              First boot flow:
 *              - Performs an I2C sanity check (GPIO expander reachability) and
 *                runs peripheral tests (see test.h / test.cpp).
 *              - Prompts the operator over Serial (115200 baud) to enter the
 *                panel VCOM voltage (typically negative, range 0.0 to -5.0 V),
 *                then programs it into EEPROM using setVcom().
 *              - Shows a splash image and prints the programmed VCOM value on
 *                the display.
 *
 *              Subsequent boots:
 *              - Detects that VCOM was already programmed, enables the e-paper
 *                power rails (einkOn()) and reads back the stored VCOM value.
 *              - Enters "peripheral mode": a Serial-driven command loop that
 *                forwards commands to the Inkplate peripheral interface (see
 *                Peripheral.h). Peripheral mode processes Serial input
 *                continuously, so make sure your terminal does not inject
 *                unexpected line endings or extra characters if commands are
 *                strict.
 *
 *              Display modes: 1-bit (BW) is used for initialization and fast
 *              updates, and the sketch switches to 3-bit grayscale only to draw
 *              the splash bitmap.
 *
 *              einkOn()/einkOff() control the e-paper power supply and are
 *              intended for controlled factory/service workflows - do not toggle
 *              them repeatedly in normal applications, as incorrect use may
 *              damage the display hardware. Factory test requirements depend on
 *              test.cpp: missing WiFi credentials, I2C slave or microSD may cause
 *              tests to fail and stop the process.
 *
 *              Expected output: test status messages, the VCOM prompt,
 *              success/failure logs and peripheral-mode interaction on the Serial
 *              Monitor; a splash/demo image with the programmed VCOM voltage
 *              printed near the bottom on the e-paper.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6 / Inkplate 6 V2, USB cable
 * - Extra:      microSD card (formatted, any content), EasyC I2C slave device
 *               for factory tests, WiFi credentials in test.cpp if the tests
 *               require network
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) (Factory) Connect required test hardware:
 *    - Insert a formatted microSD card.
 *    - Connect an EasyC I2C slave device at the address expected by test.cpp
 *      (0x30 by default). If you don't have one, flash the helper firmware from
 *      the InkplateEasyCTester folder onto a compatible Dasduino board and use
 *      it as the I2C slave.
 * 3) Open Serial Monitor at 115200 baud.
 * 4) Upload the sketch. On first startup it will:
 *    - Run peripheral tests and print results to Serial.
 *    - Prompt for VCOM voltage; enter the value (include the '-' sign when
 *      required) until programming succeeds.
 * 5) After the splash screen appears, the device stays in peripheral mode.
 *    Send supported peripheral-mode commands over Serial (see Peripheral.h /
 *    Inkplate peripheral-mode documentation).
 *
 * @warning     VCOM programming is limited: the panel VCOM can be programmed a
 *              finite number of times (typically ~100 writes). Avoid repeated
 *              programming and use it only when necessary.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

// Include needed libraries in the sketch
#include "Inkplate.h"
#include "Wire.h"

// Include our functions and image
#include "Peripheral.h"
#include "test.h"
#include "image.h"

// Create object on Inkplate library and set library to work in monochorme mode
Inkplate display(INKPLATE_1BIT);

// If you want to write new VCOM voltage and perform all tests change this number
const int EEPROMaddress = 0;

// Peripheral mode variables and arrays
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];

void setup()
{
    Serial.begin(115200);
    display.setTextSize(3);
    EEPROM.begin(512);
    Wire.begin();

    // Wakeup button
    pinMode(GPIO_NUM_36, INPUT);

    // Setting default value for safety
    double vcomVoltage = -2.35;

    // Check for the first run of this code. If it is first run, check the I2C bus.
    bool isFirstStartup = (EEPROM.read(EEPROMaddress) != 170);
    if (isFirstStartup)
    {
        // Try to ping first expander.
        Wire.setTimeOut(1000);
        Wire.beginTransmission(IO_INT_ADDR);
        int result = Wire.endTransmission();

        if (result == 5)
        {
            Serial.println("I2C Bus Error!");
            failHandler(true);
        }
    }

    // Init the Inkplate library (after the check of the I2C bus).
    display.begin();

    if (isFirstStartup)
    {
        // Test all the peripherals
        testPeripheral();
        
        while (true)
        {
          // Get VCOM voltage from serial from user
          uint8_t flag = getVCOMFromSerial(&vcomVoltage);

          // Show the user the entered VCOM voltage
          Serial.print("Entered VCOM: ");
          Serial.println(vcomVoltage);
          display.print(vcomVoltage);
          display.partialUpdate();

          if (display.setVCOM(vcomVoltage))
          {
            Serial.println("\nVCOM EEPROM PROGRAMMING OK\n");

            // Mark the device as tested and provisioned so the tests
            // don't run again on the next boot
            EEPROM.write(EEPROMaddress, 170);
            EEPROM.commit();
            break;
          }
          else
          {
            Serial.println("ERROR");
          }
        }
        
        display.selectDisplayMode(INKPLATE_3BIT);
    }
    else
    {
        Serial.println("VCOM already set!");
        // *****************************************************
        // Turn on power supply for epaper display. 
        // WARNING: Do not call this method repeatedly as it 
        //          can damage your display if used incorrectly!
        // *****************************************************
        display.einkOn();
    }

    memset(commandBuffer, 0, BUFFER_SIZE);

    showSplashScreen(vcomVoltage);
}

void loop()
{
    // Peripheral mode

    if (Serial.available())
    {
        while (Serial.available())
        {
            for (int i = 0; i < (BUFFER_SIZE - 1); i++)
            {
                commandBuffer[i] = commandBuffer[i + 1];
            }
            commandBuffer[BUFFER_SIZE - 1] = Serial.read();
        }
    }

    // Function in Peripheral.h
    run(commandBuffer, BUFFER_SIZE, &display);
}

// Print the initial image that remains on the screen
void showSplashScreen(float vComVoltage)
{
    display.display();
    display.selectDisplayMode(INKPLATE_3BIT);
    display.image.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    display.setTextColor(0, 7);
    display.setTextSize(1);
    display.setCursor(5, 734);
    display.print(vComVoltage, 2);
    display.print("V");
    display.display();
}

// Prompt user to enter VCOM
double getVCOMFromSerial(double *_vcom)
{
    double vcom = 1;
    char serialBuffer[50];
    unsigned long serialTimeout;

    // Display a message on Inkplate
    display.print("\r\n- Write VCOM on UART: ");
    display.partialUpdate(0, 1);

    while (true)
    {
        Serial.println(
            "Write VCOM voltage from epaper panel.\r\nDon't forget negative (-) sign!\r\nUse dot as the decimal point. "
            "For example -1.23\n");
        while (!Serial.available())
            ;

        serialTimeout = millis();
        int i = 0;
        while ((Serial.available()) && ((unsigned long)(millis() - serialTimeout) < 500))
        {
            if ((Serial.available()) && (i < 49))
            {
                serialBuffer[i++] = Serial.read();
                serialTimeout = millis();
            }
        }
        serialBuffer[i] = 0;
        if (sscanf(serialBuffer, "%lf", &vcom) == 1)
        {
            *_vcom = vcom;
            return 1;
        }
    }

    return 0;
}
