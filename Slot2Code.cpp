#include <LiquidCrystal_I2C.h>

// Pin definitions
const int trigPin = 7;       
const int echoPin = 6;       
const int buttonPin = 5;     
const int bulbPin = 10;
const int tempSensorPin = A0;
const int rgbForTempRedPin = 13;     
const int rgbForTempGreenPin = 12;   
const int buzzerForTempPin = 11;
const int gasSensorPin = A2;
const int rgbForGasRedPin = 4;
const int rgbForGasGreenPin = 3;
const int buzzerForGasPin = 2;
const int alertPin = 9;
const int statusPin = 8;

// Constants
const int carDetectedRange = 10; // Range in cm to detect cars
const float higherTemperature = 45.0; // Temperature threshold in °C for TMP36
const int gasDetectedRange = 300;

// I2C LCD setup
LiquidCrystal_I2C lcd(0x20, 16, 2); // Address 0x20, 16 columns, 2 rows

bool faultDetected = false; // Track fault state

void setup() {
  // Initialize pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(bulbPin, OUTPUT);
  pinMode(rgbForTempRedPin, OUTPUT);
  pinMode(rgbForTempGreenPin, OUTPUT);
  pinMode(buzzerForTempPin, OUTPUT);
  pinMode(rgbForGasRedPin, OUTPUT);
  pinMode(rgbForGasGreenPin, OUTPUT);
  pinMode(buzzerForGasPin, OUTPUT);
  pinMode(alertPin, OUTPUT);
  pinMode(statusPin, OUTPUT);

  // Initialize I2C LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CarChargingStn");
  lcd.setCursor(0, 1);
  lcd.print("Happy2Serve");
  delay(500);

  // Set initial alert state
  digitalWrite(alertPin, LOW);
  digitalWrite(statusPin, HIGH);

  // Start Serial Monitor for debugging
  Serial.begin(9600);
}

void loop() {
  // Read distance from ultrasonic sensor
  int distance = getDistance(trigPin, echoPin);

  // Read the button state
  bool buttonPressed = (digitalRead(buttonPin) == LOW);

  // Read gas sensor value
  int gasLevel = analogRead(gasSensorPin);

  // Read temperature from TMP36
  float temperature = getTemperature(tempSensorPin);

  // Debugging output
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Button Pressed: ");
  Serial.print(buttonPressed);
  Serial.print(", Gas Level: ");
  Serial.print(gasLevel);
  Serial.print(", Temperature: ");
  Serial.println(temperature);

  // Check for car detection and button press
  if ((distance > 0) && (distance <= carDetectedRange) && buttonPressed) {
    // Reset fault state if no fault is detected
    digitalWrite(statusPin, LOW);
    if (!faultDetected) {
      digitalWrite(bulbPin, HIGH);
      lcd.clear();
      lcd.print("Parking: Slot 2");
      lcd.setCursor(0, 1);
      lcd.print("Booked");
      digitalWrite(rgbForGasGreenPin, HIGH);
      digitalWrite(rgbForTempGreenPin, HIGH);
    }
    
    // Check for faults
    if ((gasLevel > gasDetectedRange) || (temperature >= higherTemperature)) {
      faultDetected = true; // Set fault flag
      digitalWrite(bulbPin, LOW); // Turn off the bulb
      digitalWrite(alertPin, HIGH); // Send alert signal
      
      if ((gasLevel>gasDetectedRange) && (temperature >= higherTemperature)){
        digitalWrite(buzzerForTempPin, HIGH);
        digitalWrite(buzzerForGasPin, HIGH);
        digitalWrite(rgbForTempGreenPin, LOW);
        digitalWrite(rgbForTempRedPin, HIGH);
        digitalWrite(rgbForGasGreenPin, LOW);
        digitalWrite(rgbForGasRedPin, HIGH);
        lcd.clear();
        lcd.print("Parking: Slot 2");
        lcd.setCursor(0,1);
        lcd.print("Fault in Slot 2");
        digitalWrite(alertPin, HIGH);
      }else if (gasLevel > gasDetectedRange) {
        // Handle gas fault
        digitalWrite(buzzerForGasPin, HIGH);
        digitalWrite(buzzerForTempPin, LOW);
        digitalWrite(rgbForGasGreenPin, LOW);
        digitalWrite(rgbForGasRedPin, HIGH);
        digitalWrite(rgbForTempGreenPin, HIGH);
        digitalWrite(rgbForTempRedPin, LOW);
        lcd.clear();
        lcd.print("Parking: Slot 2");
        lcd.setCursor(0, 1);
        lcd.print("Gas Alert");
        digitalWrite(alertPin, HIGH);
      }else if (temperature >= higherTemperature) {
        // Handle temperature fault
        digitalWrite(buzzerForTempPin, HIGH);
        digitalWrite(rgbForTempGreenPin, LOW);
        digitalWrite(rgbForTempRedPin, HIGH);
        digitalWrite(buzzerForGasPin, LOW);
        digitalWrite(rgbForGasGreenPin, HIGH);
        digitalWrite(rgbForGasRedPin, LOW);
        lcd.clear();
        lcd.print("Parking: Slot 2");
        lcd.setCursor(0, 1);
        lcd.print("Temp Alert");
        digitalWrite(alertPin, HIGH);
      }
      
    } else {
      faultDetected = false; // Clear fault flag
      digitalWrite(alertPin, LOW); // Clear alert signal
      digitalWrite(buzzerForGasPin, LOW);
      digitalWrite(buzzerForTempPin, LOW);
      digitalWrite(rgbForGasRedPin, LOW);
      digitalWrite(rgbForGasGreenPin,HIGH);
      digitalWrite(rgbForTempRedPin, LOW);
      digitalWrite(rgbForTempGreenPin, HIGH);
    }
  } else {
    // No car detected or button not pressed
    digitalWrite(bulbPin, LOW);
    digitalWrite(alertPin, LOW); // Clear alert signal
    digitalWrite(statusPin, HIGH);
    faultDetected = false; // Reset fault state
    lcd.clear();
    lcd.print("Parking: Slot 2");
    lcd.setCursor(0, 1);
    lcd.print("Available");

    // Reset indicators
    digitalWrite(rgbForTempRedPin, LOW);
    digitalWrite(rgbForTempGreenPin, LOW);
    digitalWrite(rgbForGasRedPin, LOW);
    digitalWrite(rgbForGasGreenPin, LOW);
    digitalWrite(buzzerForGasPin, LOW);
    digitalWrite(buzzerForTempPin, LOW);
  }

  delay(200); // Small delay for stability
}

// Function to calculate distance from an ultrasonic sensor
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

// Function to calculate temperature from TMP36
float getTemperature(int pin) {
  int reading = analogRead(pin);
  float voltage = reading * 5.0 / 1023.0; // Convert to voltage
  float temperature = (voltage - 0.5) * 100.0; // Convert to °C
  return temperature;
}
