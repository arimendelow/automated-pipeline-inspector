
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>

ros::NodeHandle nh;

void joyCallback( const sensor_msgs::Joy::ConstPtr& joy);

// Motor Driver
int DirRight = 6; //Wiring for the motors should be:
int PwmRight = 5; //M1A: Red - M1B: Black - M2A:Red - M2B:Black
int DirLeft = 7;
int PwmLeft = 10;
int pwm_value;

int lienar_, angular_;
double l_scale, a_scale;
ros::Publisher vel_pub_;
ros::Subscriber joy_sub_;

void setup() {

  pinMode(DirRight, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(DirLeft, OUTPUT);
  pinMode(PwmRight, OUTPUT);




}

void loop() {

  pwm_value = 255.0;
  digitalWrite(DirRight, HIGH);
  analogWrite(PwmRight, pwm_value);
  digitalWrite(DirLeft, HIGH);
  analogWrite(PwmLeft, pwm_value);

}


