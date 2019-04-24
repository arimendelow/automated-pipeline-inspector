
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>

// Motor Drive globals
int DirRight = 6; //Wiring for the motors should be:
int PwmRight = 5; //M1A: Red - M1B: Black - M2A:Red - M2B:Black
int DirLeft = 7;
int PwmLeft = 10;
int pwm_value;

//Encoder globals
int encoder_left_pin = 1;
int encoder_right_pin = 2;
volatile int encoder_left_count = 0;
volatile int encoder_right_count = 0;
int encoder_left_offset = 0;
int encoder_right_offset = 0;

//Physical constants
const float pi = 3.14159;
const float WHEEL_RADIUS = 0.15875;  // meters
const float WHEEL_SEPARATION = 0.7493; //meters
const float WHEEL_TO_MOTOR_RATIO = 1.0;       // wheel rotation/motor rotation
const float MOTOR_TO_ENCODER_RATIO = 1.0;     // motor rotation/encoder rotation
const float ENCODER_TICKS_PER_REV = 100.0;    // encoder ticks/revolution

//Distance and angles from base link to laser (note that base link is defined as the center point between your two weels)
const float BASE_TO_LASER_X = 1.0;            //[m] distance from base_link to laser in x direction
const float BASE_TO_LASER_Y = 1.0;            //[m] distance from base_link to laser in y direction
const float BASE_TO_LASER_Z = 1.0;            //[m] distance from base_link to laser in z direction
const float BASE_TO_LASER_YAW = 1.0;          //[rad] angle from base_link to laser in yaw direction (about z axis)
const float BASE_TO_LASER_PITCH = 1.0;        //[rad] angle from base_link to laser in pitch direction (about y axis)
const float BASE_TO_LASER_ROLL = 1.0;         //[rad] angle from base_link to laser in roll direction (about x axus)

//Tunable values
const float ENCODER_FUDGE_FACTOR_LEFT = 1.0;  // used for tuning left encoder
const float ENCODER_FUDGE_FACTOR_RIGHT = 1.0; // used for tuning right encoder
const float FORWARD_ODOM_SCALE = 1.0;         // used for tuning forward odometry
const float ANGULAR_ODOM_SCALE = 1.0;         // used for tuning angular odometry

//Calculated
const float TICKS_TO_ANGLE = (2*pi/ENCODER_TICKS_PER_REV)*ENCODER_TO_MOTOR_RATIO*MOTOR_TO_WHEEL_RATIO;
const float TICKS_TO_ANGLE_LEFT = ENCODER_FUDGE_FACTOR_LEFT * TICKS_TO_ANGLE;
const float TICKS_TO_ANGLE_RIGHT = ENCODER_FUDGE_FACTOR_RIGHT * TICKS_TO_ANGLE;

//Odometry globals
float odom_x = 0.0;
float odom_y = 0.0;
float odom_a = 0.0;
float left_wheel_angle_old = 0.0;
float right_wheel_angle_old = 0.0;
ros::Time time_old;

// ROS globals
ros::NodeHandle nh;
tf::TransformBroadcaster tf_broadcaster;
tf::TransformStamped base_to_laser_tf;
nav_msgs::Odometry odom_msg;
ros::Publisher odom_pub("/odom", &odom_msg);
//Declared subsciber near setup() due to depenency on callback
ros::Rate loop_rate(50); //number is in hz, may need to adjust this number depending on cpu performance and control requirements


/////////////////////////////////
//     Motor control functions
/////////////////////////////////

