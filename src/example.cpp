#include "ArduinoESP32BLE.h"

int i = 0;

void onReceiveData(const String newValue);

void setup() {
  Serial.begin(115200);
  setupBluetooth("ArduinoESP32BLE Example", onReceiveData);
}

void loop() {
  i += 1;
  if (i % 5 == 0) sendBluetoothData("Hello, World! " + String(i)); 
  delay(1000);
}

void onReceiveData(const String newValue) {
  Serial.println("Received data: " + newValue);
}