#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "HSS Hugo Morais";
const char* password = "abcdefgh";

// GPIO pins for outputs
const int outputPin1 = 25;
const int outputPin2 = 26;
const int outputPin3 = 27;

// Blink delays
const int delayOut1 = 10;  // Blink delay for output 1
const int delayOut2 = 15;  // Blink delay for output 2
const int delayOut3 = 20;  // Blink delay for output 3
const int delayOff = 100;  // Off delay for blinking

// Web server on port 80
AsyncWebServer server(80);

// Control flags
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

  // Define server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String html = "<html><body>";
    html += "<h1>ESP32 Web Server</h1>";
    html += "<button onclick=\"location.href='/start'\">Start Fading</button><br><br>";
    html += "<button onclick=\"location.href='/blink'\">Start Blinking</button><br><br>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/start", HTTP_GET, [](AsyncWebServerRequest* request) {
    String html = "<html><body>";
    html += "<h1>Fading Outputs...</h1>";
    html += "<button onclick=\"location.href='/stop'\">Stop Fading</button><br><br>";
    html += "</body></html>";
    request->send(200, "text/html", html);

    stopFading = false;
    xTaskCreatePinnedToCore(fadeAllOutputsSimultaneously, "FadeTask", 4096, NULL, 1, NULL, 0);
  });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest* request) {
    stopFading = true;
    stopBlinking = true;
    String html = "<html><body>";
    html += "<h1>Operation Stopped</h1>";
    html += "<p>Returning to home page...</p>";
    html += "<meta http-equiv='refresh' content='3;url=/' />";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/blink", HTTP_GET, [](AsyncWebServerRequest* request) {
    String html = "<html><body>";
    html += "<h1>Blinking Outputs...</h1>";
    html += "<button onclick=\"location.href='/stop'\">Stop Blinking</button><br><br>";
    html += "</body></html>";
    request->send(200, "text/html", html);

    stopBlinking = false;
    xTaskCreatePinnedToCore(blinkAllOutputs, "BlinkTask", 4096, NULL, 1, NULL, 0);
  });

  // Start server
  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  // No operations needed in the main loop, server handles requests asynchronously
}

void fadeAllOutputsSimultaneously(void* parameter) {
  while (!stopFading) {
    // Implement your fading logic here
    // This function will be called for fading operation
  }

  // Ensure all outputs are off after stopping
  digitalWrite(outputPin1, LOW);
  digitalWrite(outputPin2, LOW);
  digitalWrite(outputPin3, LOW);

  vTaskDelete(NULL); // Delete the task when done
}

void blinkAllOutputs(void* parameter) {
  unsigned long lastTime1 = 0, lastTime2 = 0, lastTime3 = 0;

  while (!stopBlinking) {
    unsigned long currentTime = millis();

    // Blink Output 1
    if (currentTime - lastTime1 >= delayOut1 + delayOff) {
      lastTime1 = currentTime;
      digitalWrite(outputPin1, !digitalRead(outputPin1));
    }

    // Blink Output 2
    if (currentTime - lastTime2 >= delayOut2 + delayOff) {
      lastTime2 = currentTime;
      digitalWrite(outputPin2, !digitalRead(outputPin2));
    }

    // Blink Output 3
    if (currentTime - lastTime3 >= delayOut3 + delayOff) {
      lastTime3 = currentTime;
      digitalWrite(outputPin3, !digitalRead(outputPin3));
    }
  }

  // Ensure all outputs are off after stopping
  digitalWrite(outputPin1, LOW);
  digitalWrite(outputPin2, LOW);
  digitalWrite(outputPin3, LOW);

  vTaskDelete(NULL); // Delete the task when done
}
