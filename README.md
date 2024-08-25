# Agrotech-Final Project- “Cutting-Edge Closed-Environment System for Monitoring Microbial Metabolism and Viability”
## Project Goal
Our goal is to track metabolism indicators by the use of sensors to assess microbial viability within a closed system over time, where a peristaltic pump that refreshes the air once CO2 concentration reaches a threshold.
### System Concept
The system is equipped with a range of sensors: CO2, O2, temperature, humidity, and light intensity to track environmental conditions. Data from these sensors is transmitted to our [Thingspeak channel](https://thingspeak.com/channels/2595959 thingspeak channel). When CO2 levels reach a predefined threshold (1000 ppm, 400-10000 ppm range), The system automatically activates two peristaltic pumps: one to introduce fresh air into the closed environment and another to expel used air, ensuring optimal conditions for precise microbial viability assessment.
### Components:

Electronics:

* x1 firebeetle ESP32 microcontroller 
* x1 Relays 
* x2 Peristaltic Pump 
* Multiple Cables
* x1 12V Power Source
* x1 CO2 Sensor
* x1 O2 Sensor
* x1 LDR (Light dependent resistor)
* x1 נגד
* x1 SHT31 Sensor
* x2 Potentiometers

Hardware:

* x1 Plastic box
* x1 גומי ךכבלים

Disclaimers:
* We were able to only monitor, Light, humidity and temperature. CO2 and O2 sensors were simulated using Potentiometers and code that set out to “fake” CO2 and O2 data.

# Construction:

## Prototype
The initial setup involved connecting the sensors to the ESP32 microcontroller and testing the basic functionality. and the relays were connected to control the peristaltic pumps. The system was assembled on a breadboard to validate the design before integrating it into a more permanent solution.

In addition The system as it stands lacks the integration of CO2 and O2 sesnors, in order to deal with that we used 2 potentiometers that are programmed to feed “fake” CO2 and O2 data so that we have a way to control the pumps and something to show in the presentation.
חיישני אור וטמפרטורה 
בסוך העלאה לthingpeakd

## Sketch
  * Red - VCC
  * Blue - GND for 12V components or analog read for sensors
  * Black - GND
  * Orange - VCC for sensors and analog pins for relays (also yellow)
  * Brown - 12V power supply or GND for relays and sensors

# Code

#include <WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>      // Include the Wire library for I2C communication
#include "Adafruit_SHT31.h" // Include the SHT31 library from Adafruit


// WiFi network credentials
const char* ssid = "Gabriella";
const char* password = "38383838";


// ThingSpeak channel and API key
unsigned long myChannelNumber = 2595959; // Replace with your channel number
const char* myWriteAPIKey = "YIUFQHOWQVZN72JP";


// Define analog sensor pins
int sensorPin_CO2 = A0;   // CO2 sensor pin
int sensorPin_O2 = A1;    // O2 sensor pin
int sensorPin_LDR = A3;   // Light sensor pin (LDR)
int RelayPin = A4;        // Relay pin for controlling the pump


// Variables for sensor readings and calculated values
int sensorValue_CO2 = 0;
int sensorValue_O2 = 0;
float co2PPM = 0.0;
float o2Percentage = 0.0;
int sensorValue_LDR = 0;
float temperature = 0.0;
float humidity = 0.0;


// Create a WiFiClient object to connect to the WiFi network
WiFiClient client;


// Create an instance of the SHT31 object
Adafruit_SHT31 sht31 = Adafruit_SHT31();


void setup() {
 // Start serial communication
 Serial.begin(9600);


 // Set sensor pins and relay pin modes
 pinMode(sensorPin_CO2, INPUT);
 pinMode(sensorPin_O2, INPUT);
 pinMode(sensorPin_LDR, INPUT);
 pinMode(RelayPin, OUTPUT);
 digitalWrite(RelayPin, LOW); // Ensure pump is off initially


 // Connect to WiFi network
 connectToWiFi();


 // Initialize ThingSpeak
 ThingSpeak.begin(client);


 // Initialize SHT31 sensor
 if (!sht31.begin(0x44)) {   // Check if SHT31 is connected and start it with address 0x44
   Serial.println("Couldn't find SHT31");
   while (1) delay(1);  // Wait indefinitely
 }
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


 // Read light sensor value
 sensorValue_LDR = analogRead(sensorPin_LDR);


 // Read temperature and humidity from SHT31
 temperature = sht31.readTemperature();
 humidity = sht31.readHumidity();


 // Print sensor data to Serial monitor
 Serial.print("CO2 - Sensor value: ");
 Serial.print(sensorValue_CO2);
 Serial.print(", CO2 ppm: ");
 Serial.println(co2PPM);


 Serial.print("O2 - Sensor value: ");
 Serial.print(sensorValue_O2);
 Serial.print(", O2 percentage: ");
 Serial.print(o2Percentage);
 Serial.println("%");


 Serial.print("LDR Value: ");
 Serial.println(sensorValue_LDR);


 if (!isnan(temperature)) {
   Serial.print("Temperature (°C): ");
   Serial.println(temperature);
 } else {
   Serial.println("Failed to read temperature");
 }


 if (!isnan(humidity)) {
   Serial.print("Humidity (%): ");
   Serial.println(humidity);
 } else {
   Serial.println("Failed to read humidity");
 }


 // Control relay for peristaltic pump based on CO2 ppm
 if (co2PPM < 1000) {
   digitalWrite(RelayPin, HIGH); // Turn on the relay (start pump)
   Serial.println("Pump ON");
 } else {
   digitalWrite(RelayPin, LOW); // Turn off the relay (stop pump)
   Serial.println("Pump OFF");
 }


 // Upload sensor data to ThingSpeak
 uploadToThingSpeak(co2PPM, o2Percentage, sensorValue_LDR, humidity, temperature);


 // Delay before the next loop iteration
 delay(1000); // 1000 ms (time for reading)
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


void uploadToThingSpeak(float co2, float o2, int ldr, float humidity, float temperature) {
 // Set fields with sensor data
 ThingSpeak.setField(1, co2);
 ThingSpeak.setField(2, ldr);  // Light sensor data in field 2
 ThingSpeak.setField(3, o2);
 ThingSpeak.setField(4, humidity);  // Humidity data in field 4
 ThingSpeak.setField(5, temperature);  // Temperature data in field 5


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
“

# The final outcome

Picture from the Thingspeak:

## Data analysis
