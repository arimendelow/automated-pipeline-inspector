#include <string.h>

// Motor Driver
int DirRight = 6; //Wiring for the motors should be:
int PwmRight = 5; //M1A: Red - M1B: Black - M2A:Red - M2B:Black
int DirLeft = 7;
int PwmLeft = 10;
int pwm_value;

static int speed[3];
static char buff[10];

static char rightCmd[5];
static char leftCmd[5];

static int left = 0;
static int right = 0;

const char s[2] = "*";
static char *token;

void setup() {
  
    Serial.begin(115200);
    
    pinMode(DirRight, OUTPUT);
    pinMode(PwmRight, OUTPUT);
    pinMode(DirLeft, OUTPUT);
    pinMode(PwmRight, OUTPUT);
    
}

void loop() {
 
  int counter = 0;
  int status = 1;
  
  while(Serial.available()) {
    

    for(int i = 0; i < 10; i++) {
      buff[i] = Serial.read();
      Serial.print(buff[i]);
      int j = 0;
      
      
      
      if(status == 1){
        if(buff[i] != ',') {
          rightCmd[i] = buff[i];
          //Serial.print(rightCmd[i]);
        } else {
          //Serial.println();
          right = atoi(&rightCmd[1]);
          status = 2;
          i++;
        }
      } else {
        if(buff[i] != '*') {
          leftCmd[j] = buff[i];
          //Serial.print(leftCmd[j]);
          j++;
        } else {
          //Serial.println();
          //Serial.println();
          left = atoi(&leftCmd[1]);
          i = 0;
          j = 0;
          
          control_motor(right, PwmRight, DirRight);
          control_motor(left, PwmLeft, DirLeft);
          
          status = 1;
          break;
        }
      }
 
      
    }

    
  }
      
}

// function to control motor
// speed is how fast the motor rotates
// Please set pwmPin, InaPin and INbPin for the motor you want to drive
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
        analogWrite(pwmPin, 0);
        digitalWrite(INaPin, LOW);;
    }
}


