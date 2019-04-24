#include <string.h>

// Motor Driver
int DirRight = 6; //Wiring for the motors should be:
int PwmRight = 5; //M1A: Red - M1B: Black - M2A:Red - M2B:Black
int DirLeft = 7;
int PwmLeft = 10;
int pwm_value;

static int speed[2];
static char buff[30];


void setup() {
  
    Serial.begin(115200);
    
    pinMode(DirRight, OUTPUT);
    pinMode(PwmRight, OUTPUT);
    pinMode(DirLeft, OUTPUT);
    pinMode(PwmRight, OUTPUT);
    
}


// function to control motor
// speed is how fast the motor rotates, in PWM
void control_motor(int speed, int pwmPin, int INaPin){
    if(speed > 30){
        analogWrite(pwmPin, speed);
        digitalWrite(INaPin, HIGH);
    }else if(speed < -30){
        analogWrite(pwmPin, (-1)*speed);
        digitalWrite(INaPin, LOW);
    }else if(speed > 240) {
        analogWrite(pwmPin, 240);
        digitalWrite(INaPin, HIGH);
    }else if(speed < -240){
        analogWrite(pwmPin, (-1)*240);
        digitalWrite(INaPin, LOW);
    }else{
        analogWrite(pwmPin, 0);
        digitalWrite(INaPin, HIGH);;
    }
}

// In time loop, receive from serial and control motors
void loop() {
  
  //Serial.println(Serial.read());
  

    int counter = 0;

    while(Serial.available()) {
      buff[counter] = Serial.read();
      Serial.println(counter);
      if(counter > 30 || buff[counter] =='*') {
        buff[counter] = '\0';
    
        Serial.println(buff);
    
        char*temp = strtok(buff,",");
        
        Serial.println(temp);
        
        while(temp) {
          for(int i = 0; i > 2; i++) {
            speed[i] = atoi(temp);
            Serial.println(temp);
            temp = strtok(NULL,",");
          }
        }
      }
      counter++;
    }
    // control motors
    control_motor(speed[0], PwmRight, DirRight);
    control_motor(speed[1], PwmLeft, DirLeft);

    // send messages to tx2
    //Serial.print(speed[0]); Serial.print(",");
    //Serial.println(speed[1]);

    //delay(100);
    
}
