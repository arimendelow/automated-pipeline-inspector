/*
   ACTUATOR SETUP
   This code is only used to extend the arm out into starting position.
   Do not use this code when running the entire system.
   
   To extend the arm, type anything into the serial monitor.
*/
int LinearOut = 11;  //Red in Out 1 Black in Out 2
int LinearIn = 8;
int LinearSpeed = 9;


void setup() {
  
  // initialize serial communication:
  Serial.begin(9600);
  establishContact();
  pinMode(LinearOut, OUTPUT); //Shield
  pinMode(LinearIn, OUTPUT); //Shield
  pinMode(LinearSpeed, OUTPUT); //Shield
  
}


void loop() {
  
  in();
  //out();
  
}


void establishContact() {

  while (Serial.available() <= 0) {
    
    Serial.println('x');
    delay(1000);
    
  }
  
  Serial.flush();
  
}

void in() {
  
  digitalWrite(LinearIn, HIGH);
  digitalWrite(LinearOut, LOW);
  analogWrite(LinearSpeed, 255);
  
}

void out() {
  
  digitalWrite(LinearIn, LOW);
  digitalWrite(LinearOut, HIGH);
  analogWrite(LinearSpeed, 255);
  
}
