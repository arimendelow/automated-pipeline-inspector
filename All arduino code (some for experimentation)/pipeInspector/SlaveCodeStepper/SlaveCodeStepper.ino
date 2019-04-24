/*This is the code for the Stepper motor.

  This board is a slave reciever. This is done through serial ports.
  Connect a wire from port 0 of the master to port 1 of this board.
  Connect a wire from port 1 of the master to port 0 of this board.
  NOTE: When uploading codes to Arduino, disconnect these ports.

*/
#include <Stepper.h>
const int stepsPerRevolution = 200; // change this to fit the number of steps per revolution
// for your motor

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

  // initialize the serial port:
  Serial.begin(9600);
  // set the motor speed (for multiple steps only):
  myStepper.setSpeed(20);
  Serial.flush();
}


void loop() {
  
//  
//        myStepper.step(-275); // 275 steps rotates the clamp the necessary 90 degrees (can vary)
//      delay(1000);          // waits one second to take picture (can vary)
//      myStepper.step(+275);
//      delay(1000);
  
  if (Serial.available()) { // looks for serial from master
    delay(200);
    int x = Serial.read();  // defines the input from masters
    if (x > -1) {           // this is the input from master that will trigger the stepper motor once
      myStepper.step(-275); // 275 steps rotates the clamp the necessary 90 degrees (can vary)
      delay(1000);          // waits one second to take picture (can vary)
      myStepper.step(+275);
      delay(1000);
    }
  }
}
