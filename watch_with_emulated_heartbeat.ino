#include<BLEDevice.h>
#include<BLEServer.h>
#include<BLEUtils.h>
#include<BLE2902.h>

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int txValue = 0;

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6F400003-B5A3-F393-E0A9-E50F24DCCA9F"

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect (BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect (BLEServer* pServer) {
    deviceConnected = false;
    pServer->getAdvertising()->start();
  }
};

void setup() {
  Serial.begin(9600);
  // Create the BLE Device
  BLEDevice::init("ESP32");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY); //BLE2902 needed to notify
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting for a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    txValue = (int) random(50, 100); // Conversion of txValue
    char txString[8];
    dtostrf(txValue, 1, 2, txString);

    pCharacteristic->setValue(txString);
  // Notifying the connected client
    pCharacteristic->notify();
    Serial.println("Sent value: " + String (txString));
    delay(500);
  }
}
