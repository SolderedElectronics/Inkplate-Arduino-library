/**
 **************************************************
 * @file        Inkplate4TEMPERA_Buzzer.ino
 * @brief       Demonstrates how to drive the built-in buzzer on Inkplate 4
 *              TEMPERA, including simple beeps and approximate pitch control.
 *
 * @details     This example shows basic usage of the Inkplate 4 TEMPERA buzzer
 *              API. After initializing the board and calling the buzzer init
 *              routine, the sketch plays several short demo sequences:
 *
 *              - Fixed-duration beeps using beep(duration_ms)
 *              - Manual on/off control using beepOn() and beepOff()
 *              - Frequency-controlled beeps using beep(duration_ms, freq_hz)
 *
 *              In the main loop, it plays a short repeating pattern based on a
 *              C Maj7 chord (C, E, G, B). The pattern alternates between single
 *              notes and doubled notes to create a simple rhythm.
 *
 *              Pitch control is approximate: the buzzer frequency is set via a
 *              digital potentiometer, which does not behave linearly. Only a
 *              limited frequency range is supported reliably (approximately
 *              572-2933 Hz); frequencies outside this range may be unstable or
 *              inaudible. Delays are used for rhythm and spacing between notes,
 *              so this example is intentionally blocking and focused on
 *              demonstrating the buzzer API. Display mode is 1-bit BW
 *              (INKPLATE_1BIT); the display is not actively used here, it is
 *              only initialized as part of the standard board setup.
 *
 *              Expected output: audible beeps from the on-board buzzer - three
 *              short beeps at startup, two longer beeps using manual on/off
 *              control, two low-frequency beeps followed by two high-frequency
 *              beeps, then a repeating short "song" built from the chord notes.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     none
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 4 TEMPERA.
 * 3) Listen to the startup demo sequences (short beeps, on/off beeps, then
 *    low/high pitched beeps).
 * 4) After the demos, the loop continuously plays a simple melody pattern.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-09-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Frequencies for the C Maj7 chord (in Hz):
//               C    E    G    B
int chord[4] = {523, 659, 783, 987};
// This scale best fits the range of the buzzer where the frequency can be accurately conrolled via the digipot

// Some helpful variables for playing the chord
int currentNoteIndex = 0;
int repeatCounter = 0;

// Setup code, runs only once
void setup()
{
    display.begin();   // Init Inkplate library (you should call this function ONLY ONCE)
    display.display(); // Put clear image on display

    // Init the buzzer, this must be called if using it
    display.buzzer.init();

    // The most basic example
    // This will produce three short beeps
    // The beeps are each 80ms long
    display.buzzer.beep(80);
    delay(80);
    display.buzzer.beep(80);
    delay(80);
    display.buzzer.beep(80);
    delay(80);

    delay(5000); // Wait 5 seconds before the next example

    // The buzzer may also be controlled by manually turning it on or off
    // beepOn will turn on the buzzer indefinitely until beepOff is called
    // This will produce two 200 ms beeps
    display.buzzer.beepOn();
    delay(200);
    display.buzzer.beepOff();
    delay(200);
    display.buzzer.beepOn();
    delay(200);
    display.buzzer.beepOff();
    delay(200);

    delay(5000); // Wait 5 seconds before the next example

    // Pitch may also be controlled
    // Note that pitch is approximated as the digital potentiometer does not affect the pitch in a linear way
    // Frequencies from 572 to 2933 Hz are supported
    // Here are two low pitched (~750Hz) followed by two high pitched (~2400Hz) beeps
    display.buzzer.beep(300, 750);
    delay(50);
    display.buzzer.beep(300, 750);
    delay(50);
    display.buzzer.beep(300, 2400);
    delay(50);
    display.buzzer.beep(300, 2400);
    delay(50);

    delay(5000); // Wait 5 seconds before the next example
}

// In the loop, play a little song with the notes of the chord
void loop()
{
    // The first two times play the notes normally
    if (repeatCounter < 2)
    {
        // Play the note set to be played for 100 ms
        display.buzzer.beep(100, chord[currentNoteIndex]);
        delay(600); // Wait 600 ms so there's room between the notes
    }
    // The second four times play the notes twice
    else
    {
        // Play the note set to be played for 100 ms
        display.buzzer.beep(100, chord[currentNoteIndex]);
        delay(250); // Wait 300 ms and play it again for 50 ms
        display.buzzer.beep(50, chord[currentNoteIndex]);
        delay(300); // Wait for 300 ms, this totals to 700 so it's in rhythm
    }

    // Increment the counter and reset if it went out of bounds
    currentNoteIndex++;
    if (currentNoteIndex >= 4)
    {
        currentNoteIndex = 0;

        // If the loop overall was repeated 4 times, the song is done, a bit longer
        repeatCounter++;
        if (repeatCounter >= 4)
        {
            repeatCounter = 0;
            delay(3000);
        }
    }
}