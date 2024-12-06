#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_wpa2.h>

// Your school Wi-Fi details
const char* ssid = "UCSD-PROTECTED";
const char* username = "vit008";
const char* password = "Van7Thanh7Thinh7*";

// MQTT server details
const char* mqttServer = "100.84.0.36"; // Replace with your MQTT broker IP
const int mqttPort = 1883;
const char* mqttTopic = "squirrel/detection";

// Actuator pins
#define LED_PIN 2
#define SPEAKER_PIN 21
#define MTR_HI 5
#define MTR_LO 18

WiFiClient espClient;
PubSubClient client(espClient);

// Actuation states
bool squirrelDetected = false; // Whether a squirrel is detected
bool escalationMode = false;   // Whether escalation has occurred

void setupWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    // WPA2-Enterprise setup
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t*)username, strlen(username)); 
    esp_wifi_sta_wpa2_ent_set_username((uint8_t*)username, strlen(username)); 
    esp_wifi_sta_wpa2_ent_set_password((uint8_t*)password, strlen(password)); 
    esp_wifi_sta_wpa2_ent_enable();

    WiFi.begin(ssid);
    Serial.println("Connecting to Wi-Fi...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected to Wi-Fi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void setupMQTT() {
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);

    while (!client.connected()) {
        Serial.println("Connecting to MQTT broker...");
        if (client.connect("ESP32Client")) {
            Serial.println("Connected to MQTT broker!");
            client.subscribe(mqttTopic);
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

    if (message == "SQUIRREL_DETECTED") {
        squirrelDetected = true;
        escalationMode = false; // Reset escalation mode
    } else if (message == "SQUIRREL_LEFT") {
        squirrelDetected = false;
    }
}

void playTone(int frequency, int duration) {
    unsigned long startTime = millis();
    int period = 1000000 / frequency; // Period in microseconds
    int halfPeriod = period / 2;

    digitalWrite(LED_PIN, HIGH); // Turn on LED during tone
    while (millis() - startTime < duration) {
        digitalWrite(SPEAKER_PIN, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(SPEAKER_PIN, LOW);
        delayMicroseconds(halfPeriod);
    }
    digitalWrite(LED_PIN, LOW); // Turn off LED after tone
}

void sprayWater() {
    digitalWrite(LED_PIN, HIGH); // Turn on LED during spray
    analogWrite(MTR_HI, 255);    // Activate motor for water spray
    analogWrite(MTR_LO, 0);
    delay(5000);                 // Spray for 5 seconds
    analogWrite(MTR_HI, 0);      // Stop motor
    analogWrite(MTR_LO, 0);
    digitalWrite(LED_PIN, LOW);  // Turn off LED after spray
}

void loop() {
    if (!client.connected()) {
        setupMQTT();
    }
    client.loop();

    if (squirrelDetected) {
        if (!escalationMode) {
            // Initial deterrent: play 20 kHz tone for 5 seconds
            playTone(20000, 5000);
            delay(2000);

            // Check if the squirrel is still detected
            if (squirrelDetected) {
                // Escalate: Play 18 kHz and 10 kHz tones for 5 seconds each
                playTone(18000, 5000);
                delay(2000);
                playTone(10000, 5000);
                delay(2000);

                // If still detected after escalation, enter alternate mode
                escalationMode = true;
            }
        } else {
            // Alternate spray and tone indefinitely until the squirrel leaves
            while (squirrelDetected) {
                playTone(20000, 5000);
                delay(2000);
                sprayWater();
                delay(2000);
            }
        }
    } else {
        escalationMode = false; // Reset if squirrel is no longer detected
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(SPEAKER_PIN, OUTPUT);
    pinMode(MTR_HI, OUTPUT);
    pinMode(MTR_LO, OUTPUT);

    setupWiFi();
    setupMQTT();
}
