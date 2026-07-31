/**
 **************************************************
 * @file        Inkplate5V2_Mutex_Test.ino
 * @brief       Two-task display mutex test for Inkplate 5v2.
 *
 * @details     Spawns two FreeRTOS tasks that independently draw and
 *              partial-update opposite sides of the screen. Each task
 *              acquires the display mutex before touching the framebuffer
 *              or calling partialUpdate(), preventing interleaved writes
 *              that would corrupt the output.
 *
 *              Left task:  bouncing square on the left half  (x 0..639)
 *              Right task: bouncing square on the right half (x 640..1279)
 *
 *              To observe what happens without locking, comment out the
 *              display.displayLock() / displayUnlock() calls.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5v2"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 5v2.
 * 3) Both squares bounce on their own half of the screen without corrupting
 *    each other's output.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/5v2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @license     GNU GPL V3
 ***************************************************/

#if !defined(ARDUINO_INKPLATE5V2)
#error "Wrong board selection — select Soldered Inkplate5V2 in the boards menu."
#endif

#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);

// Square size and bounce step (pixels)
static const int SQ  = 120;
static const int DY  = 6;

// ----------------------------------------------------------------
// Left task: square bounces vertically on the left half of screen
// ----------------------------------------------------------------
void taskLeft(void *pvParameters)
{
    int y  = 50;
    int dy = DY;

    while (true)
    {
        display.displayLock();

        // Erase the column used by this task, then draw the square
        display.fillRect(0, 0, 640, display.height(), WHITE);
        display.fillRect(270, y, SQ, SQ, BLACK);
        display.partialUpdate(false, true);

        display.displayUnlock();

        y += dy;
        if (y < 0 || y + SQ > display.height())
            dy = -dy;

        vTaskDelay(pdMS_TO_TICKS(150));
    }
}

// ----------------------------------------------------------------
// Right task: square bounces vertically on the right half of screen
// ----------------------------------------------------------------
void taskRight(void *pvParameters)
{
    int y  = display.height() / 2;
    int dy = -DY;

    while (true)
    {
        display.displayLock();

        // Erase the column used by this task, then draw the square
        display.fillRect(640, 0, 640, display.height(), WHITE);
        display.fillRect(910, y, SQ, SQ, BLACK);
        display.partialUpdate(false, true);

        display.displayUnlock();

        y += dy;
        if (y < 0 || y + SQ > display.height())
            dy = -dy;

        vTaskDelay(pdMS_TO_TICKS(150));
    }
}

void setup()
{
    Serial.begin(115200);

    display.begin();
    display.clearDisplay();

    // Draw a centre divider so the two task regions are visible
    display.drawFastVLine(640, 0, display.height(), BLACK);
    display.display();

    // Pin tasks to different cores to maximise contention
    xTaskCreatePinnedToCore(taskLeft,  "TaskLeft",  4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskRight, "TaskRight", 4096, NULL, 1, NULL, 1);
}

void loop()
{
    // Both tasks run independently; nothing to do in loop
    //vTaskDelay(pdMS_TO_TICKS(1000));
}
