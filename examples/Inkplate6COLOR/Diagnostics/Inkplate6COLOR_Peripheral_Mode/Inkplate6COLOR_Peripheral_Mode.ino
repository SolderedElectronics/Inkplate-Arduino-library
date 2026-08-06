/**
 **************************************************
 * @file        Inkplate6COLOR_Peripheral_Mode.ino
 * @brief       Enables UART-based Peripheral Mode control for Inkplate 6COLOR.
 *
 * @details     This example enables Peripheral Mode on Inkplate 6COLOR,
 *              allowing an external host to control the e-paper display over
 *              a UART command interface instead of running custom drawing code
 *              directly on the board.
 *
 *              In this mode, display operations are received as serial
 *              commands and executed by the Peripheral Mode parser. This makes
 *              Inkplate 6COLOR usable as a display peripheral for PCs,
 *              Raspberry Pi boards, microcontrollers, and other embedded
 *              systems that can communicate over UART.
 *
 *              Because UART bandwidth is limited, this approach is not ideal
 *              for large numbers of low-level pixel commands such as repeated
 *              drawPixel() operations. For image-heavy workflows, it is more
 *              efficient to store bitmap/image files on an SD card and trigger
 *              higher-level drawing operations from there. UART-driven
 *              rendering is slower than native local drawing code.
 *
 *              This firmware is typically preloaded from the factory, acts as a
 *              serial command interpreter, continuously processes incoming data
 *              in loop(), and can be extended if additional command
 *              functionality is needed.
 *
 *              Expected output: "READY" on Serial after successful Peripheral
 *              Mode initialization, and the display updating according to
 *              received UART commands.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB-C cable
 * - Extra:      External controller device (optional)
 * - Serial:     115200 baud, standard parity, line ending "\n\r"
 *               (newline + carriage return)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6COLOR.
 * 3) Connect to the board over USB serial or directly to the ESP32 UART pins.
 * 4) Configure the serial terminal or host device for 115200 baud with
 *    newline + carriage return line endings.
 * 5) Wait for the "READY" message.
 * 6) Send supported Peripheral Mode commands to control the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2025-10-23
 * @license     GNU GPL V3
 **************************************************/

// Include Inkplate library
#include "Inkplate.h"

// Include Peripheral Mode library
#include "InkplatePeripheralMode.h"

// Include the header file with sketch settings (buffer size, serial timeout, argument termination char, etc.)
#include "settings.h"

// Pointer to the singleton Peripheral Mode instance
PeripheralMode *peripheral;

// Create Inkplate 6COLOR display object
Inkplate display;

void setup()
{
    // Initialize Inkplate library
    display.begin();

    // Create instance of Peripheral Mode object
    peripheral = PeripheralMode::getInstance();

    // Initialize Peripheral Mode (UART, display, buffer, etc.)
    if (!peripheral->begin(&Serial, &display, 115200ULL, SERIAL_UART_RX_PIN, SERIAL_UART_TX_PIN, SERIAL_BUFFER_SIZE))
    {
        // Send an error message to serial
        Serial.println("Peripheral Mode init failed!\nProgram halted!");

        // Stop program execution
        while (1);
    }

    Serial.println("READY");
}

void loop()
{
    // Check if there is incoming data on serial and process commands
    peripheral->getDataFromSerial(SERIAL_TIMEOUT_MS);
}
