/*************************************************************
Basic Arduino sketch for the PB6600 Stepper Driver + Adafruit 5V Trinket Pro micro-controller
Rotates stepper motor in 1 direction at slow or high speed.
Uses the Stepper.h library that comes by default with the Arduino IDE (no need to download library)

Version = 1.0    (11/2024) by Raphtronic https://raphtronic.blogspot.com

Schematics, CAD, Print and Code files at https://github.com/RaphTronic/Peristaltic_Pump_with_Stepper_Motor

Feel free to copy / leverage this work
*************************************************************/

#include <Stepper.h>

// Define pin connections
const int directionPin = 16;      // direction of rotation
const int stepPin = 17;           // pulses to rotate motor
const int startSwitchPin = 10;     // start / stop switch
const int speedSwitchPin = 9;    // speed switch

// Motor control states
bool start = false;               // if true, motor is ON an spinning
bool highSpeed = false;           // if true rotation is fast, if false slow rotation

// Start & Speed switches' debounce variables
int startSWRead;
int startSWState;
int lastStartSWState = HIGH;
unsigned long lastStartSWTime = 0;
int speedSWRead;
int speedSWState;
int lastSpeedSWState = HIGH;
unsigned long lastspeedSWTime = 0;
unsigned long debounceDelay = 50;

void setup()
{
  Serial.begin(115200);

  pinMode(stepPin, OUTPUT);
  pinMode(directionPin, OUTPUT);

  pinMode(startSwitchPin, INPUT);
  pinMode(speedSwitchPin, INPUT);
  // setting input pins to high means turning on internal pull-up resistors
  digitalWrite(startSwitchPin, HIGH);
  digitalWrite(speedSwitchPin, HIGH);

  digitalWrite(directionPin, HIGH);   // Set rotation direction
}

void loop()
{
  readMomentarySwitches();

  if (start) {
    // Motor is ON, increase rotation by 1 step by sending a high pulse
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);

    if (highSpeed) {  // delay before next loop / step
      delay(10);      // high speed = low delay
    } else {
      delay(300);     // slow speed
    }
  }
}

void readMomentarySwitches()
{
  startSWRead = digitalRead(startSwitchPin);
  speedSWRead = digitalRead(speedSwitchPin);

  if (startSWRead != lastStartSWState) {
    lastStartSWTime = millis();       // reset the debouncing timer
  }

  if (speedSWRead != lastSpeedSWState) { 
    lastspeedSWTime = millis();       // reset the debouncing timer
  }

  if ((millis() - lastStartSWTime) > debounceDelay) {
    if (startSWRead != startSWState) {
      if (startSWRead == 0) {         // button is debounced and being pushed, so changed state
        startSWState = startSWRead;
        start = !start;               // change the start / stop state
      } else {
        startSWState = startSWRead;   // button is debounced and was released
      }
    }
  }

  if ((millis() - lastspeedSWTime) > debounceDelay) {
    if (speedSWRead != speedSWState) {
      if (speedSWRead == 0) {         // button is debounced and being pushed, so changed state
        speedSWState = speedSWRead;
        highSpeed = !highSpeed;       // change the speed state
      } else {
        speedSWState = speedSWRead;   // button is debounced and was released
      }
    }
  }

  lastStartSWState = startSWRead;
  lastSpeedSWState = speedSWRead;

  // Serial.println(String(startSWRead) + " : " + String(speedSWRead) + " start: " + String(start) + " speed: " + String(highSpeed));
}