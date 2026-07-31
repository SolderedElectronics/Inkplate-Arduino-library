/**
 **************************************************
 * @file        Inkplate2_Peripheral_Mode.ino
 * @brief       Enables UART-based Peripheral Mode control for Inkplate 2.
 *
 * @details     This example enables Peripheral Mode on the Inkplate 2,
 *              allowing external devices to control the e-paper display
 *              through a UART command interface instead of Arduino code.
 *
 *              In this mode, drawing commands, display updates, and other
 *              operations are sent as text commands over the serial
 *              connection. This allows Inkplate 2 to function as a
 *              display peripheral for systems such as PCs, Raspberry Pi,
 *              microcontrollers, or embedded Linux devices.
 *
 *              The sketch initializes the Inkplate display and a
 *              PeripheralMode command parser which listens for incoming
 *              UART commands and executes them in real time. This firmware is
 *              typically preloaded on Inkplate 2 devices from the factory and
 *              provides a flexible way to control the display without compiling
 *              Arduino sketches. It acts as a command interpreter and runs
 *              continuously, processing serial commands inside the main loop.
 *
 *              Because UART communication is relatively slow, sending large
 *              numbers of pixel-level commands (such as drawPixel loops) is
 *              inefficient. For displaying images, it is recommended to
 *              store bitmap files on the SD card and load them directly.
 *
 *              Expected output: a "READY" message on Serial when Peripheral Mode
 *              is initialized, and the display updating according to received
 *              UART drawing commands.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB-C cable
 * - Extra:      External controller device (optional)
 * - Serial:     115200 baud, standard parity, line ending "\n\r"
 *               (newline + carriage return)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Upload the sketch to the Inkplate 2.
 * 3) Connect to the board using a serial terminal or external device.
 * 4) Configure UART to 115200 baud with line ending "\n\r".
 * 5) Send supported Peripheral Mode commands to control the display.
 * 6) The device responds with "READY" when initialization completes.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
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

// Create Inkplate 2 display object
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
