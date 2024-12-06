#define SPEAKER_PIN 21 // Replace with your speaker's GPIO pin
#define LED 2          // Define the pin for the LED (optional)
#define MTR_HI 5       // Pin to control the high side of the motor
#define MTR_LO 18      // Pin to control the low side of the motor

int base_value = 0;                 // PWM value starts at 0
int step_value_for_base_value = 10; // Faster ramp-up
int time_for_delay = 5;             // Reduced delay for faster acceleration

void setup() {
  // Initialize pins for speaker, LED, and motor
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(MTR_HI, OUTPUT);
  pinMode(MTR_LO, OUTPUT);

  analogWrite(MTR_HI, 0);
  analogWrite(MTR_LO, 0);
}

void loop() {
  // LED Blinking
  digitalWrite(LED, HIGH); // Turn the LED on
  delay(150);
  digitalWrite(LED, LOW);  // Turn the LED off
  delay(150);

  // Run the motor at full speed forward to squeeze the bottle
  analogWrite(MTR_HI, 255); // Immediate full speed
  analogWrite(MTR_LO, 0);   // Ensure motor spins in one direction
  delay(1000);              // Keep squeezing for 1 second

  // Stop the motor to let it reset
  analogWrite(MTR_HI, 0);
  analogWrite(MTR_LO, 0);
  delay(1000);

  // Smooth acceleration to full speed
  while (base_value <= 255) {
    analogWrite(MTR_HI, base_value);
    analogWrite(MTR_LO, 0);
    base_value += step_value_for_base_value;
    delay(time_for_delay);
  }

  // Smooth deceleration to stop
  while (base_value >= 0) {
    analogWrite(MTR_HI, base_value);
    analogWrite(MTR_LO, 0);
    base_value -= step_value_for_base_value;
    delay(time_for_delay);
  }

  // Generate 22 kHz tone
  for (int i = 0; i < 1000; i++) {
    digitalWrite(SPEAKER_PIN, HIGH);
    delayMicroseconds(22); // 22 kHz half-period
    digitalWrite(SPEAKER_PIN, LOW);
    delayMicroseconds(22);
  }
  delay(1000); // Pause for 1 second

  // Generate 24 kHz tone
  for (int i = 0; i < 1000; i++) {
    digitalWrite(SPEAKER_PIN, HIGH);
    delayMicroseconds(21); // 24 kHz half-period
    digitalWrite(SPEAKER_PIN, LOW);
    delayMicroseconds(21);
  }
  delay(1000); // Pause for 1 second
}
