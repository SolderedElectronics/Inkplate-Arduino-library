/**
 **************************************************
 * @file        Inkplate4TEMPERA_Accelerometer_Gyroscope_Read.ino
 * @brief       Reads the on-board LSM6DS3 accelerometer/gyroscope and visualizes
 *              motion by rotating a wireframe 3D cube on the e-paper display.
 *
 * @details     This example demonstrates how to use the built-in LSM6DS3 IMU
 *              (inertial measurement unit) on Inkplate 4 TEMPERA. After waking
 *              the accelerometer peripheral (via
 *              wakePeripheral(INKPLATE_ACCELEROMETER)) and initializing the
 *              sensor, the sketch continuously reads raw accelerometer axes
 *              (X/Y/Z) and gyroscope axes (X/Y/Z). The numeric readings are
 *              printed on the display for reference.
 *
 *              In parallel, the sketch renders a rotating wireframe cube. The
 *              cube's rotation angles are derived from the accelerometer values
 *              (scaled by ANGLE_MODIFIER) and smoothed by averaging with the
 *              previous frame's angles. Each cube edge is projected from 3D to
 *              2D using basic rotation matrices and a simple perspective
 *              projection, then drawn as lines.
 *
 *              The display runs in 1-bit black/white mode (INKPLATE_1BIT) to
 *              enable fast partial updates. The cube animation uses frequent
 *              partial updates for a higher apparent frame rate and forces a
 *              full refresh after NUM_PARTIAL_UPDATES_BEFORE_FULL_REFRESH
 *              updates to reduce ghosting. Panel power is kept enabled during
 *              partial updates for stability and speed (higher power usage).
 *
 *              This is a visualization example, not a calibrated orientation
 *              filter. For stable orientation/attitude estimation, use a sensor
 *              fusion algorithm (e.g. complementary filter, Madgwick, Mahony)
 *              and calibrated units.
 *
 *              Expected output: a wireframe cube rendered near the centre of the
 *              display, rotating as the device is moved, plus text readouts for
 *              ACC X/Y/Z and GYRO X/Y/Z updated continuously.
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
 * 2) Optionally adjust ANGLE_MODIFIER to change how strongly motion affects cube
 *    rotation, and NUM_PARTIAL_UPDATES_BEFORE_FULL_REFRESH to tune the refresh
 *    cadence.
 * 3) Upload the sketch to Inkplate 4 TEMPERA.
 * 4) Tilt and rotate the device; the cube rotation changes with acceleration.
 * 5) Watch live accelerometer and gyroscope readings printed below the cube.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-08-10
 * @license     GNU GPL V3
 **************************************************/

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
        numRefreshes = 0; // reset the counter
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
