#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "HSS Hugo Morais";
const char* password = "abcdefgh";

// GPIO pins for outputs
const int outputPin1 = 25; // Adjust pin numbers as needed
const int outputPin2 = 26;
const int outputPin3 = 27;

// GPIO delays
const int delayOut1 = 10; // Adjust pin numbers as needed
const int delayOut2 = 15;
const int delayOut3 = 20;

const int delayOff=100;

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

void fadeAllOutputsSimultaneously() {
  int brightness1 = 0, brightness2 = 0, brightness3 = 0;
  int direction1 = 1, direction2 = 1, direction3 = 1;
  unsigned long lastTime1 = 0, lastTime2 = 0, lastTime3 = 0;

  stopFading = false; // Reset the stop flag

  while (!stopFading) {
    unsigned long currentTime = millis();

    // Handle Output 1 fading
    if (currentTime - lastTime1 >= delayOut1) {
      lastTime1 = currentTime;
      brightness1 += direction1;
      if (brightness1 >= 255 || brightness1 <= 0) direction1 *= -1;
      analogWrite(outputPin1, brightness1);
    }

    // Handle Output 2 fading
    if (currentTime - lastTime2 >= delayOut2) {
      lastTime2 = currentTime;
      brightness2 += direction2;
      if (brightness2 >= 255 || brightness2 <= 0) direction2 *= -1;
      analogWrite(outputPin2, brightness2);
    }

    // Handle Output 3 fading
    if (currentTime - lastTime3 >= delayOut3) {
      lastTime3 = currentTime;
      brightness3 += direction3;
      if (brightness3 >= 255 || brightness3 <= 0) direction3 *= -1;
      analogWrite(outputPin3, brightness3);
    }

    // Check for stop condition
    delay(1); // Small delay to allow for server checks
    server.handleClient();
  }

  // Set all outputs to 0 after stopping
  analogWrite(outputPin1, 0);
  analogWrite(outputPin2, 0);
  analogWrite(outputPin3, 0);
}

void blinkAllOutputs() {
  unsigned long lastTime1 = 0, lastTime2 = 0, lastTime3 = 0;
  stopBlinking = false; // Reset the stop flag

  while (!stopBlinking) {
    // Turn all outputs ON
    unsigned long currentTime = millis();

    // Handle Output 1 fading
    if (currentTime - lastTime1 >= delayOut1) {
      lastTime1 = currentTime;
      digitalWrite(outputPin1, HIGH);
      delay(delayOff);
      digitalWrite(outputPin1, LOW);
    };
    if (currentTime - lastTime2 >= delayOut2) {
      lastTime1 = currentTime;
      digitalWrite(outputPin2, HIGH);
      delay(delayOff);
      digitalWrite(outputPin2, LOW);
    };
    if (currentTime - lastTime3 >= delayOut3) {
      lastTime1 = currentTime;
      digitalWrite(outputPin3, HIGH);
      delay(delayOff);
      digitalWrite(outputPin3, LOW);
    };

    // Check for stop condition
    server.handleClient();
  }

  // Ensure all outputs are off after stopping
  digitalWrite(outputPin1, LOW);
  digitalWrite(outputPin2, LOW);
  digitalWrite(outputPin3, LOW);
  stopBlinking=true;
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
