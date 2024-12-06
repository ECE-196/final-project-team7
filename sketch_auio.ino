#include <WiFi.h>
#include <PubSubClient.h>

// AP Configuration
const char* apSSID = "ESP32-AP";        // SSID of the ESP32 Wi-Fi
const char* apPassword = "12345678";   // Password for the ESP32 Wi-Fi

// MQTT Configuration
const char* mqttServer = "192.168.4.1";  // The ESP32 itself will act as the broker host
const int mqttPort = 1883;
const char* mqttTopic = "squirrel/detection";

WiFiClient espClient;
PubSubClient client(espClient);

void setupAccessPoint() {
    // Start the ESP32 as an access point
    WiFi.softAP(apSSID, apPassword);

    // Print the IP address of the ESP32 AP
    Serial.print("Access Point started! IP Address: ");
    Serial.println(WiFi.softAPIP());
}

void setupMQTT() {
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);

    // Connect to MQTT broker (running on the ESP32 itself)
    while (!client.connected()) {
        Serial.println("Connecting to MQTT broker...");
        if (client.connect("ESP32Client")) {
            Serial.println("Connected to MQTT broker!");
            client.subscribe(mqttTopic); // Subscribe to the topic
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println("Message received: " + message);

    if (message == "ACTUATE") {
        Serial.println("Triggering actuators!");
        // Add your actuator code here (e.g., motor control)
    }
}

void setup() {
    Serial.begin(115200);

    // Set up the ESP32 as an Access Point
    setupAccessPoint();

    // Set up MQTT
    setupMQTT();
}

void loop() {
    if (!client.connected()) {
        setupMQTT();
    }
    client.loop();
}