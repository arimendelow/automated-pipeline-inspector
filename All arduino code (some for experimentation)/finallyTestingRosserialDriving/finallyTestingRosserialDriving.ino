
#include <ros.h>
#include <geometry_msgs/Twist.h>

ros::NodeHandle nh;
geometry_msgs::Twist msg;

// Motor Driver
int DirRight = 6; //Wiring for the motors should be:
int PwmRight = 5; //M1A: Red - M1B: Black - M2A:Red - M2B:Black
int DirLeft = 7;
int PwmLeft = 10;
int pwm_value;

float x;
float angular;

int right;
int left;

void messageCb( const geometry_msgs::Twist& cmd_vel){
  
  x = cmd_vel.linear.x;
  angular = cmd_vel.angular.z;
  
  right = (int)((x + angular) * 50);
  left = (int)((x - angular) * 50);
  
  Serial.print(right); Serial.print(","); Serial.println(left);
  
  // control motors
  move_base(right, PwmRight, DirRight);
  move_base(left, PwmLeft, DirLeft);
  
}   


ros::Subscriber<geometry_msgs::Twist> sub("/cmd_vel", messageCb );


void setup() {
  
  Serial.begin(115200);
  
  pinMode(DirRight, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(DirLeft, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  
  nh.initNode();
  nh.subscribe(sub);
  
}

void loop() {
  
  nh.spinOnce();
  
}

// function to control motor
// speed is how fast the motor rotates, in PWM
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
