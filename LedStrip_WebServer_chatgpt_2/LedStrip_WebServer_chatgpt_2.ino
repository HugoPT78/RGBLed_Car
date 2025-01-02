#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "HSS Hugo Morais";
const char* password = "abcdefgh";

// GPIO pins for outputs
const int outputPin1 = 25;
const int outputPin2 = 26;
const int outputPin3 = 27;

// Fading parameters
const int delayOut1 = 10;  // Fading delay for output 1
const int delayOut2 = 15;  // Fading delay for output 2
const int delayOut3 = 20;  // Fading delay for output 3
const int maxBrightness = 255; // Maximum brightness for PWM
const int delayOff = 100;  // Off delay for blinking

// Web server on port 80
WebServer server(80);

// Volatile variables for control
volatile bool stopFading = false;
volatile bool stopBlinking = false;

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);

  // Set pins as output
  pinMode(outputPin1, OUTPUT);
  pinMode(outputPin2, OUTPUT);
  pinMode(outputPin3, OUTPUT);

  // Start WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define web server routes
  server.on("/", handleRoot);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.on("/blink", handleBlink);

  // Start the server
  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>ESP32 Web Server</h1>";
  html += "<button onclick=\"location.href='/start'\">Start Fading</button><br><br>";
  html += "<button onclick=\"location.href='/blink'\">Start Blinking</button><br><br>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void fadeOutput(int pin, int delayTime, volatile bool &stopFlag) {
  int brightness = 0;
  int direction = 1;

  while (!stopFlag) {
    // Increment or decrement brightness
    brightness += direction;
    if (brightness >= maxBrightness || brightness <= 0) {
      direction *= -1; // Reverse direction
    }

    // Simulate PWM by turning the pin on and off
    int onTime = brightness; // On time proportional to brightness
    int offTime = maxBrightness - brightness; // Off time complements on time
    digitalWrite(pin, HIGH);
    delayMicroseconds(onTime * delayTime); // Adjust delay for smooth fading
    digitalWrite(pin, LOW);
    delayMicroseconds(offTime * delayTime);

    // Check for stop condition
    server.handleClient();
  }

  // Ensure the pin is turned off when stopping
  digitalWrite(pin, LOW);
}

void fadeAllOutputsSimultaneously() {
  stopFading = false;

  while (!stopFading) {
    fadeOutput(outputPin1, delayOut1, stopFading);
    fadeOutput(outputPin2, delayOut2, stopFading);
    fadeOutput(outputPin3, delayOut3, stopFading);

    server.handleClient(); // Allow server handling during fading
  }
}

void blinkAllOutputs() {
  stopBlinking = false;

  while (!stopBlinking) {
    // Turn all outputs ON
    digitalWrite(outputPin1, HIGH);
    digitalWrite(outputPin2, HIGH);
    digitalWrite(outputPin3, HIGH);
    delay(delayOff);

    // Turn all outputs OFF
    digitalWrite(outputPin1, LOW);
    digitalWrite(outputPin2, LOW);
    digitalWrite(outputPin3, LOW);
    delay(delayOff);

    // Check for stop condition
    server.handleClient();
  }

  // Ensure all outputs are off after stopping
  digitalWrite(outputPin1, LOW);
  digitalWrite(outputPin2, LOW);
  digitalWrite(outputPin3, LOW);
}

void handleStart() {
  String html = "<html><body>";
  html += "<h1>Fading Outputs...</h1>";
  html += "<button onclick=\"location.href='/stop'\">Stop Fading</button><br><br>";
  html += "</body></html>";
  server.send(200, "text/html", html);

  fadeAllOutputsSimultaneously(); // Fades all outputs simultaneously
}

void handleStop() {
  stopFading = true;   // Signal to stop the fading loop
  stopBlinking = true; // Signal to stop the blinking loop

  String html = "<html><body>";
  html += "<h1>Operation Stopped</h1>";
  html += "<p>Returning to home page...</p>";
  html += "<meta http-equiv='refresh' content='3;url=/' />";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleBlink() {
  String html = "<html><body>";
  html += "<h1>Blinking Outputs...</h1>";
  html += "<button onclick=\"location.href='/stop'\">Stop Blinking</button><br><br>";
  html += "</body></html>";
  server.send(200, "text/html", html);

  blinkAllOutputs(); // Starts blinking all outputs
}
