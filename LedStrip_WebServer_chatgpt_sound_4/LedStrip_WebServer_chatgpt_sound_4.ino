// Include the necessary libraries

// Pin definitions
#define SOUND_SENSOR_PIN 36  // Analog pin where the sound sensor is connected
#define LED_1_PIN 27         // Pin for LED 1 (Red)
#define LED_2_PIN 25         // Pin for LED 2 (Green)
#define LED_3_PIN 26         // Pin for LED 3 (Blue)

// Thresholds and settings
const int soundThreshold = 200; // Adjust sensitivity (lower = more sensitive)
const int maxAnalogValue = 2000; // Maximum analog value for the ESP32

void setup() {
  Serial.begin(115200);            // Start serial communication for debugging

  pinMode(SOUND_SENSOR_PIN, INPUT); // Set the sound sensor pin as input
  pinMode(LED_1_PIN, OUTPUT);      // Set LED 1 as output
  pinMode(LED_2_PIN, OUTPUT);      // Set LED 2 as output
  pinMode(LED_3_PIN, OUTPUT);      // Set LED 3 as output 

  digitalWrite(LED_1_PIN, LOW);    // Turn off LED 1
  digitalWrite(LED_2_PIN, LOW);    // Turn off LED 2
  digitalWrite(LED_3_PIN, LOW);    // Turn off LED 3
}

void loop() {
  // Read the analog value from the sound sensor
  int soundLevel = analogRead(SOUND_SENSOR_PIN);

  // Map the sound level to brightness levels for the LEDs
  int brightness1 = map(soundLevel, 500, maxAnalogValue, 0, 255); // Red LED reacts to low sound levels
  int brightness2 = map(soundLevel, maxAnalogValue / 3, (2 * maxAnalogValue) / 3, 0, 255); // Green LED reacts to mid sound levels
  int brightness3 = map(soundLevel, (2 * maxAnalogValue) / 3, maxAnalogValue, 0, 255); // Blue LED reacts to high sound levels

  // Ensure brightness values stay within range
  brightness1 = constrain(brightness1, 0, 255);
  brightness2 = constrain(brightness2, 0, 255);
  brightness3 = constrain(brightness3, 0, 255);

  // If the sound level is above the threshold, update the LEDs
  if (soundLevel > soundThreshold) {
    Serial.println(soundLevel);
    analogWrite(LED_1_PIN, brightness1); // Adjust brightness of LED 1 (Red)
    analogWrite(LED_2_PIN, brightness2); // Adjust brightness of LED 2 (Green)
    analogWrite(LED_3_PIN, brightness3); // Adjust brightness of LED 3 (Blue)
  }

  if (soundLevel > 0){// || brightness2 < soundThreshold || brightness3 < soundThreshold) {
    clearLEDs(); // Turn off the LEDs if below the threshold
    //Serial.print("Cleared LEDS\n");
    //soundLevel=0;
  }

  // Debugging output
  /*Serial.print("Sound Level: ");
  Serial.print(soundLevel);
  Serial.print(" | Brightness 1 (Red): ");
  Serial.print(brightness1);
  Serial.print(" | Brightness 2 (Green): ");
  Serial.print(brightness2);
  Serial.print(" | Brightness 3 (Blue): ");
  Serial.println(brightness3);*/

  delay(1); // Small delay to smooth out reactions
}

// Function to clear the LEDs
void clearLEDs() {
  digitalWrite(LED_1_PIN, LOW);
  digitalWrite(LED_2_PIN, LOW);
  digitalWrite(LED_3_PIN, LOW);
}
