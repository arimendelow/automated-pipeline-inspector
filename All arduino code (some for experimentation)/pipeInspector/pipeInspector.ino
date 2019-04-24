/*
    This is the code for the master board.
    The linear actuator, motor driver and sensors are controlled by this board.

    Connect master pin 0 to slave pin 1
    Connect master pin 1 to slave pin 0

    Place C-clamp into neutral postition before power is applied.

    WARNING!:
    Before uploading, disconnect the Motor Driver power to keep robot still.
    WARNING!
*/
// Sensors
const int LinTrig = 2;    //Actuator Sensor
const int LinEcho = 3;    //Actuator Sensor

// Linear Actuator
int LinearOut = 11;        //These pins are designated from the motor shield.
int LinearSpeed = 9;      //DO NOT ASSIGN ANYTHING ELSE TO THESE PINS!
int LinearIn = 8;        // Wiring for actuator should be OUT1: Red. OUT2: Black.

// Motor Driver
int DirRight = 6; //Wiring for the motors should be:
int PwmRight = 5; //M1A: Red - M1B: Black - M2A:Red - M2B:Black
int DirLeft = 7;
int PwmLeft = 10;
int pwm_value;

long duration, inches;


void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  // clears any serial signals previously sent:
  Serial.flush();

  //Serial.println("initialized");

  pinMode(LinearOut, OUTPUT); //Shield
  pinMode(LinearIn, OUTPUT); //Shield
  pinMode(LinearSpeed, OUTPUT); //Shield
  pinMode(DirRight, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(DirLeft, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(LinTrig, OUTPUT);
  pinMode(LinEcho, INPUT);
  
  //Serial.println("extending arm...");
  Extend();        //extend arm
  //delay(20000);  //wait until extended
  
}

void loop() {
  Sensor1();//linear
  
  //Serial.println("inches: " + inches);
  
  delay(200);

  if (inches <= 3) {   //if a pipe support is within 3 inches of sensor:
    
    //Serial.println("stopping robot...");
    
    //stop robot
    move_base(0, PwmRight, DirRight);
    move_base(0, PwmLeft, DirLeft);
    
    Serial.write(1); //tell cameras to take one last read
    //Serial.println("one last check...");
    
    delay(13000);     //wait for scanning to finish
    Retract();       //retract the arm
    //Serial.println("retracting arm...");
    
    delay(20000);     //wait until fully retracted
    StopArm();       //stop arm
    delay(1000);
    
    //move forward
    
    //Serial.println("moving forward...");
    move_base(62, PwmRight, DirRight);
    move_base(62, PwmLeft, DirLeft);
    
    delay(2500);      //wait until past pipe
    
    //stop robot
    //Serial.println("stopping robot...");
    move_base(0, PwmRight, DirRight);
    move_base(0, PwmLeft, DirLeft);
    
    delay(1000);
    //Serial.println("extending arm...");
    Extend();        //extend arm
    delay(20000);  //wait until extended
    
  } else {                  //or else
 
    Serial.println("checking pipe...");
    Serial.write(1);    //tell arm to scan
    delay(13000);       //wait for scanto complete
    
    //move forward
    //Serial.println("moving forward...");
    move_base(62, PwmRight, DirRight);
    move_base(62, PwmLeft, DirLeft);
    
    delay(500);
    
    //stop robot
    //Serial.println("stopping...");
    move_base(0, PwmRight, DirRight);
    move_base(0, PwmLeft, DirLeft);;
    
    delay(500);
    
  }
  
  delay(100);
}

void Sensor1() {   //detects supports

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

  digitalWrite(LinearIn, LOW);
  digitalWrite(LinearOut, HIGH);
  analogWrite(LinearSpeed, 255);
  
}

void Retract() {             //retracts arm

  digitalWrite(LinearIn, HIGH);
  digitalWrite(LinearOut, LOW);
  analogWrite(LinearSpeed, 255);
  
}

void StopArm() {             //stops arm

  analogWrite(LinearSpeed, 0);
  
}

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

