

#include <ros.h>
#include <geometry_msgs/Twist.h> //so we can subscribe to /cmd_vel
#include <sensor_msgs/Joy.h> //so we can listen to the joy topic
#include <Wire.h>

ros::NodeHandle_<ArduinoHardware, 2, 2, 150, 150> nh;

// Motor Driver
int DirRight = 6; //Wiring for the motors should be:
int PwmRight = 5; //M1A: Red - M1B: Black - M2A:Red - M2B:Black
int DirLeft = 7;
int PwmLeft = 10;
int pwm_value;

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


// function for controlling drive motors
// speed is how fast the motor rotates, in PWM3
void move_base(int speed, int pwmPin, int INaPin){
  if(speed > 30 && speed <= 255){
    analogWrite(pwmPin, speed);
    digitalWrite(INaPin, HIGH);
  }else if(speed < -30 && speed >= -255){
    analogWrite(pwmPin, (-1)*speed);
    digitalWrite(INaPin, LOW);
  }else if(speed > 255) {
    move_base(255, pwmPin, INaPin);
  }else if(speed < -255){
    move_base(-255, pwmPin, INaPin);
  }else{
    analogWrite(pwmPin, 0);
    digitalWrite(INaPin, HIGH);;
  }
}

////
  //functions for pipe inspection
////

void pipeInspectorMaster() {
  
  
  
  Sensor1(); //check if support
  
  //Serial.println("inches: " + inches);
  
  delay(200); //short enough that we dont need to use delayWithSpin

  if (inches <= 3) {   //if a pipe support is within 3 inches of sensor:
    
    //Serial.println("stopping robot...");
    
    //stop robot
    move_base(0, PwmRight, DirRight);
    move_base(0, PwmLeft, DirLeft);
    
    inspect(); //tell cameras to take one last read
    //Serial.println("one last check...");
    
    delayWithSpin(11);     //wait for scanning to finish
    Retract();       //retract the arm
    //Serial.println("retracting arm...");
    
    StopArm();       //stop arm
    delayWithSpin(1);
    
    //move forward
    
    //Serial.println("moving forward...");
    move_base(62, PwmRight, DirRight);
    move_base(62, PwmLeft, DirLeft);
    
    delayWithSpin(2);      //wait until past pipe
    
    //stop robot
    //Serial.println("stopping robot...");
    move_base(0, PwmRight, DirRight);
    move_base(0, PwmLeft, DirLeft);
    
    delayWithSpin(1);
    //Serial.println("extending arm...");
    Extend();        //extend arm
    
  } else {                  //or else
 
    //Serial.println("checking pipe...");
    inspect();    //tell arm to scan
    delayWithSpin(11);       //wait for scanto complete
    
    //move forward
    //Serial.println("moving forward...");
    move_base(62, PwmRight, DirRight);
    move_base(62, PwmLeft, DirLeft);
    
    delay(500);
    nh.spinOnce();
    
    //stop robot
    //Serial.println("stopping...");
    move_base(0, PwmRight, DirRight);
    move_base(0, PwmLeft, DirLeft);;
    
    delay(500);
    nh.spinOnce();
    
  }
  
  delay(100);
  nh.spinOnce();
  
}

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
  nh.spinOnce();
  
}


void Extend() {              //extends arm
  
  if(armState == 0) {   //if arm is not yet extended
    digitalWrite(LinearIn, LOW);
    digitalWrite(LinearOut, HIGH);
    analogWrite(LinearSpeed, 255);
    delayWithSpin(22);
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
  
  
  if((joy.buttons[5]) == 1) {
    Retract(); //retract arm if not yet retracted
    inspectingPipe = 0;
  } else if((joy.buttons[4]) == 1) {
    inspectingPipe = 1;
  }

  if(inspectingPipe == 1) {
    
    nh.spinOnce();
    Extend(); //extend arm if not yet extended
    
    pipeInspectorMaster();
    nh.spinOnce();
    
  }
  
}


void cmdVelCB( const geometry_msgs::Twist& cmd_vel){

  float x;
  float angular;
  int right;
  int left;
  
  x = cmd_vel.linear.x;
  angular = cmd_vel.angular.z;
  
  right = (int)((x + angular) * 50);
  left = (int)((x - angular) * 50);
  
  Serial.print(right); Serial.print(","); Serial.println(left);
  
  // control motors
  move_base(right, PwmRight, DirRight);
  move_base(left, PwmLeft, DirLeft);
  
}   


ros::Subscriber<geometry_msgs::Twist> subVel("/cmd_vel", cmdVelCB ); //create /cmd_vel subscriber subVel
ros::Subscriber<sensor_msgs::Joy> subJoy("/joy", joyCB ); //create /joy subscriber subJoy


void setup() {
  
////
  //for driving
////
  
  pinMode(DirRight, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(DirLeft, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  
  nh.initNode();
  nh.subscribe(subVel);
  nh.subscribe(subJoy);

////
  //for pipe inspection
////

  Wire.begin();

  pinMode(LinearOut, OUTPUT); //Shield
  pinMode(LinearIn, OUTPUT); //Shield
  pinMode(LinearSpeed, OUTPUT); //Shield
  pinMode(DirRight, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(DirLeft, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(LinTrig, OUTPUT);
  pinMode(LinEcho, INPUT);


}

void loop() {
  
  nh.spinOnce();
  
}








