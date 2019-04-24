  //increase rosserial buffer size: https://answers.ros.org/question/73627/how-to-increase-rosserial-buffer-size/



#include <ros.h>
#include <sensor_msgs/Joy.h> //so we can listen to the joy topic
#include <Wire.h>

ros::NodeHandle_<ArduinoHardware, 2, 2, 150, 150> nh;


//pipe inspection stuff
// Sensors
const int LinTrig = 2;    //Actuator Sensor
const int LinEcho = 3;    //Actuator Sensor

// Linear Actuator
int LinearOut = 11;        //These pins are designated from the motor shield.
int LinearSpeed = 9;      //DO NOT ASSIGN ANYTHING ELSE TO THESE PINS!
int LinearIn = 8;        // Wiring for actuator should be OUT1: Red. OUT2: Black.

int armState = 0; //0 = retracted, 1 = extended);

int inspectingPipe = 0;

long duration, inches;



////
  //functions for pipe inspection
////



void inspect() {
  Wire.beginTransmission(9);
  Wire.write(1);
  Wire.endTransmission();
}

void Sensor1() {   //detects supports with ultrasonic sensor

  digitalWrite(LinTrig, LOW);
  delayMicroseconds(5);
  digitalWrite(LinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(LinTrig, LOW);
  duration = pulseIn(LinEcho, HIGH);
  inches = (duration / 2) / 74;
  delay(100);
  
}


void Extend() {              //extends arm
  
  if(armState == 0) {   //if arm is not yet extended
    digitalWrite(LinearIn, LOW);
    digitalWrite(LinearOut, HIGH);
    analogWrite(LinearSpeed, 255);
    delayWithSpin(20);
    armState = 1;
  }

  
}

void Retract() {             //retracts arm

  if(armState == 1) {   //if arm is currently extended
    digitalWrite(LinearIn, HIGH);
    digitalWrite(LinearOut, LOW);
    analogWrite(LinearSpeed, 255);
    delayWithSpin(20);
    
    armState = 0;
  }

  
}

//need to run nh.spinOnce fairly often, so can't use normal delay
void delayWithSpin(int t) {
  
  for(int i = 0; i < t; i++){
    nh.spinOnce();
    delay(1000);
  }  
}

void StopArm() {             //stops arm

  analogWrite(LinearSpeed, 0);
  
}

void joyCB( const sensor_msgs::Joy& joy){
  

  if((joy.buttons[4]) == 1) { //replace this axis value with the button that we want

    Extend(); //extend arm
    
    inspectingPipe = 1;
    
  } else if((joy.buttons[5]) == 1) {

      Retract(); //retract arm
      
      inspectingPipe = 0;
    
  }
  
}




ros::Subscriber<sensor_msgs::Joy> subJoy("/joy", joyCB ); //create /joy subscriber subJoy


void setup() {
  
////
  //for driving
////
  
  
  nh.initNode();
  nh.subscribe(subJoy);

////
  //for pipe inspection
////

  Wire.begin();

  pinMode(LinearOut, OUTPUT); //Shield
  pinMode(LinearIn, OUTPUT); //Shield
  pinMode(LinearSpeed, OUTPUT); //Shield
  pinMode(LinTrig, OUTPUT);
  pinMode(LinEcho, INPUT);


}

void loop() {
  
  nh.spinOnce();
  
}








