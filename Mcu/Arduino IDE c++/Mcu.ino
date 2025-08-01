#include <WiFi.h>
#include <WiFiClient.h>
#include <math.h>

// WiFi Configuration
const char* ssid = "SARP";
const char* password = "Sarposky2011";

// TCP Server Configuration
const char* serverIP = "192.168.1.193";
const uint16_t serverPort = 5000;
WiFiClient client;

// AI Drawing Configuration
enum Shape { SQUARE, CIRCLE, TRIANGLE };
Shape currentShape = SQUARE;
const int canvasSize = 1000;
const int pointCount = 50;  // Points per shape
unsigned long shapeDuration = 20000;  // 20 seconds per shape
unsigned long lastShapeChange = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Connect to WiFi
  connectToWiFi();
  
  // Initialize random seed
  randomSeed(analogRead(0));
}

void loop() {
  // Maintain connections
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
    return;
  }
  
  if (!client.connected()) {
    connectToServer();
    return;
  }

  // Change shape periodically
  if (millis() - lastShapeChange > shapeDuration) {
    changeShape();
    lastShapeChange = millis();
  }

  // Generate and send coordinates
  generateAndSendCoordinates();
  
  delay(50);  // Small delay to prevent watchdog trigger
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.println("IP: " + WiFi.localIP().toString());
    connectToServer();
  } else {
    Serial.println("\nWiFi connection failed!");
  }
}

void reconnectWiFi() {
  Serial.println("Reconnecting to WiFi...");
  WiFi.reconnect();
  delay(5000);
}

void connectToServer() {
  Serial.print("Connecting to TCP server...");
  if (client.connect(serverIP, serverPort)) {
    Serial.println("connected!");
  } else {
    Serial.println("failed!");
  }
}

void changeShape() {
  currentShape = static_cast<Shape>((currentShape + 1) % 3);
  Serial.print("Changed to shape: ");
  switch(currentShape) {
    case SQUARE: Serial.println("SQUARE"); break;
    case CIRCLE: Serial.println("CIRCLE"); break;
    case TRIANGLE: Serial.println("TRIANGLE"); break;
  }
}

void generateAndSendCoordinates() {
  static unsigned long lastSendTime = 0;
  static int pointIndex = 0;
  
  if (millis() - lastSendTime < (shapeDuration / pointCount)) {
    return;
  }
  
  lastSendTime = millis();
  pointIndex = (pointIndex + 1) % pointCount;
  
  int x, y;
  float progress = (float)pointIndex / pointCount;
  
  // Generate coordinates based on current shape
  switch(currentShape) {
    case SQUARE:
      if (progress < 0.25) {
        x = 100 + progress * 800 * 4;
        y = 100;
      } else if (progress < 0.5) {
        x = 900;
        y = 100 + (progress - 0.25) * 800 * 4;
      } else if (progress < 0.75) {
        x = 900 - (progress - 0.5) * 800 * 4;
        y = 900;
      } else {
        x = 100;
        y = 900 - (progress - 0.75) * 800 * 4;
      }
      break;
      
    case CIRCLE:
      {
        float angle = progress * 2 * PI;
        int radius = 400;
        x = 500 + radius * cos(angle);
        y = 500 + radius * sin(angle);
      }
      break;
      
    case TRIANGLE:
      if (progress < 0.333) {
        x = 500 + (progress * 3) * 400;
        y = 900 - (progress * 3) * 800;
      } else if (progress < 0.666) {
        x = 900 - (progress - 0.333) * 3 * 800;
        y = 100 + (progress - 0.333) * 3 * 800;
      } else {
        x = 100 + (progress - 0.666) * 3 * 800;
        y = 900 - (progress - 0.666) * 3 * 400;
      }
      break;
  }
  
  // Constrain to canvas
  x = constrain(x, 0, canvasSize);
  y = constrain(y, 0, canvasSize);
  
  // Send coordinates
  String message = String(x) + "," + String(y) + "\n";
  client.print(message);
  
  Serial.print("Sent: ");
  Serial.print(message);
}