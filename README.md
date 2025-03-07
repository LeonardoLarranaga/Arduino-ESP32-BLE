# ESP32BLE.h
**An ESP32 class for BLE using the Arduino framework.**

A simple header file for easily setting up a Bluetooth Low Energy service for sending and receiving data with ESP32.

## Usage
Just drop the `ESP32BLE.h` file into your project.

To simply setup the service:
```c++
ESP32BLE bluetooth;
void setup() {
  bluetooth.setup("Example");
}
```

To trigger a function upon receiving data:
```c++
ESP32BLE bluetooth;

void onReceiveData(const String newValue);

void setup() {
  Serial.begin(115200);
  bluetooth.setup("Example", onReceiveData);
}

void onReceiveData(const String newValue) {
  Serial.println("Received data: " + newValue);
}
```

To send data:
```c++
int i = 0;
void loop() {
  i += 1;
  bluetooth.send(i);
  delay(1000);
}
```
