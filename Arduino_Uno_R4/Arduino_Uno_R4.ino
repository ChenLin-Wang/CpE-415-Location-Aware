#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

TinyGPSPlus gps;
SoftwareSerial mygps(4, 3);

const char* ssid = "CL";
const char* password = "12345678";

const char* serverAddress = "192.168.60.45";
int port = 1880;

double test_lat = 10.69971, test_lng = 122.56288;
const char* id = "a1b2c3d4-e5f6-47a8-9b0c-123456789abc";
const char* name = "Arduino Uno Test Device";
const char* description = "device for testing - Arduino Uno R4 with WIFI";

WiFiClient wifi;
HttpClient client(wifi, serverAddress, port);

void sendDeviceInfo() {
  String json = "{\"id\":\"" + String(id) + "\"," 
                "\"name\":\"" + String(name) + "\"," 
                "\"description\":\"" + String(description) + "\"}";

  Serial.println("Registering device...");

  client.beginRequest();
  client.post("/device");
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", json.length());
  client.beginBody();
  client.print(json);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Device register response: ");
  Serial.println(statusCode);
}

void sendLocation(double lat, double lng) {
  String json = "{\"device_id\":\"" + String(id) + "\"," 
                "\"latitude\":" + String(lat, 6) + "," 
                "\"longitude\":" + String(lng, 6) + "}";

  Serial.print("Sending location: ");
  Serial.print(lat, 6);
  Serial.print(", ");
  Serial.print(lng, 6);

  client.beginRequest();
  client.post("/location");
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", json.length());
  client.beginBody();
  client.print(json);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print(" [");
  Serial.print(statusCode);
  Serial.println("]");
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  mygps.begin(9600);

  connectWiFi();

  sendDeviceInfo();
}

// fake data generate -- for testing
void simulateMovement(double lat, double lng, double &newLat, double &newLng, double maxDistanceMeters) {
  const double earthRadius = 6371000.0;

  double distance = ((double)rand() / RAND_MAX) * maxDistanceMeters;
  double angle = ((double)rand() / RAND_MAX) * 2.0 * PI;

  double deltaLat = distance * cos(angle) / earthRadius;
  double deltaLng = distance * sin(angle) / (earthRadius * cos(lat * PI / 180.0));

  newLat = lat + deltaLat * 180.0 / PI;
  newLng = lng + deltaLng * 180.0 / PI;
}

void loop() {
  // while (mygps.available() > 0) {
  //   gps.encode(mygps.read());

  //   if (gps.location.isUpdated()) {
  //     double lat = gps.location.lat();
  //     double lng = gps.location.lng();

  //     Serial.print("Latitude = ");
  //     Serial.print(lat, 6);
  //     Serial.print(", Longitude = ");
  //     Serial.println(lng, 6);

  //     sendLocation(lat, lng);
  //   }
  // }

  // fake random data -- for testing
  sendLocation(test_lat, test_lng);
  double lat, lng;
  simulateMovement(test_lat, test_lng, lat, lng, 30);
  test_lat = lat;
  test_lng = lng;

  delay(1000);
}