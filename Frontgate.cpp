#include <Servo.h>
#include <LiquidCrystal_I2C.h>
const int trigPin1 = 5;  
const int echoPin1 = 6;  
const int trigPin2 = 3;  
const int echoPin2 = 4;  
const int leftServoPin = 13;  
const int rightServoPin = 12;  
const int parkingSlot1Input = 11;  
const int parkingSlot2Input = 9;  
const int slot1StatusInput = 8;
const int slot2StatusInput = 10;
const int detectionRange = 20; 
const int gateOpenAngleRight = 90;  
const int gateCloseAngleRight = 0;  
const int gateOpenAngleLeft = 90;   
const int gateCloseAngleLeft = 0;   
LiquidCrystal_I2C lcd(0x20, 16, 2);
Servo rightGateServo;
Servo leftGateServo;
void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(parkingSlot1Input, INPUT);
  pinMode(parkingSlot2Input, INPUT);
  pinMode(slot1StatusInput, INPUT);
  pinMode(slot2StatusInput, INPUT);
  rightGateServo.attach(rightServoPin);
  leftGateServo.attach(leftServoPin);
  rightGateServo.write(gateCloseAngleRight); 
  leftGateServo.write(gateCloseAngleLeft);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Welcome To");
  lcd.setCursor(0,1);
  lcd.print("Ev Park"); 
  Serial.begin(9600);
}
void loop() {
  int slot1Fault = digitalRead(parkingSlot1Input);
  int slot2Fault = digitalRead(parkingSlot2Input);
  int slot1Status = digitalRead(slot1StatusInput);
  int slot2Status = digitalRead(slot2StatusInput);
  int distance1 = getDistance(trigPin1, echoPin1);
  int distance2 = getDistance(trigPin2, echoPin2);
  Serial.print("Distance1: ");
  Serial.print(distance1);
  Serial.print(" cm, Distance2: ");
  Serial.print(distance2);
  Serial.print(", EmergencySlot1: ");
  Serial.print(slot1Fault);
  Serial.print(", EmergencySlot2: ");
  Serial.println(slot2Fault);
  if (slot1Fault || slot2Fault) {
    Serial.println("Gate not open due to fault!");
    rightGateServo.write(gateCloseAngleRight);
    leftGateServo.write(gateCloseAngleLeft);
    if(slot1Fault && slot2Fault){
      rightGateServo.write(gateCloseAngleRight);
      leftGateServo.write(gateCloseAngleLeft);
      lcd.clear();
      lcd.print("BothGateIsClosed");
      lcd.setCursor(0,1);
      lcd.print("FaultInBothSlot");
      delay(500);
      lcd.clear();
      lcd.print("SorryForYourTime");
      lcd.clear();
    }
    else if(slot1Fault){
      rightGateServo.write(gateCloseAngleRight);
      lcd.clear();
      lcd.print("RightGateIsClose");
      lcd.setCursor(0,1);
      lcd.print("Fault in Slot1");
      if(distance1>0 && distance1<= detectionRange && slot2Status){
        leftGateServo.write(gateOpenAngleLeft);
        lcd.clear();
        lcd.print("Slot2 Available");
        lcd.setCursor(0,1);
        lcd.print("LeftGateisOpen");
      }else if (distance2 > detectionRange && slot2Status){
        leftGateServo.write(gateCloseAngleLeft);

      }
    }else if(slot2Fault){
      leftGateServo.write(gateCloseAngleLeft);
      lcd.clear();
      lcd.print("LeftGateIsClose");
      lcd.setCursor(0,1);
      lcd.print("Fault in Slot2");
      if(distance1>0 && distance1<= detectionRange && slot1Status){
       rightGateServo.write(gateOpenAngleRight);
        lcd.clear();
        lcd.print("Slot1 Available");
        lcd.setCursor(0,1);
        lcd.print("RightGateisOpen");
      }else if (distance2 > detectionRange && slot1Status){
        rightGateServo.write(gateCloseAngleRight);
      }
    }
  } else {
    lcd.clear();
    lcd.print("NormalSituation");
    // Normal gate logic
    if (distance1 > 0 && distance1 <= detectionRange && (slot1Status || slot2Status)) {
      // Open gates if vehicle detected at entrance
      rightGateServo.write(gateOpenAngleRight);
      leftGateServo.write(gateOpenAngleLeft);

    } else if (distance2 > detectionRange || distance2 == 0  ) 
      rightGateServo.write(gateCloseAngleRight);
      leftGateServo.write(gateCloseAngleLeft);
    }
  }

  delay(100); // Small delay for stability
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
