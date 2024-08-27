// Define the light sensor pin
#define LIGHT_SENSOR_PIN A0  // A0 pin for the light sensor (analog input)

// Variable to store the value read from the analog pin
int lightSensorValue;

void setup() {
  Serial.begin(9600);  // Initialize serial communication at 9600 baud
}

void loop() {
  lightSensorValue = analogRead(LIGHT_SENSOR_PIN);  // Read the LDR value from the A0 pin
  Serial.print("LDR Value: ");  // Print the LDR value label
  Serial.println(lightSensorValue);  // Print the LDR value to the Serial Monitor
  delay(500);  // Wait for 500 milliseconds before the next reading
}
