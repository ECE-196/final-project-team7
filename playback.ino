#include <SPIFFS.h>

const int audioPin = 25;  // DAC pin
const int audioSampleRate = 8000;  // Match your audio sample rate

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  File audioFile = SPIFFS.open("/audio.wav", "r");
  if (!audioFile) {
    Serial.println("Failed to open file");
    return;
  }

  while (audioFile.available()) {
    uint8_t value = audioFile.read();
    dacWrite(audioPin, value);
    delayMicroseconds(1000000 / audioSampleRate);
  }

  audioFile.close();
}

void loop() {
  // Do nothing
}
