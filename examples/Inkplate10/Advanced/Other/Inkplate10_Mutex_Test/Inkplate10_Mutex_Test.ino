/**
 **************************************************
 * @file        Inkplate10_Mutex_Test.ino
 * @brief       Two-task display mutex test for Inkplate 10.
 *
 * @details     Spawns two FreeRTOS tasks that independently draw and
 *              partial-update opposite sides of the screen. Each task
 *              acquires the display mutex before touching the framebuffer
 *              or calling partialUpdate(), preventing interleaved writes
 *              that would corrupt the output.
 *
 *              Left task:  bouncing square on the left half  (x 0..599)
 *              Right task: bouncing square on the right half (x 600..1199)
 *
 *              To observe what happens without locking, comment out the
 *              display.displayLock() / displayUnlock() calls.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 10.
 * 3) Both squares bounce on their own half of the screen without corrupting
 *    each other's output.
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
 ***************************************************/

#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection — select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);

static const int SQ = 140;
static const int DY = 6;

void taskLeft(void *pvParameters)
{
    int y = 60, dy = DY;
    while (true)
    {
        display.displayLock();
        display.fillRect(0, 0, 600, display.height(), WHITE);
        display.fillRect(230, y, SQ, SQ, BLACK);
        display.partialUpdate(false, true);
        display.displayUnlock();

        y += dy;
        if (y < 0 || y + SQ > display.height())
            dy = -dy;
        vTaskDelay(pdMS_TO_TICKS(150));
    }
}

void taskRight(void *pvParameters)
{
    int y = display.height() / 2, dy = -DY;
    while (true)
    {
        display.displayLock();
        display.fillRect(600, 0, 600, display.height(), WHITE);
        display.fillRect(830, y, SQ, SQ, BLACK);
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
    display.drawFastVLine(600, 0, display.height(), BLACK);
    display.display();

    xTaskCreatePinnedToCore(taskLeft,  "TaskLeft",  4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskRight, "TaskRight", 4096, NULL, 1, NULL, 1);
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
