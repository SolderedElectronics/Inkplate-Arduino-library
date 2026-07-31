/**
 **************************************************
 * @file        Inkplate10_Peripheral_Mode.ino
 * @brief       Runs Inkplate 10 in "Peripheral Mode", accepting UART commands
 *              to control the e-paper display without custom Arduino drawing
 *              code.
 *
 * @details     This example turns Inkplate 10 into a serial-controlled display
 *              peripheral. Instead of implementing graphics logic in the sketch
 *              itself, an external host (PC, SBC, or another MCU) sends UART
 *              commands to the ESP32, and the PeripheralMode library parses and
 *              executes them on the Inkplate display.
 *
 *              The sketch initializes the Inkplate driver in 3-bit grayscale
 *              mode (INKPLATE_3BIT), then starts the PeripheralMode singleton
 *              with a configurable UART RX/TX pin mapping, serial buffer size,
 *              and command timeout (defined in settings.h). In the main loop it
 *              continuously reads and processes incoming serial data. Adjust
 *              SERIAL_UART_RX_PIN, SERIAL_UART_TX_PIN, SERIAL_BUFFER_SIZE and
 *              SERIAL_TIMEOUT_MS in settings.h if needed.
 *
 *              Because UART bandwidth is limited, sending many pixel-level
 *              commands (e.g., drawPixel in large loops) is inefficient. For
 *              image-heavy content, store bitmaps/images on the microSD card
 *              and use commands that load and render files instead. Partial
 *              updates are not supported in grayscale mode, so refresh
 *              operations performed by commands are full refreshes. This sketch
 *              is commonly used as a "factory" firmware to allow Inkplate to be
 *              driven from non-Arduino platforms via a simple serial protocol.
 *
 *              Expected output: the Serial Monitor prints "READY" when
 *              initialization succeeds and the display updates in response to
 *              valid UART commands sent by the host. If initialization fails, an
 *              error message is printed and the sketch halts.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB-C cable
 * - Extra:      none (optional: microSD card for image file rendering)
 * - Serial:     115200 baud, standard parity, line ending "\\n\\r"
 *               (both NL & CR) as expected by the command parser
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) Open a serial terminal at 115200 baud and wait for the device to print
 *    "READY".
 * 4) Send Peripheral Mode commands terminated with "\\n\\r".
 * 5) For faster image updates, place image/bitmap files on a microSD card and
 *    use commands that load and draw from SD instead of per-pixel drawing.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2024-04-15
 * @license     GNU GPL V3
 **************************************************/
// Include Inkplate library.
#include "Inkplate.h"

// Include peripheral mode library.
#include "InkplatePeripheralMode.h"

// Include the header files with sketch settngs (buffer size, serial timeput, argument termination char etc).
#include "settings.h"

// Pointer to the singleton class instance.
PeripheralMode *peripheral;

// Inkplate object.
Inkplate display(INKPLATE_3BIT);

void setup()
{
    // Init Inkplate library.
    display.begin();

    // Create one instance of the object.
    peripheral = PeripheralMode::getInstance();

    // Set the baud rate (in this case 115200 bauds), set the serial buffer and it's size.
    if (!peripheral->begin(&Serial, &display, 115200ULL, SERIAL_UART_RX_PIN, SERIAL_UART_TX_PIN, SERIAL_BUFFER_SIZE))
    {
        // Send an error message on serial.
        Serial.println("Peripheral mode init failed!\nProgram halted!");

        // Stop the program from executing!
        while(1);
    }
    Serial.println("READY");
}

void loop()
{
    // Check if there is something in the serial.
    peripheral->getDataFromSerial(SERIAL_TIMEOUT_MS);
}