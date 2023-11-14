const byte InsertCoinGoSignal = 13;
bool Check_InsertCoinGoSignal = false;

#include <Servo.h>
Servo servo[4];
const byte servoCoinSlot = 11;
const byte servoEntrance = 10;
const byte servoGoBottom = 9;
const byte servoGoLeft   = 8;
int angle = 0;

const byte IndSensor = A0;
const byte CapSensor = A1;
int Check_IndSensor;
int Check_CapSensor;

const byte trig = 7; 
const byte echo = A2;
int duration, cm;

const byte SignalToESPcam = 2;
const byte ShapeLikeBottle = 3;
const byte NotBottle = 4;
bool Check_ShapeLikeBottle;
bool Check_NotBottle;

void Servos(byte location);
void Angle(byte loc, int sec);
bool BottleStatus();
bool BottleShapeStatus();
long microsecondsToCentimeters(long microseconds);

void setup() {
  Serial.begin(9600);
  pinMode(InsertCoinGoSignal, INPUT);
  servo[0].attach(servoEntrance);
  servo[1].attach(servoGoBottom);
  servo[2].attach(servoGoLeft);
  servo[3].attach(servoCoinSlot);
  pinMode(IndSensor, INPUT);
  pinMode(CapSensor, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  
  pinMode(SignalToESPcam,  OUTPUT);
  pinMode(ShapeLikeBottle, INPUT);
  pinMode(NotBottle,       INPUT);
  digitalWrite(SignalToESPcam, 0);
  
  Serial.println("Setting up...");
  delay(2000);
  Serial.println("Please wait for seconds...");
  delay(2000);
  Serial.println("The machine is now ready!");
  Serial.println();
  delay(1000);
}

void loop() {
  Check_InsertCoinGoSignal = digitalRead(InsertCoinGoSignal);
  
  if (Check_InsertCoinGoSignal) {
    digitalWrite(trig, LOW);
  	delayMicroseconds(2);
  	digitalWrite(trig, HIGH);
  	delayMicroseconds(10);
  	digitalWrite(trig, LOW);
  	duration = pulseIn(echo, HIGH);
    cm = microsecondsToCentimeters(duration);
    Serial.print("Object_Distance: ");
    Serial.print(cm);
    Serial.println(" cm");
    
    if(cm < 150) {
      if (BottleStatus() == false) {
        Servos(servoGoBottom);
      } else {
        digitalWrite(SignalToESPcam, 1);
        delay(3000);
        if (BottleShapeStatus() == false) {
          Servos(servoGoBottom);
        } else {
          Servos(servoGoLeft);
          Servos(servoCoinSlot);
        }
        digitalWrite(SignalToESPcam, 0);
      }
    } else {
      Serial.println("No object found.");
    }
  } else {
    Serial.println("Waiting for insert botte signal.");
  }
  Serial.println();
}

void Servos(byte location) {
  switch (location) {
    case servoCoinSlot:
      Angle(3, 0);
      break;
    case servoGoLeft:
      Angle(2, 500);
      break;
    case servoGoBottom:
      Angle(1, 500);
      break;
    case servoEntrance:
      Angle(0, 0);
      break;
  }
}

void Angle(byte loc, int sec) {
  delay(sec);
  
  for(angle = 0; angle < 180; angle++) {
    servo[loc].write(angle);
    delay(10);
  }
  
  for(angle = 180; angle > 0; angle--) {
    servo[loc].write(angle);
    delay(10);
  }
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

/*** Check if it is a plastic and if it has water. ***/
bool BottleStatus() {
  byte countDown = 3;
  bool bStatus;

  while (countDown != 0) {
  	Check_CapSensor = analogRead(CapSensor);
  	Check_IndSensor = analogRead(IndSensor);
  	delay(500);
    countDown--;
  }
  
  Serial.print("IndSensor: ");
  Serial.print(Check_IndSensor);
  Serial.print("  &&  CapSensor: ");
  Serial.println(Check_CapSensor);
  
  if (Check_CapSensor < 25 && Check_IndSensor > 25) {
    bStatus = true;
  } else {
    bStatus = false;
    Serial.println("Bottle has water or it's a metal.");
  }
  
  Servos(servoEntrance);
  return bStatus;
}

bool BottleShapeStatus() {
  Check_ShapeLikeBottle = digitalRead(ShapeLikeBottle);
  Check_NotBottle = digitalRead(NotBottle);
  Serial.print("ShapeLikeBottle: ");
  Serial.print(Check_ShapeLikeBottle);
  Serial.print("  &&  NotBottle: ");
  Serial.println(Check_NotBottle);
  
  if (Check_ShapeLikeBottle && !Check_NotBottle) {
    return true;
  } else {
    return false;
  }
}