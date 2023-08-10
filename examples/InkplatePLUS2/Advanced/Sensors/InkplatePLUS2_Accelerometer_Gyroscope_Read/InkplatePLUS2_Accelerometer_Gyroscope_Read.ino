/*
   InkplatePLUS2_Accelerometer_Gyroscope_Read example for Soldered Inkplate PLUS2
   For this example you will need only a USB-C cable and Inkplate PLUS2.
   Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to read from the built-in LSM6DS3 gyroscope/accelerometer.
   The rotation of the 3D Cube is modified by the accelerometer.
   The gyroscope changes TODO

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   10 Aug 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)
// This helps with faster refreshes

// Variable that keeps count on how many times the screen has been partially updated
int n = 0;

// Variables which are used for drawing the 3D Cube

// Cube vertices
float cube[8][3] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}};

// Cube edges
int edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
    {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
    {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
};

// Remember the previously measured acceleraton
// The differences in accelerations will be used to compute the angles which rotate the 3D cube
float previousAccelX, previousAccelY, previousAccelZ;

void setup()
{
    Serial.begin(115200); // TODO Remove this later
    display.begin();      // Init Inkplate library (you should call this function ONLY ONCE)
    display.display();    // Put clear image on display

    display.accelerometer.begin(); // Init the sensor

    // Get the initial readings and remember them
    previousAccelX = display.accelerometer.readFloatAccelX();
    previousAccelY = display.accelerometer.readFloatAccelY();
    previousAccelZ = display.accelerometer.readFloatAccelZ();
}
void loop()
{
    // Get the new readings from the accelerometer
    float newAccelX = display.accelerometer.readFloatAccelX();
    float newAccelY = display.accelerometer.readFloatAccelY();
    float newAccelZ = display.accelerometer.readFloatAccelZ();

    // Calculate the differences
    float accelDiffX = previousAccelX - newAccelX;
    float accelDiffY = previousAccelY - newAccelY;
    float accelDiffZ = previousAccelZ - newAccelZ;

    // Create variable which remembers the angles used to draw the cube
    static float angleX = 0;
    static float angleY = 0;
    static float angleZ = 0;

    // Use the difference between measured accelerations as differences in angles
    // between two renders
    angleX += accelDiffX;
    angleY += accelDiffY;
    angleZ += accelDiffZ;

    // Remember the old state for the next iteration of the loop
    previousAccelX = newAccelX;
    previousAccelY = newAccelY;
    previousAccelZ = newAccelZ;

    // Read acceleration and print it on serial
    // TODO Remove this
    Serial.print("ACCX:");
    Serial.print(display.accelerometer.readFloatAccelX(), 4);
    Serial.print(",");
    Serial.print("ACCY:");
    Serial.print(display.accelerometer.readFloatAccelY(), 4);
    Serial.print(",");
    Serial.print("ACCZ:");
    Serial.print(display.accelerometer.readFloatAccelZ(), 4);
    Serial.print(",");

    // Read rotational velocity and print it on serial
    // Note: for precise results, you have to calibrate the gyro
    Serial.print("GYROX:");
    Serial.print(display.accelerometer.readFloatGyroX(), 4);
    Serial.print(",");
    Serial.print("GYROY:");
    Serial.print(display.accelerometer.readFloatGyroY(), 4);
    Serial.print(",");
    Serial.print("GYROZ:");
    Serial.print(display.accelerometer.readFloatGyroZ(), 4);
    Serial.println();

    // Clear what was previously on the display
    display.clearDisplay();

    // Draw the cube
    for (int i = 0; i < 12; i++)
    {
        // Get the start and end vertices for this edge
        float *v1 = cube[edges[i][0]];
        float *v2 = cube[edges[i][1]];

        // Rotate and project the vertices to 2D
        int x1, y1, x2, y2;
        project(v1, angleX, angleY, angleZ, &x1, &y1);
        project(v2, angleX, angleY, angleZ, &x2, &y2);

        // Draw the edge
        display.drawLine(x1, y1, x2, y2, BLACK);
    }

    // Update the screen
    if (n > 20)
    {                      // Check if you need to do full refresh or you can do partial update
        display.display(); // Do a full refresh
        n = 0;
    }
    else
    {
        display.partialUpdate(false, true); // Do partial update
        n++;                                // Keep track on how many times screen has been partially updated
    }

    // Wait a short time
    delay(50);
}

// This functions projects 3D space onto 2D with a set rotation
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