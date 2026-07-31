
/**
 **************************************************
 * @file        Inkplate6COLOR_Bluetooth_Serial.ino
 * @brief       Bluetooth SPP serial bridge example for Soldered Inkplate 6COLOR.
 *
 * @details     Demonstrates how to use classic Bluetooth (SPP profile) on the
 *              Inkplate 6COLOR to exchange text data between the board and a
 *              smartphone. Text received from the phone is shown on the
 *              Inkplate display and logged to the Serial Monitor, while text
 *              sent from the Inkplate (via Serial Monitor) is forwarded to
 *              the connected phone.
 *
 *              Expected output is text received over Bluetooth shown on the
 *              Inkplate display, with sent and received messages logged in the
 *              Serial Monitor. Bluetooth pairing and app behavior may differ
 *              between Android and iOS; some iOS apps do not support classic
 *              Bluetooth SPP.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      Bluetooth serial terminal app (e.g. Serial Bluetooth Terminal)
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6COLOR"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6COLOR.
 * 3) Pair the Inkplate with your phone in Bluetooth settings.
 * 4) Open a Bluetooth serial terminal app and connect to Inkplate.
 * 5) Send text from the phone and verify it appears on the Inkplate display
 *    and in the Serial Monitor.
 * 6) Send text from the Serial Monitor and verify it appears in the phone app.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-17
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// Include Inkplate and BLE libraries to the sketch
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "Inkplate.h"

// BLE UART service UUID and RX/TX characteristics for serial-style communication over Bluetooth LE
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"   // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"   // For data coming IN to Inkplate
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"   // For data coming OUT from Inkplate

Inkplate inkplate;         // Create an object on Inkplate library and also set library into 1-bit mode (BW)

BLEServer *pServer = NULL;            // Pointer to BLEServer
BLECharacteristic *pTxCharacteristic; // Pointer to TX characteristic
bool deviceConnected = false;         // Is client currently connected       
bool oldDeviceConnected = false;      // Store the previous connection

// Connect/Disconnect callback
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) 
  {
    deviceConnected = true;
    Serial.println("Device connected");
  };

  void onDisconnect(BLEServer *pServer) 
  {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

// On receive callback
class MyCallbacks : public BLECharacteristicCallbacks 
{
  void onWrite(BLECharacteristic *pCharacteristic) 
  {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) 
    {
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++) 
      {
        Serial.print(rxValue[i]);
      }
      Serial.println();
    }
  }
};

void setup()
{
  inkplate.begin();                        // Init Inkplate library (you should call this function ONLY ONCE)
  inkplate.clearDisplay();                 // Clear frame buffer of display
  inkplate.setTextSize(4);                 // Scale text to be 5 times bigger then original (5x7 px)

  // Display initial text
  inkplate.setTextColor(BLACK);
  inkplate.println("Bluetooth Serial");
  inkplate.println("Example");
  inkplate.println();
  inkplate.setTextSize(3);

  // Init serial communication
  Serial.begin(115200);
  // Create the BLE Device
  BLEDevice::init("Inkplate 6COLOR");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic (TX)
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  // Let client turn On/Off notifications for this characteristic (needed for ESP32 BLE)
  // NOTE: Descriptor 2902 is not required when using NimBLE as it is automatically added based on the characteristic properties
  pTxCharacteristic->addDescriptor(new BLE2902());

  // Create a BLE Characteristic (RX)
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  // Assign a callback handler
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();

  // Check if Bluetooth controller is enabled
  if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) 
  {
    Serial.println("BLE controller failed to start");
    inkplate.println("BLE FAILED");
  } 
  else 
  {
    // Bluetooth started successfully, print the messages on the screen
    inkplate.println("The device started, now you can ");
    inkplate.println("pair it with Bluetooth and send");
    inkplate.println("messages.");
    inkplate.println();
    inkplate.println("Open Serial Monitor at 115200");
    inkplate.println("baud!");
  }

  // Update the display
  inkplate.display();
}

void loop()
{
  // If Bluetooth is connected, check for available data on Serial and send that data via Bluetooth to the connected device
  if (deviceConnected) 
  {
    if (Serial.available()) 
    {
      String s = Serial.readStringUntil('\n') + "\n";                       // Load the message from serial
      Serial.printf("Sent value: %s\n", s.c_str());
      pTxCharacteristic->setValue((uint8_t *)s.c_str(), strlen(s.c_str())); // Convert the message into bytes
      pTxCharacteristic->notify();                                          // Transmit the message
    }
  }

  // When a client disconnects, restart BLE advertising so that we can accept new connections
  if (!deviceConnected && oldDeviceConnected) 
  {
    delay(500);                       // Allow bluetooth stack some time
    pServer->startAdvertising();      // Restart advertising
    oldDeviceConnected = false;       // Update state
  }

  // Detect new connections
  if (deviceConnected && !oldDeviceConnected) 
  {
    // Do stuff here on connecting...
    oldDeviceConnected = true;      // Update the old state
  }
}
