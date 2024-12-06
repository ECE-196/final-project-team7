#include <BluetoothSerial.h>

// Create a Bluetooth Serial object
BluetoothSerial SerialBT;

// Pin for actuator (e.g., motor control)
const int actuatorPin = 2;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);

    // Initialize Bluetooth
    SerialBT.begin("ESP32-Squirrel-Detector"); // Bluetooth name
    Serial.println("Bluetooth device is ready to pair");

    // Set up actuator pin
    pinMode(actuatorPin, OUTPUT);
    digitalWrite(actuatorPin, LOW); // Ensure actuator is off
}

void loop() {
    if (SerialBT.available()) {
        String message = SerialBT.readStringUntil('\n'); // Read message from Mac
        Serial.println("Message received: " + message);

        if (message == "ACTUATE") {
            Serial.println("Triggering actuator!");
            digitalWrite(actuatorPin, HIGH); // Activate actuator
            delay(1000); // Keep actuator on for 1 second
            digitalWrite(actuatorPin, LOW);  // Deactivate actuator
        }
    }
}