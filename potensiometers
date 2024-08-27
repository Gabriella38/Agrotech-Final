#include <WiFi.h>
#include "ThingSpeak.h"

// WiFi network credentials
const char* ssid = "Gabriella";
const char* password = "38383838";

// ThingSpeak channel and API key
unsigned long myChannelNumber = 2595959; // Replace with your channel number
const char* myWriteAPIKey = "YIUFQHOWQVZN72JP";

// Define analog sensor pins
int sensorPin_CO2 = A0;   // CO2 sensor pin (analog input A4)
int sensorPin_O2 = A1;    // O2 sensor pin (analog input A1)

// Variables for sensor readings and calculated values
int sensorValue_CO2 = 0;
int sensorValue_O2 = 0;
float co2PPM = 0.0;
float o2Percentage = 0.0;

// Create a WiFiClient object to connect to the WiFi network
WiFiClient client;

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Connect to WiFi network
  connectToWiFi();

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Read CO2 sensor value
  sensorValue_CO2 = analogRead(sensorPin_CO2);
  // Calculate CO2 ppm
  co2PPM = 400.0 + ((sensorValue_CO2 * (10000.0 - 400.0)) / 4095.0);

  // Read O2 sensor value
  sensorValue_O2 = analogRead(sensorPin_O2);
  // Calculate O2 percentage
  o2Percentage = (sensorValue_O2 * 25.0) / 4095.0;

  // Print sensor data to Serial monitor
  Serial.print("CO2 - ");
  Serial.print("Sensor value: ");
  Serial.print(sensorValue_CO2);
  Serial.print(", CO2 ppm: ");
  Serial.println(co2PPM);

  Serial.print("O2 - ");
  Serial.print("Sensor value: ");
  Serial.print(sensorValue_O2);
  Serial.print(", O2 percentage: ");
  Serial.print(o2Percentage);
  Serial.println("%");

  // Upload CO2 and O2 data to ThingSpeak
  uploadToThingSpeak(co2PPM, o2Percentage);

  // Delay before next reading and upload
  delay(60000); // 60000 ms = 1 min
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - IP address: ");
  Serial.println(WiFi.localIP());
}

void uploadToThingSpeak(float co2, float o2) {
  // Set fields with sensor data
  ThingSpeak.setField(1, co2);
  ThingSpeak.setField(3, o2);

  // Write to ThingSpeak
  int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  // Check if successful
  if (httpCode == 200) {
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.print("Error sending data to ThingSpeak: ");
    Serial.println(httpCode);
  }
}
