#include <Arduino.h>
#include <ros.h>
#include <std_msgs/UInt16.h>

// Motor Driver
int DirRight = 7; //Wiring for the motors should be:
int PwmRight = 10; //M1A: Red - M1B: Black - M2A:Red - M2B:Black
int DirLeft = 6;
int PwmLeft = 5;
int pwm_value;

ros::NodeHandle nh;

std_msgs::UInt16 whatIsRead;

void callback( const std_msgs::UInt16& cmd_msg) {
  
  if(cmd_msg.data == 0) {
    
    control_motor(-255, PwmRight, DirRight);
    control_motor(-255, PwmLeft, DirLeft);
    
  } else if(cmd_msg.data == 1) {
    
    control_motor(255, PwmRight, DirRight);
    control_motor(255, PwmLeft, DirLeft);
    
  }
  
}

ros::Subscriber<std_msgs::UInt16> sub("motor", callback);


ros::Publisher chatter("chatter", &whatIsRead);

void setup() {
  
    nh.initNode();
    nh.subscribe(sub);
    nh.advertise(chatter);
  
    pinMode(DirRight, OUTPUT);
    pinMode(PwmRight, OUTPUT);
    pinMode(DirLeft, OUTPUT);
    pinMode(PwmRight, OUTPUT);
    
}

void loop() {
  
    whatIsRead.data = 123;
    chatter.publish( &whatIsRead );
    nh.spinOnce();
    delay(1);
  
}

void control_motor(int speed, int pwmPin, int INaPin){
  if(speed > 30 && speed <= 240){
    analogWrite(pwmPin, speed);
    digitalWrite(INaPin, HIGH);
  }else if(speed < -30 && speed >= -240){
    analogWrite(pwmPin, (-1)*speed);
    digitalWrite(INaPin, LOW);
  }else if(speed > 240) {
    control_motor(240, pwmPin, INaPin);
//    analogWrite(pwmPin, 240);
//    digitalWrite(INaPin, HIGH);
  }else if(speed < -240){
    control_motor(-240, pwmPin, INaPin);
//    analogWrite(pwmPin, (-1)*240);
//    digitalWrite(INaPin, LOW);
  }else{
    analogWrite(pwmPin, 0);
    digitalWrite(INaPin, HIGH);;
  }
}
