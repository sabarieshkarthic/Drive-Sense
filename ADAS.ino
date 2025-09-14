#include <IRremote.h>
#include "TimerOne.h"

// Ultrasonic Sensor Pins
int trigPin = 11;    // Trigger pin for HC-SR04
int echoPin = 12;    // Echo pin for HC-SR04
long duration;        // Duration of the echo pulse
float cm1, cm2;      // Distances measured in centimeters
float sonarSpeed;    // Speed in cm/min from ultrasonic sensor

// Wheel Revolution Variables
volatile unsigned long lastTime = 0; // Last time a revolution was detected
volatile unsigned long revolutionTime = 0; // Time taken for the last revolution
volatile unsigned int counter = 0; // Counter for revolutions
const int sensorPin = 2; // Pin connected to LM393 sensor output
float wheelSpeed = 0; // Speed in cm/min from LM393

// IR Remote Variables
const int recv_pin = 3; // Pin connected to the IR receiver
IRrecv ir(recv_pin);
// Motor control variables
int motsp=0;
int motor1pin1 = 4;
int motor1pin2 = 7;
int motor2pin1 = 8;
int motor2pin2 = 13;

void setup() {
    Serial.begin(9600); // Start serial communication

    // Setup for Ultrasonic Sensor
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    pinMode(motor1pin1, OUTPUT);
    pinMode(motor1pin2, OUTPUT);
    pinMode(motor2pin1,   OUTPUT);
    pinMode(motor2pin2, OUTPUT);
    pinMode(5, OUTPUT); 
    pinMode(6, OUTPUT);

    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);

    // Setup for Wheel Revolution Sensor
    pinMode(sensorPin, INPUT); // Set sensor pin as input
    attachInterrupt(digitalPinToInterrupt(sensorPin), countRevolutions, RISING); // Attach interrupt to sensor pin

    // Setup for IR Receiver
    ir.enableIRIn(); // Start the receiver
}

void loop() {
    // Measure distance using HC-SR04
    measureDistance();

    Serial.print("Wheel Speed: ");
    Serial.print(wheelSpeed);

    // Print speeds and compare them if both are available
    Serial.print(" Comparing Speeds: ");
    //idika pora condition
    if ((wheelSpeed > sonarSpeed)&&(motsp!=0)) {
        Serial.println("Wheel is faster than Sonar.");
          motsp-=20;
          analogWrite(5, motsp);
          analogWrite(6, motsp);
    } else if ((wheelSpeed <= sonarSpeed)&&(mptsp!=0)) {
        Serial.println("Sonar is faster than Wheel.");
        motsp=0;
        analogWrite(5, motsp);
        analogWrite(6, motsp);
    } else if(sonarSpeed<0)&&(mots){
      if(motsp%85!=0)
        motsp+=20;
        analogWrite(5, motsp);
        analogWrite(6, motsp);
    }

    // Check for incoming IR signals
if (ir.decode()) {
        Serial.print("IR Code: ");
        Serial.println(ir.decodedIRData.decodedRawData, HEX); // Print the received code

            switch (ir.decodedIRData.decodedRawData) {
                case 0xF9067F80: // Example button code for speed increase
                    Serial.println("Speed increase");
                    if(motsp < 255) {
                        motsp += 85;
                    }
                    analogWrite(5, motsp);
                    analogWrite(6, motsp);
                    delay(1000);
                    break;
                case 0xFA057F80: // Example button code for speed decrease
                    Serial.println("Speed decrease");
                    if(motsp > 0) {
                        motsp -= 85;
                    }
                    analogWrite(5, motsp);
                    analogWrite(6, motsp);
                    delay(1000);
                    break;
                case 0xFFE21D: // Example button code for another action
                    Serial.println("Button C pressed");
                    break;
                default:
                    Serial.println("Unknown button pressed");
                    break;
            }
        ir.resume();  // Receive the next value
    }
}

// Function to measure distance using HC-SR04
void measureDistance() {
a:
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH); // Read echo pulse duration
    cm1 = (duration / 2) / 29.1; // Convert to cm

    delay(99); // Delay for stability before next measurement

    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH); // Read second echo pulse duration
    cm2 = (duration / 2) / 29.1; // Convert to cm

    if(cm1 > 105 && cm2 > 105) {
        goto a;
    } else {
        if(cm2 < (cm1 - 1.5) || (cm1 + 1.5) < cm2) {
            sonarSpeed = ((cm1 - cm2) / 99.015) * 60000; 
        } else {
            sonarSpeed = 0;
        }
    }

    Serial.print("Sonar Distance1: ");
    Serial.print(cm1);
    Serial.print(" cm, Distance2: ");
    Serial.print(cm2);
    Serial.print(" cm, Sonar Speed: ");
    Serial.print(sonarSpeed);
    Serial.println(" cm/min");
}

// Function to count revolutions and measure time
void countRevolutions() {
    unsigned long currentTime = millis(); // Get current time in milliseconds

    if (counter > 0) { // If this is not the first revolution
        revolutionTime = currentTime - lastTime; // Calculate time for this revolution
        
        if(revolutionTime >= 500) {
            wheelSpeed = (22.2 / revolutionTime) * 60000;
        }
        
        Serial.print("Time for last revolution: ");
        Serial.print(revolutionTime);
        Serial.println(" ms"); 
        
        Serial.print("Wheel Speed: ");
        Serial.print(wheelSpeed);
        Serial.println(" cm/min"); 
    }

    lastTime = currentTime; 
    counter++; 
}