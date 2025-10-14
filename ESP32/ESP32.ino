#include <WiFi.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <HTTPClient.h>

const char* ssid = "2.4G SSID";
const char* password = "@20Wangcl01phil";

const char* serverUrl = "http://192.168.1.17:1880";

const char* deviceId = "e5f6a7b8-1234-4cde-89ab-9876543210ab";
const char* deviceName = "ESP32 Test Device";
const char* deviceDescription = "ESP32 with GPS and Wi-Fi for testing";

TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // UART1 for GPS
const int RXPin = 16;  // GPS TX → ESP32 RX1
const int TXPin = 17;  // GPS RX → ESP32 TX1
const uint32_t gpsBaud = 9600;

double latitude = 40.712776;
double longitude = -74.005974;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1000; // 1 second

void setup() {
  Serial.begin(115200);
  delay(1000);

  gpsSerial.begin(gpsBaud, SERIAL_8N1, RXPin, TXPin);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  sendDeviceInfo();
}

void sendDeviceInfo() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(serverUrl) + "/device");
    http.addHeader("Content-Type", "application/json");

    String json = "{";
    json += "\"id\":\"" + String(deviceId) + "\",";
    json += "\"name\":\"" + String(deviceName) + "\",";
    json += "\"description\":\"" + String(deviceDescription) + "\"";
    json += "}";

    int httpResponseCode = http.POST(json);
    if (httpResponseCode > 0) {
      Serial.printf("Device info sent, response code: %d\n", httpResponseCode);
    } else {
      Serial.printf("Error sending device info: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  }
}

void sendLocation(double lat, double lon) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(serverUrl) + "/location");
    http.addHeader("Content-Type", "application/json");

    String json = "{";
    json += "\"device_id\":\"" + String(deviceId) + "\",";
    json += "\"latitude\":" + String(lat, 6) + ",";
    json += "\"longitude\":" + String(lon, 6);
    json += "}";

    int httpResponseCode = http.POST(json);
    if (httpResponseCode > 0) {
      Serial.printf("Location sent, response code: %d\n", httpResponseCode);
    } else {
      Serial.printf("Error sending location: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  }
}

// Simulate random movement
void simulateMovement(double &lat, double &lon, double maxDistanceMeters = 30.0) {
  const double earthRadius = 6371000.0;
  double distance = ((double)rand() / RAND_MAX) * maxDistanceMeters;
  double angle = ((double)rand() / RAND_MAX) * 2.0 * PI;

  double deltaLat = distance * cos(angle) / earthRadius;
  double deltaLon = distance * sin(angle) / (earthRadius * cos(lat * PI / 180.0));

  lat += deltaLat * 180.0 / PI;
  lon += deltaLon * 180.0 / PI;
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastSendTime >= sendInterval) {
    lastSendTime = currentMillis;

    // Send location to Node-RED
    sendLocation(latitude, longitude);

    // Simulate movement
    simulateMovement(latitude, longitude, 30);
  }
}