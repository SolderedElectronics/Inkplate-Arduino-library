/*
   Inkplate7SPECTRA_Bluetooth_Peripheral_Mode example for Soldered Inkplate 7 SPECTRA
   For this example you will need USB cable and an Inkplate 7 SPECTRA
   Select "Soldered Inkplate 7 SPECTRA" from Tools -> Board menu.

   This example shows how to use Inkplate as a peripheral device over Bluetooth.

   Upload this example to the Inkplate and connect your phone to it via Bluetooth.
   First, you have to pair the Inkplate with your phone in Bluetooth settings in your phone, then go to the
   Serial Bluetooth Terminal app and you can find the Inkplate in the device list. You can use another similar app.
   If Bluetooth starts successfully, you can send commands from your phone. Don't forget you need to send #L(1)* after
   each command to show it on the display (equal to display->display()).

  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  21 January 2026 by Soldered Electronics
*/

// Include Inkplate and BLE libraries to the sketch
#include "Inkplate.h"
#include "BLESerial.h"
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Include peripheral functions
#include "Peripheral.h"

// BLE UART service UUID and RX/TX characteristics for serial-style communication over Bluetooth LE
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"   // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"   // For data coming IN to Inkplate
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"   // For data coming OUT from Inkplate

BLEServer *pServer = NULL;            // Pointer to BLEServer
BLECharacteristic *pTxCharacteristic; // Pointer to TX characteristic
bool deviceConnected = false;         // Is client currently connected       
bool oldDeviceConnected = false;      // Store the previous connection

// Create an object on Inkplate library
Inkplate inkplate;

// Size of buffer for receiving commands
#define BUFFER_SIZE 1000

// Temporary buffer to send to Peripheral mode code
char commandBuffer[BUFFER_SIZE + 1];

// Connect/Disconnect callback
class MyServerCallbacks : public BLEServerCallbacks 
{
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

// On receive callback - when data passed over Bluetooth, save it and pass to Peripheral.h code
class MyCallbacks : public BLECharacteristicCallbacks 
{
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0)
    {
      size_t len = rxValue.length();
      if (len > BUFFER_SIZE)
      {
        len = BUFFER_SIZE;
      }
      memcpy(commandBuffer, rxValue.c_str(), len);
      commandBuffer[len] = 0;

      // Function in Peripheral.h
      run(commandBuffer, len, &inkplate, pTxCharacteristic);
    }
  }
};

void setup() // Initialize everything
{
  // Init serial communication
  Serial.begin(115200);
  
  // Init Inkplate library (you should call this function ONLY ONCE)
  inkplate.begin();

  // Create the BLE Device
  BLEDevice::init("Inkplate 7 SPECTRA");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
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
    Serial.println("BLE controller failed to start.");
  } 
  else 
  {
    Serial.println("The device started, now you can pair it with Bluetooth and send commands.")
  }
}

void loop()
{
  // When a client disconnects, restart BLE advertising so that we can accept new connections
  if (!deviceConnected && oldDeviceConnected) 
  {
    delay(500);                   // Allow bluetooth stack some time
    pServer->startAdvertising();  // Restart advertising
    oldDeviceConnected = false;
  }

  // Detect new connections
  if (deviceConnected && !oldDeviceConnected) 
  {
    oldDeviceConnected = true;
  }
}