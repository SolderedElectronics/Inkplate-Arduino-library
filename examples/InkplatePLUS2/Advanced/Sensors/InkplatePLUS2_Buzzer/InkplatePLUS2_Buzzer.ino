/*
   InkplatePLUS2_Buzzer example for Soldered Inkplate PLUS2
   For this example you will need only a USB-C cable and Inkplate PLUS2.
   Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to beep the built-in buzzer and set it's frequency.
   Upload the sketch and listen to the beeps!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   11 Sep 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
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
    display.buzzer.begin();

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