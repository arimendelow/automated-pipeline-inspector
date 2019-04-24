/* Ping))) Sensor
  
   This sketch reads a PING))) ultrasonic rangefinder and returns the
   distance to the closest object in range. To do this, it sends a pulse
   to the sensor to initiate a reading, then listens for a pulse 
   to return.  The length of the returning pulse is proportional to 
   the distance of the object from the sensor.
     
   The circuit:
	* +V connection of the PING))) attached to +5V
	* GND connection of the PING))) attached to ground
	* SIG connection of the PING))) attached to digital pin 7

   http://www.arduino.cc/en/Tutorial/Ping
   
   created 3 Nov 2008
   by David A. Mellis
   modified 30 Aug 2011
   by Tom Igoe
 
   This example code is in the public domain.

 */

// Sensors
const int LinTrig = 2;    //Actuator Sensor
const int LinEcho = 3;    //Actuator Sensor

long duration, inches;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
    Serial.flush();
    
    
  pinMode(LinTrig, OUTPUT);
  pinMode(LinEcho, INPUT);
}

void loop()
{
  Sensor1();
  Serial.println(inches);
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
