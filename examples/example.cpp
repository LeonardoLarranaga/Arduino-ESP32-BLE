#include "ESP32BLE.h"

int i = 0;

ESP32BLE bluetooth;

void onReceiveData(const String newValue);

void setup() {
  Serial.begin(115200);
  bluetooth.setup("Example", onReceiveData);
}

void loop() {
  i += 1;
  if (i % 5 == 0) bluetooth.send("Hello, World! " + String(i));
  delay(1000);
}

void onReceiveData(const String newValue) {
  Serial.println("Received data: " + newValue);
}