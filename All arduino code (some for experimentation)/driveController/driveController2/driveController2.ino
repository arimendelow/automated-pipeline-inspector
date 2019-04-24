#include <string.h>

// Motor Driver
int DirRight = 6; //Wiring for the motors should be:
int PwmRight = 5; //M1A: Red - M1B: Black - M2A:Red - M2B:Black
int DirLeft = 7;
int PwmLeft = 10;
int pwm_value;


void setup() {
  
    Serial.begin(115200);
    
    pinMode(DirRight, OUTPUT);
    pinMode(PwmRight, OUTPUT);
    pinMode(DirLeft, OUTPUT);
    pinMode(PwmRight, OUTPUT);
    
}


// function to control motor
// speed is how fast the motor rotates
void control_motor(int speed, int pwmPin, int INaPin){
    if(speed > 0){
        analogWrite(pwmPin, speed);
        digitalWrite(INaPin, HIGH);
    }
    else if(speed < 0){
        analogWrite(pwmPin, -speed);
        digitalWrite(INaPin, LOW);
    }
    else{
      
        analogWrite(PwmRight, 0);
        digitalWrite(DirRight, HIGH);
  
        analogWrite(PwmLeft, 0);
        digitalWrite(DirLeft, HIGH);

   }
}

// In time loop, receive from serial and control 2 motors
void loop() {
    static int speed[2];
    static char buff[30];
    int counter = 0;

    // read command from raspberry pi
    while(Serial.available()){
        buff[counter] = Serial.read();
        if (counter > 30 || buff[counter] == '*') {
            buff[counter] = '\0';
            speed[0]=atoi(strtok(buff,","));
            speed[1]=atoi(strtok(NULL,","));
        }
        else{
            counter++;
        }
    }

    // control motors
    control_motor(speed[0], PwmRight, DirRight);
    control_motor(speed[1], PwmLeft, DirLeft);

    // send messages to raspberry pi
    Serial.print(speed[0]); Serial.print(",");
    Serial.print(speed[1]);

    delay(100);
}
