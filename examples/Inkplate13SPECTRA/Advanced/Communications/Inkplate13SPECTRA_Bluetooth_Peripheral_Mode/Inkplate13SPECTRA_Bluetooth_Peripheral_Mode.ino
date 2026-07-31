/**
 **************************************************
 * @file        Inkplate13SPECTRA_Bluetooth_Peripheral_Mode.ino
 * @brief       Bluetooth peripheral mode example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to use Inkplate as a peripheral device over Bluetooth.
 *              Upload this example to the Inkplate and connect your phone to it
 *              via Bluetooth. First pair the Inkplate with your phone in the
 *              Bluetooth settings on your phone, then open the Serial Bluetooth
 *              Terminal app (or another similar app) and find the Inkplate in
 *              the device list.
 *
 *              If Bluetooth starts successfully, you can send commands from your
 *              phone. Don't forget you need to send #L(1)* after each command to
 *              show it on the display (equal to display->display()).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable, smartphone
 * - Extra:      Bluetooth serial terminal app (e.g. Serial Bluetooth Terminal)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) Pair the Inkplate with your phone in the phone's Bluetooth settings.
 * 4) Open the Serial Bluetooth Terminal app and connect to the Inkplate.
 * 5) Send peripheral mode commands, terminating each with #L(1)* to refresh the
 *    display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-21
 **************************************************/

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
  BLEDevice::init("Inkplate 13 SPECTRA");

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