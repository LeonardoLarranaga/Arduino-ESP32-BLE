#ifndef ESP32BLE_H
#define ESP32BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID "74a575e4-ffdd-4a90-8cb3-de2e85648de7"
#define SEND_UUID "8a97c3d6-6a3b-4e5c-8bfa-3e5d9a6d69a1"
#define RECEIVE_UUID "7bd70539-5e32-4b6b-91b0-50b2b5af7937"

bool verbose;

/// @brief Callback function type for receiving data over Bluetooth.
typedef void (*onReceiveValueCallback)(const String newValue);

/// @brief The BLECharacteristic object for sending data
BLECharacteristic *sendCharacteristic;

/// @brief Callbacks for receiving data over Bluetooth
class BLECallback : public BLECharacteristicCallbacks {
public:
  BLECallback(onReceiveValueCallback callback) : onReceiveValueFunc(callback) {}

  void onWrite(BLECharacteristic *pCharacteristic) override {
    const String value = pCharacteristic->getValue().c_str();
    if (onReceiveValueFunc != nullptr) {
        if (verbose) Serial.println("Data received via Bluetooth: " + value);
        // Call the user-defined callback with the received value
        onReceiveValueFunc(value);
    }
  }

private:
  onReceiveValueCallback onReceiveValueFunc;  // Store the callback function
};

/// @brief Sets up the Bluetooth service with the given name and callback function.
/// @param name The name of the Bluetooth service.
/// @param callback The callback function to be called when data is received.
/// @param verbose Whether to Serial.print debug messages (Bluetooth is setup, a message is sent/received).
void setupBluetooth(const String name, onReceiveValueCallback callback = {}, const bool v = false) {
  // Initialize the BLE device with the name received as a parameter
  BLEDevice::init(name.c_str());

  // Create a BLE server to manage connections
  BLEServer *receiveServer = BLEDevice::createServer();

  // Create a new service on the BLE server with a unique service UUID
  BLEService *receiveService = receiveServer->createService(SERVICE_UUID);

  // Create a characteristic within the service for receiving data
  // BLECharacteristic::PROPERTY_WRITE    -> Allows clients to write data to it
  // BLECharacteristic::PROPERTY_WRITE_NR -> Allows writing without response
  BLECharacteristic *receiveCharacteristic = receiveService->createCharacteristic(
    RECEIVE_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );

  // Add a descriptor to enable notifications and indications
  receiveCharacteristic->addDescriptor(new BLE2902());

  // Set the callback for receiving data
  receiveCharacteristic->setCallbacks(new BLECallback(callback));

  // Create a characteristic within the service for sending data
  sendCharacteristic = receiveService->createCharacteristic(
    SEND_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  // Add a descriptor to enable notifications and indications
  sendCharacteristic->addDescriptor(new BLE2902());

  // Start the service to make it available to clients
  receiveService->start();

  // Configure BLE advertising to broadcast the service UUID, enabling discovery by clients
  BLEAdvertising *receiveAdvertising = BLEDevice::getAdvertising();
  receiveAdvertising->addServiceUUID(SERVICE_UUID);
  receiveAdvertising->setScanResponse(true);  // Respond with additional data in scan response
  receiveAdvertising->setMinPreferred(0x06);  // Set preferred advertising interval minimum
  receiveAdvertising->setMaxPreferred(0x12);  // Set preferred advertising interval maximum

  verbose = v;

  // Start advertising to allow clients to connect to the device
  BLEDevice::startAdvertising();
  if (v) Serial.println("\n" + name + " Bluetooth service has started.");
}

/// @brief Sends data over Bluetooth.
/// @tparam T Any type of data.
/// @param data The data to send.
template <typename T>
void sendBluetoothData(T data) {
    if (sendCharacteristic == nullptr) return;
    String sendData = String(data);
    sendCharacteristic->setValue(sendData.c_str());
    sendCharacteristic->notify();
    if (verbose) Serial.println("Data sent via Bluetooth: " + sendData);
}

#endif