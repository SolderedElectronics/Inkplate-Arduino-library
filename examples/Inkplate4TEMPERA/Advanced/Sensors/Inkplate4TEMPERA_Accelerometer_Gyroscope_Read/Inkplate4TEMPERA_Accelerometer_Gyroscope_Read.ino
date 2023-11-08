/*
   Inkplate4TEMPERA_Accelerometer_Gyroscope_Read example for Soldered Inkplate 4 TEMPERA
   For this example you will need only a USB-C cable and Inkplate 4 TEMPERA.
   Select "Soldered Inkplate 4 TEMPERA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 4 TEMPERA" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to read from the built-in LSM6DS3 gyroscope/accelerometer.
   The rotation of the 3D Cube is modified by the accelerometer values.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   10 Aug 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)
// This helps with faster refreshes

// Variable that keeps count on how many times the screen has been partially updated
int numRefreshes = 0;

// How many partial updates we want before doing a full refresh
// 35 still looks OK, it's not reccomended to go over this
#define NUM_PARTIAL_UPDATES_BEFORE_FULL_REFRESH 35

// Variables which are used for drawing the 3D Cube
// Cube vertices
float cube[8][3] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}};
// Cube edges
int edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
    {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
    {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
};
// This value multiplies the accelerometer readings to help project the cube in the orientation of the accelerometer
// If you want accelerometer movements to have more effect on the cube's retation, increase this
// And vice versa
#define ANGLE_MODIFIER 0.0008
// Variables for the angles at which the cube gets projected
float angleX = 0;
float angleY = 0;
float angleZ = 0;
// Also, remember the previous angles
// This is just to calculate the average between the two in order to smooth out the movement
float previousAngleX = 0;
float previousAngleY = 0;
float previousAngleZ = 0;

// Setup code, runs only once
void setup()
{
    display.begin();   // Init Inkplate library (you should call this function ONLY ONCE)
    display.display(); // Put clear image on display

    // Enable the accelerometer & gyroscope.
    display.wakePeripheral(INKPLATE_ACCELEROMETER);

    // Set text size to be 2x larger than default (5x7px)
    display.setTextSize(2);
    display.setTextColor(BLACK); // Set the text color to black also

    // Try to init the acccelerometer
    if (display.lsm6ds3.begin() != 0)
    {
        // In case of initialization failure, notify the user
        display.setCursor(50, 50);
        display.setFont();
        display.print("ERROR: can't init LSM6DS3!");
        display.display();

        // Go to sleep
        esp_deep_sleep_start();
    }
}

void loop()
{
    // First, clear what was previously in the frame buffer
    display.clearDisplay();
    
    // Read values from the accelerometer
    float accelX = display.lsm6ds3.readRawAccelX();
    float accelY = display.lsm6ds3.readRawAccelY();
    float accelZ = display.lsm6ds3.readRawAccelZ();

    // Read values from the gyroscope
    float gyroX = display.lsm6ds3.readFloatGyroX();
    float gyroY = display.lsm6ds3.readFloatGyroY();
    float gyroZ = display.lsm6ds3.readFloatGyroZ();

    // Print accelerometer readings on the display
    display.setCursor(40, 430);
    display.print("ACC X:");
    display.print(accelX, 4);
    display.setCursor(40, 450);
    display.print("ACC Y:");
    display.print(accelY, 4);
    display.setCursor(40, 470);
    display.print("ACC Z:");
    display.print(accelZ, 4);

    // Print gyroscope readings on the display also
    display.setCursor(40, 490);
    display.print("GYRO X:");
    display.print(gyroX, 4);
    display.setCursor(40, 510);
    display.print("GYRO Y:");
    display.print(gyroY, 4);
    display.setCursor(40, 530);
    display.print("GYRO Z:");
    display.print(gyroZ, 4);

    // Let's draw the cube!
    // Compute the angle modifier variables from the accelerometer data
    angleX = accelX * ANGLE_MODIFIER;
    angleY = accelY * ANGLE_MODIFIER;
    angleZ = accelZ * ANGLE_MODIFIER;

    // Calculate the average between the previous
    // This makes the movement smoother
    angleX = (angleX + previousAngleX) / 2;
    angleY = (angleY + previousAngleY) / 2;
    angleZ = (angleZ + previousAngleZ) / 2;

    // Remember the value for the next loop
    previousAngleX = angleX;
    previousAngleY = angleY;
    previousAngleZ = angleZ;

    // Let's project the cube's edges!
    // For each edge...
    for (int i = 0; i < 12; i++)
    {
        // Get the start and end vertices
        float *v1 = cube[edges[i][0]];
        float *v2 = cube[edges[i][1]];

        // Rotate and project the vertices to 2D
        int x1, y1, x2, y2;

        // Project it, notice that X, Y and Z are rearranged here and not in the default order
        // This is due to the orientation of the gyroscope on the actual board
        project(v1, angleY, angleZ, angleX, &x1, &y1);
        project(v2, angleY, angleZ, angleX, &x2, &y2);

        // Draw the edge
        display.drawLine(x1, y1, x2, y2, BLACK);
    }

    // Finally, let's update the screen
    // Check if we need to do full refresh or you can do partial update
    if (numRefreshes > NUM_PARTIAL_UPDATES_BEFORE_FULL_REFRESH)
    {
        // Time for a full refresh? Do it
        display.display();
        numRefreshes = 0; // Reset the counter
    }
    else
    {
        // Time for a partial update
        display.partialUpdate(false, true); // Do partial update while keeping the screen on
        numRefreshes++;                     // Increment the partial update counter as well
    }

    // Wait 30ms so the frame rate isn't too fast
    delay(30);
}

// This function projects 3D space onto 2D with a set rotation
void project(float *v, float angleX, float angleY, float angleZ, int *x, int *y)
{
    // Rotate the vertex around the X axis
    float xr = v[0];
    float yr = v[1] * cos(angleX) - v[2] * sin(angleX);
    float zr = v[1] * sin(angleX) + v[2] * cos(angleX);

    // Rotate the vertex around the Y axis
    float xrr = xr * cos(angleY) + zr * sin(angleY);
    float yrr = yr;
    float zrr = -xr * sin(angleY) + zr * cos(angleY);

    // Rotate the vertex around the Z axis
    float xrrr = xrr * cos(angleZ) - yrr * sin(angleZ);
    float yrrr = xrr * sin(angleZ) + yrr * cos(angleZ);
    float zrrr = zrr;

    // Project the vertex to 2D
    float z = 4 / (4 + zrrr);
    *x = xrrr * z * 100 + display.width() / 2;
    *y = yrrr * z * 100 + display.height() / 2;
}
