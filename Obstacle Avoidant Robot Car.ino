//libraries
#include <IRremote.h>

//VARIABLES
 //IR Remote
#define IR 12

  //Buzzer
#define BUZZER 11  

 // Ultrasonic Sensor
#define TRIG A1
#define ECHO A0

 // RGB LED
#define RED 8
#define GREEN 9
#define BLUE 10

 // Motor Driver (L298N)
#define MOTOR_IN1 A2      // direction control motor 1
#define MOTOR_IN2 A3      // direction control motor 1
#define MOTOR_IN3 A4      // direction control motor 2
#define MOTOR_IN4 A5      // direction control motor 2
#define MOTOR_ENA 5      // speed control (PWM pin) motor 1
#define MOTOR_ENB 6      // speed control (PWM pin) motor 2

 // System State
bool systemActive = false; // tracks if remote turned it on or off

 // Motor Speeds
int speedFull = 150;      
int speedSlow = 90;      

void setup() {
  // RGB
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  // initial state = red (system off)
  setRGB(255, 0, 0);
  
  //Motor Driver 
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);  // PWM speed pin
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  pinMode(MOTOR_ENB, OUTPUT);

  // Ultrasonic Sensor
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Buzzer
  pinMode(BUZZER, OUTPUT);

  // IR Receiver - needs its own begin() function
  IrReceiver.begin(IR, ENABLE_LED_FEEDBACK);

  // Serial monitor
  Serial.begin(9600);

}

void loop() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.decodedRawData != 0xFFFFFFFF) {
      if (IrReceiver.decodedIRData.decodedRawData == 0xBF40FF00) {
          systemActive = !systemActive;
      }
    }
    IrReceiver.resume();
  }

  if (systemActive) {

    //Sensor reading
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(5);
    digitalWrite(TRIG, LOW);

    int duration = pulseIn(ECHO, HIGH);
    int distance = duration / 29 / 2;

    // ignore bad readings
    if (distance == 0 || distance > 400) {
    return; 
    }

    // print distance to serial monitor for debugging
    Serial.println(distance);

    //Distance conditions
    if (distance < 20) {
      setRGB(255, 0, 0); 
      motorStop();
      buzzFast();
    } else if (distance < 50) {
      setRGB(255, 255, 0); 
      motorSlow();
      buzzSlow();
    } else {
      setRGB(0, 255, 0);
      motorFull();
      buzzOff();
    }
    delay(50);
    
  } else {
    // System off 
    setRGB(255, 0, 0);
    motorStop();
    buzzOff();
  }                                      
}

//Define Functions 

  //Motor functions

void motorStop() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_ENA, 0); 
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);
  analogWrite(MOTOR_ENB, 0); 
}

void motorFull() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  analogWrite(MOTOR_ENA, speedFull); 
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, HIGH);
  analogWrite(MOTOR_ENB, speedFull); 
}

void motorSlow() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  analogWrite(MOTOR_ENA, speedSlow);
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, HIGH);
  analogWrite(MOTOR_ENB, speedSlow); 
}

  //RGB Function

void setRGB(int r, int g, int b) {
  analogWrite(RED, r);
  analogWrite(GREEN, g);
  analogWrite(BLUE, b);
}

  //Buzzer functions

void buzzSlow() {
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  delay(500);
}

void buzzFast() {
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
  delay(100);
}

void buzzOff() {
  digitalWrite(BUZZER, LOW);
}
