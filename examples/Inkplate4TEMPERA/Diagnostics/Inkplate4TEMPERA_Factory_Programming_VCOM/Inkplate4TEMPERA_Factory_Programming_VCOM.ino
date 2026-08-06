/**
 **************************************************
 * @file        Inkplate4TEMPERA_Factory_Programming_VCOM.ino
 * @brief       Factory utility for Inkplate 4TEMPERA: program panel VCOM, run
 *              hardware self-tests, select and store a display waveform, then
 *              play the onboarding slide sequence.
 *
 * @details     This sketch is intended for factory/production use on Inkplate
 *              4TEMPERA. It performs a first-boot provisioning flow and a
 *              subsequent onboarding/demo flow.
 *
 *              First boot flow:
 *              - Performs I2C sanity checks and peripheral tests (see test.h /
 *                test.cpp).
 *              - Prompts the operator over Serial (115200 baud) to enter the
 *                panel VCOM voltage (typically negative, range 0.0 to -5.0 V),
 *                then programs it into EEPROM using setVcom().
 *              - Prompts the operator to select one of several pre-defined
 *                waveform tables and previews a grayscale gradient to help
 *                choose the best match for the panel.
 *              - Displays a splash screen showing the programmed VCOM and the
 *                selected waveform ID.
 *
 *              Normal boot flow:
 *              - Reads the stored VCOM, enables the panel (einkOn()), allocates
 *                PSRAM for slide decompression, and runs the onboarding sequence
 *                consisting of a sidebar UI and multiple slides. Some slides are
 *                stored in RLE-compressed form and are decompressed at runtime
 *                into a large buffer to save flash space.
 *
 *              Display modes: 1-bit (BW) mode is used for partial-update
 *              demonstrations and fast UI elements, and 3-bit grayscale for most
 *              onboarding slides. Partial updates are used only in BW mode.
 *
 *              This sketch allocates a large PSRAM buffer for decompression
 *              (ps_malloc(393938)); it requires sufficient PSRAM and will halt
 *              with an on-screen error if allocation fails. RLE decompression
 *              expands compressed slide data into a raw 3-bit bitmap buffer, so
 *              avoid modifying buffer sizes unless you also update the assets.
 *              Factory test requirements depend on test.cpp: missing WiFi
 *              credentials, I2C slave or microSD may cause tests to fail and stop
 *              the process.
 *
 *              Expected output: test status messages, prompts for VCOM and
 *              waveform selection and programming success/failure messages on the
 *              Serial Monitor; a splash screen showing VCOM and waveform ID on
 *              the e-paper, followed by a multi-slide onboarding sequence with a
 *              sidebar and BW/grayscale slides (including a partial-update
 *              animation demo).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4TEMPERA
 * - Hardware:   Inkplate 4TEMPERA, USB cable
 * - Extra:      microSD card (formatted, any content), EasyC I2C slave device
 *               for factory tests, WiFi credentials in test.cpp if the tests
 *               require network
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate4TEMPERA"
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
 *      required) and confirm until programming succeeds.
 *    - Prompt to select a waveform (send '1'..'5' and then 'OK' to confirm).
 * 5) After successful setup, the device shows a splash screen and then remains
 *    idle. Power-cycle or reset to start the onboarding slideshow.
 * 6) During onboarding, press the wake button to advance through slides.
 *
 * @warning     VCOM programming is limited: the panel VCOM can be programmed a
 *              finite number of times (typically ~100 writes). Avoid repeated
 *              programming and use it only when necessary.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

// Include needed libraries in the sketch
#include "Inkplate.h"
#include "Wire.h"

// Include our functions and image
#include "Peripheral.h"
#include "demo_image.h"
#include "test.h"

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
    double vcomVoltage = -1.8;

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

// Functions that writes data in register over I2C communication
void writeReg(uint8_t _reg, uint8_t _data)
{
    Wire.beginTransmission(0x48);
    Wire.write(_reg);
    Wire.write(_data);
    Wire.endTransmission();
}

// Functions that reads data from register over I2C communication
uint8_t readReg(uint8_t _reg)
{
    Wire.beginTransmission(0x48);
    Wire.write(_reg);
    Wire.endTransmission(false);
    Wire.requestFrom(0x48, 1);
    return Wire.read();
}

// Print the initial image that remains on the screen
void showSplashScreen(float vComVoltage)
{
    display.display();
    display.selectDisplayMode(INKPLATE_3BIT);
    display.image.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);

    // Also, print VCOM in small font
    display.setTextColor(0, 7);
    display.setTextSize(1);
    display.setCursor(31, 570);
    display.print(vComVoltage, 2);
    display.print("V");
    display.display();
}

// Write the set VCOM to EEPROM memory for pernament usage
uint8_t writeVCOMToEEPROM(double v)
{
    int vcom = int(abs(v) * 100);
    int vcomH = (vcom >> 8) & 1;
    int vcomL = vcom & 0xFF;

    // Set PCAL pin where TPS65186 INT pin is connectet to input pull up
    display.expander1.pinMode(6, INPUT_PULLUP);

    // First power up TPS65186 so we can communicate with it
    display.einkOn();

    // Wait a little bit
    delay(250);

    // Send to TPS65186 first 8 bits of VCOM
    writeReg(0x03, vcomL);

    // Send new value of register to TPS
    writeReg(0x04, vcomH);
    delay(1);

    // Program VCOM value to EEPROM
    writeReg(0x04, vcomH | (1 << 6));

    // Wait until EEPROM has been programmed
    delay(100);
    do
    {
        delay(1);
    } while (display.expander1.digitalRead(6));

    // Clear Interrupt flag by reading INT1 register
    readReg(0x07);

    // Now, power off whole TPS
    display.einkOff();

    // Wait a little bit...
    delay(1000);

    // Power up TPS again
    display.einkOn();

    delay(10);

    // Read VCOM valuse from registers
    vcomL = readReg(0x03);
    vcomH = readReg(0x04);

    // Trun off the TPS65186 and wait a little bit
    display.einkOff();
    delay(100);

    if (vcom != (vcomL | (vcomH << 8)))
    {
        Serial.println("\nVCOM EEPROM PROGRAMMING FAILED!\n");
        return 0;
    }
    else
    {
        Serial.println("\nVCOM EEPROM PROGRAMMING OK\n");
        return 1;
    }
    return 0;
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
