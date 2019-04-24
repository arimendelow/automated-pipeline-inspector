/*This is the code for the Stepper motor.

  This board is a slave reciever. This is done through I2C.

*/
#include <Stepper.h>
#include <Wire.h>

const int stepsPerRevolution = 200; // change this to fit the number of steps per revolution
// for your motor

int x = 0;

// initialize the stepper library on the motor shield
Stepper myStepper(stepsPerRevolution, 12, 13);

// give the motor control pins names:
const int pwmA = 3;
const int pwmB = 11;
const int brakeA = 9;
const int brakeB = 8;
const int dirA = 12;
const int dirB = 13;

void setup() {
  // set the PWM and brake pins so that the direction pins  // can be used to control the motor:
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(brakeA, OUTPUT);
  pinMode(brakeB, OUTPUT);
  digitalWrite(pwmA, HIGH);
  digitalWrite(pwmB, HIGH);
  digitalWrite(brakeA, LOW);
  digitalWrite(brakeB, LOW);
  
  Serial.begin(9600);

  Wire.begin(9);
  Wire.onReceive(receiveEvent);

  // set the motor speed (for multiple steps only):
  myStepper.setSpeed(20);
}

void receiveEvent(int bytes) {
  x = Wire.read(); //read one char from I2C
}

void loop() {
  
  if (x == 1) { // looks for serial from master
  
      Serial.write('T');
      myStepper.step(-275); // 275 steps rotates the clamp the necessary 90 degrees (can vary)
      delay(500);
      Serial.write('T');
      delay(500);          // waits one second to take picture (can vary)
      myStepper.step(+275);
      delay(500);
      delay(500);          // waits one second to take picture (can vary)
      
      x = 0;
      
  }
}
