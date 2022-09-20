/*
    Asana API example for Soldered Inkplate 2
    For this example you will need only USB cable, Inkplate 2 and a WiFi with stable Internet connection.
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example show how to use Asana API to show info about tasks.
    You will need to obtain PAT by going to: https://app.asana.com/ and
    opening My Settings... there. Next step is opening App submenu and clicking
    Manage Developer Apps. Then you need to Create new token and when you create
    token it will be shown first and last time and it needs to be copied and saved somewhere.
    when you get your token (PAT) you can copy it in user_PAT array. After obtaining PAT
    you need to obtain user task list GID by going to: https://app.asana.com/ and clicking
    My Tasks. From there in URL bar you need to copy long number before /list and paste it
    in user_task_list_gid array. URL will look like https://app.asana.com/0/1234567891234567/list
    and part which needs to be copied is 1234567891234567 (16 numbers).

    IMPORTANT:
    Make sure to change wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    7 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

char user_task_list_gid[] = ""; // Your task list GID, you can find it in URL
// in your browser when you open your task list
// It is long number starting with 120...

char user_PAT[] = ""; // This is PAT (Personal Access Token) which
// which is used to authentificate. You can learn more how to obtain
// one and obtain one at this link: https://developers.asana.com/docs/authentication-quick-start
// When you obtain PAT just copy it here.

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/Inter16pt7b.h"
#include "Fonts/Inter8pt7b.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"


// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Delay between API calls in miliseconds
#define DELAY_MS (uint32_t)3 * 60 * 1000

char today[12]; // Array to store current date

struct task *tasks = NULL;
struct task *curr_task = NULL;

// Our functions declared below setup and loop
void drawAll();

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(true);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Welcome screen
    display.setCursor(20, 90); // Set cursor, custom font uses different method for setting cursor

    // You can find more about that here https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
    display.setTextSize(1);
    display.println(F("Asana API Example!"));
    display.display();
    display.clearDisplay();
    delay(1000);
    Serial.println("Connecting to WiFi");

    // Our begin function
    network.begin();
    Serial.println("Getting tasks...");
    do
    {
        tasks = network.getData();
        delay(1000);
    } while (tasks == NULL);
    Serial.println("Tasks fetched.");
    curr_task = tasks;
    network.getTime(today);

    // Our main drawing function
    Serial.println("Drawing...");
    display.clearDisplay();
    drawAll();

    // Refresh
    display.display();

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000 * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

// Our main drawing function
void drawAll()
{
    curr_task = tasks;
    display.setCursor(10, 5); // Set cursor to 5, 10

    while (display.getCursorY() < 80 && curr_task != 0) // Don't print events if cursor is below 80 pixels
    {
        uint8_t cnt = 0;
        network.getDue(curr_task); // Get time for current task
        if (strstr(curr_task->due, today))
        {
            uint8_t tempY = display.getCursorY(); // Get y coordinate of cursor
            while (curr_task->name[cnt] != '\0')  // Print until 0 sign
            {
                display.print(curr_task->name[cnt]); // Print one letter
                cnt++;
                if (display.getCursorX() > 200) // Check if cursor is near border of display
                {
                    display.setCursor(10,
                                      display.getCursorY() + 10); // If cursor is near border, set cursor to new line
                }
            }
            display.setCursor(10, display.getCursorY() + 10); // Set cursor to new line to print due
            if (curr_task->due[0] != 'N')                     // If due is "No due!" don't execute this code
            {
                curr_task->due[4] = curr_task->due[7] = ' '; // Replace '-' with space so sscanf can catch numbers

                int mday, mon, yr; // Variables to store due date
                sscanf(curr_task->due, "%d %d %d", &yr, &mon,
                       &mday); // Due is in format "2022 06 09", so sscanf command can fetch it and store in variables
                time_t nowSecs = time(nullptr);  // Get epoch through NTP server
                struct tm *t = gmtime(&nowSecs); // Convert epoch to time struct

                if ((t->tm_mday > mday) && (t->tm_mon >= mon) && (t->tm_year >= yr)) // Test if due has past
                {
                    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
                    display.print("Due: ");
                    display.print(curr_task->due);
                    display.print(" (Overdue)");
                    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
                }
                else
                {
                    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
                    display.print("Due: ");
                    display.print(curr_task->due);
                }
            }
            else
            {
                display.print("Due: No due!");
            }
            display.drawRoundRect(5, tempY - 2, 202, display.getCursorY() - tempY + 12, 4,
                                  INKPLATE2_BLACK);           // Draw rectangle around task details
            display.setCursor(10, display.getCursorY() + 15); // Set cursor for printing next task
        }
        curr_task = curr_task->next; // Select next task
    }
}