//Motor control callback
void cmdVelcb( const geometry_msgs::Twist& cmd_vel){
  
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


////////////////////////////////
//         Encoder functions
////////////////////////////////

// Interrupt service routines for encoders
void left_encoder_ISR(){
  encoder_left_count++;
}
void right_encoder_ISR(){
  encoder_right_count++;
}

// Convert encoder ticks to wheel angles
float get_left_wheel_angle(){
  return (encoder_left_count - encoder_left_offset) * TICKS_TO_ANGLE_LEFT;
}
float get_right_wheel_angle(){
  return (encoder_right_count - encoder_right_offset) * TICKS_TO_ANGLE_RIGHT;
}

// Reset encoder values by saving encoder offsets
// Not used currently, but may be useful in future
void reset_left_encoder(){
  encoder_left_offset = encoder_left_count;
}
void reset_right_encoder(){
  encoder_right_offset = encoder_right_count;
}



/////////////////////////////
//      Odometry functions
/////////////////////////////

// Top level function to handle odometry updates
void update_odometry(){
  
  // Declare local variables
  float left_wheel_angle;  // current wheel angles
  float right_wheel_angle;
  float left_wheel_ang_vel;  // wheel angular velocities
  float right_wheel_ang_vel;
  float left_wheel_lin_vel;  //wheel linear velocites
  float right_wheel_lin_vel;
  float vx;                  //local velocities
  float vy;
  float va;
  float dx_global;           //global position and angle changes
  float dy_global;
  float da_global;
  
  //Update values from previous loop
  ros::Time time_new = ros::Time::now();
  left_wheel_angle = get_left_wheel_angle();
  right_wheel_angle = get_right_wheel_angle();
  
  //Calculate wheel velocites
  dt = (time_new - time_old).toSec();
  left_wheel_ang_vel = (left_wheel_angle - left_wheel_angle_old)/dt;
  right_wheel_ang_vel = (right_wheel_angle - right_wheel_angle_old)/dt;
  left_wheel_lin_vel = left_wheel_ang_vel * WHEEL_RADIUS;
  right_wheel_lin_vel = right_Wheel_ang_vel * WHEEL_RADIUS;
  
  //Calculate robot velocities in local coordiante frame
  vx = (left_wheel_lin_vel + right_wheel_lin_vel)/2 * FORWARD_ODOM_SCALE;
  vy = 0;
  va = (left_wheel_lin_vel - right_wheel_lin_vel)/WHEEL_SEPARATION * ANGULAR_ODOM_SCALE;
  
  //Compute global position change from local velocity
  //Note that angle being used is angle computed in previous loop
  dx_global = vx * cos(odom_a) * dt;
  dy_global = vy * sin(odom_a) * dt;
  da_global = va * dt;
  
  //Apply update to global position
  odom_x += dx_global;
  odom_y += dy_global;
  odom_a += da_global;
  
  
  //Build TF message
  tf::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(odom_a);
  tf::TransformStamped odom_trans;
  //tf header
  odom_trans.header.stamp = time_new;
  odom_trans.header.frame_id = "odom";
  odom_trans.child_frame_id = "base_link";
  //tf body
  odom_trans.transform.translation.x = odom_x;
  odom_trans.transform.translation.y = odom_y;
  odom_trans.transform.translation.z = 0.0;
  odom_trans.transform.rotation = odom_quat;
  //send the transform
  tf_broadcaster.sendTransform(odom_trans);


  //Build odometry message
  odom_msg.header.stamp = time_new;
  odom_msg.header.frame_id = "odom";
  //set the position
  odom_msg.pose.pose.position.x = odom_X;
  odom_msg.pose.pose.position.y = odom_y;
  odom_msg.pose.pose.position.z = 0.0;
  odom_msg.pose.pose.orientation = odom_quat;
  //set the velocity
  odom_msg.child_frame_id = "base_link";
  odom_msg.twist.twist.linear.x = vx;
  odom_msg.twist.twist.linear.y = vy;
  odom_msg.twist.twist.angular.z = va;
  //publish the message
  odom_pub.publish(&odom_msg);
  

  //Set old values for next loop
  time_old = time_new;
  left_wheel_angle_old = left_wheel_angle;
  right_wheel_angle_old = right_wheel_anlge;

}


/////////////////////////////////////
//       Miscellaneous functions
/////////////////////////////////////

void setup_static_transforms(){
  
  //header
  base_to_laser_trans.header.stamp = ros::Time::now();
  base_to_laser_trans.header.frame_id = "base_link";
  base_to_laser_trans.child_frame_id = "laser";
  //tf body
  base_to_laser_trans.transform.translation.x = BASE_TO_LASER_X;
  base_to_laser_trans.transform.translation.y = BASE_TO_LASER_Y;
  base_to_laser_trans.transform.translation.z = BASE_TO_LASER_Z;
  tf::Quaterion quat;
  quat.setEulerZYX(BASE_TO_LASER_YAW,BASE_TO_LASER_PITCH,BASE_TO_LASER_ROLL);
  base_to_laser_trans.transform.rotation = quat;
  
}


void publish_static_transforms(){
  //update header time
  base_to_laser_trans.header.stamp = ros::Time::now();
  tf_broadcaster.sendTransform(base_to_laser_trans);
}


///////////////////////////
//       Main functions
///////////////////////////

//declaring subscriber here b/c of dependency on callback
ros::Subscriber<geometry_msgs::Twist> vel_sub("/cmd_vel", cmdVelcb );

void setup() {
  
  Serial.begin(115200);
  
  //Motor pin setup
  pinMode(DirRight, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(DirLeft, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  
  //Encoder pin setup
  pinMode(encoder_left_pin, INPUT_PULLUP);
  pinMode(encoder_right_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoder_left_pin), left_encoder_ISR, LOW);
  attachInterrupt(digitalPinToInterrupt(encoder_right_pin), right_encoder_ISR, LOW);
  
  //Setup node and messaging
  nh.initNode();
  nh.subscribe(vel_sub);
  nh.advertise(odom_pub);
  
  setup_static_transforms();
  time_old = ros::Time::now();
  
}

void loop() {
  
  update_odometry();
  publish_static_transforms();
  nh.spinOnce();
  loop_rate.sleep();
  
}


