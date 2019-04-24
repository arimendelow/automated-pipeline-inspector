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
const int DistTrig = 12;  //Distance Sensor for LIDAR
const int DistEcho = 13;  //Distance Sensor for LIDAR

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
long lidarduration, lidarmm;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  Serial1.begin(9600);
  // clears any serial signals previously sent:
  Serial.flush();
  Serial1.flush();

  pinMode(LinearOut, OUTPUT); //Shield
  pinMode(LinearIn, OUTPUT); //Shield
  pinMode(LinearSpeed, OUTPUT); //Shield
  pinMode(DirRight, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(DirLeft, OUTPUT);
  pinMode(PwmRight, OUTPUT);
  pinMode(LinTrig, OUTPUT);
  pinMode(LinEcho, INPUT);
  pinMode(DistTrig, OUTPUT);
  pinMode(DistEcho, INPUT);
}

void loop() {
  Sensor1();//linear
  delay(200);

  if (inches <= 3) {   //if a pipe support is within 3 inches of sensor:
    
    BrakingBase();    //stop driving
    Serial1.write(1);  //tell cameras to take one last read
    delay(13000);     //wait for scanning to finish
    Retract();       //retract the arm
    delay(41000);     //wait until fully retracted
    StopArm();       //stop arm
    delay(1000);
    ForwardBase();    //move foward
    delay(2500);      //wait until past pipe
    BrakingBase();    //stop driving
    delay(1000);
    Extend();        //extend arm
    delay(41000);  //wait until extended
    
  } else {                  //or else
    
    StopArm();         //keep arm extended
    Serial1.write(1);    //tell arm to scan
    delay(13000);       //wait for scanto complete
    ForwardBase();      //move foward
    delay(500);
    BrakingBase();      //stop
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

void Sensor2() {    //helps LIDAR

  digitalWrite(DistTrig, LOW);
  delayMicroseconds(5);
  digitalWrite(DistTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(DistTrig, LOW);
  lidarduration = pulseIn(DistEcho, HIGH);
  lidarmm = (lidarduration / 2) / 2.91;
  Serial.write('U');
  Serial.write(int(lidarmm));
  Serial.write('X');
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

float communicate () {       //Raspi communication

  if (Serial.available() > 0) {
    
    char x = Serial.read() - '0';
    
    if (x == 9) {
      
      obstruction();
      
    }
    
    if (x == 8) {
      
      int i = Serial.read() - '0';
      int j = Serial.read() - '0';
      int k = Serial.read() - '0';
      float n = i * 100 + j * 10 + k * 1;
      n = n / 100.0;
      
      return n;
      
    }
    
  }
  
  return 1.0;
  
}

void ForwardBase () {          //moves system forward

  float corr = communicate();      //recieve correction factor
  Serial.print(corr);
  pwm_value = 125.0;
  digitalWrite (DirRight, HIGH);
  digitalWrite(DirLeft, HIGH);
  analogWrite(PwmRight, (pwm_value)*corr); //LIDAR corrects motors
  analogWrite(PwmLeft, (pwm_value) / corr); //LIDAR corrects motors
  Serial.println("PWM:");
  Serial.println(pwm_value);

}

void BrakingBase() {   //stops system

  pwm_value = 0;
  analogWrite(PwmRight, pwm_value);
  analogWrite(PwmLeft, pwm_value);
  Serial.println("PWM:");
  Serial.println(pwm_value);
  
}

void BackwardBase() {         //moves system backwards

  pwm_value = 125;
  digitalWrite (DirRight, LOW);
  digitalWrite(DirLeft, LOW);
  analogWrite(PwmRight, pwm_value);
  analogWrite(PwmLeft, pwm_value);
  Serial.println("PWM:");
  Serial.println(pwm_value);
  
}

void obstruction() { // does nothing for some time
  
  int a = 1000; //10 seconds
  BrakingBase();
  StopArm();
  
  while (a > 0) {
    
    if (Serial.available() > 0) {
      
      if ((Serial.read() - '0') == 9) { //obstruction still there
        
        a = 1000;
      
      }
      
    }
    
    delay(10);
    a = a - 1;
    
  }
  
}

