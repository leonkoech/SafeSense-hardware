//#include "ussensor.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


// front left 
//#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
//#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// back left
#define  SERVICE_UUID  "0000aaaa-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID "0000bbbb-0000-1000-8000-00805F9B34FB"

BLEServer *pServer;
BLECharacteristic *pCharacteristic;
bool isConnected = false; // Flag to track device connection status

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      isConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
      isConnected = false;
    }
};

void setup() {
  SensorSetup();
   Serial.begin(115200);
  Serial.println("Starting BLE server...");

  BLEDevice::init("ESP32");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->setValue("Hello World");

  // Add the CCCD descriptor for the characteristic
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("Characteristic defined! Now you can read/write it.");

  // Print the device ID (MAC address)
  BLEAddress deviceAddress = BLEDevice::getAddress();
  Serial.print("Device ID: ");
  Serial.println(deviceAddress.toString().c_str());
}

void loop() {
  float sensorData = SensorLoop();
//  Serial.println();
  if (isConnected) {

    char buffer[10];
    dtostrf(sensorData, 6, 2, buffer);
    pCharacteristic->setValue(buffer);


    // Notify the connected device about the updated value
    pCharacteristic->notify();
    
    delay(1000); // Adjust the delay as needed based on your data rate
    
  }
}
