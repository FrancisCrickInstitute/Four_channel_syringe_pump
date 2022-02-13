#include "TeensyStep.h"
#include <Encoder.h>

Encoder knob(10, 11);
const int encoder_SW = 12;
int knob_pos = 0;

Stepper pump(7, 6);       // STEP pin: 7, DIR pin: 6  // The stepper class encapsulates the physical properties of a stepper motor like pin numbers of the STEP and DIR signals, speed and acceleration of the motor.
StepControl step_controller;    // The StepControl class is used to synchronously move up to 10 motors to their target positions.
//RotateControl rotate_controller; // The RotateControl class is used to synchronously rotate up to 10 motors.

const int MS1 = 3;
const int MS2 = 4;
const int MS3 = 5;
const int Enable = 2;

// limit switches
const int LSW1 = 15;
const int LSW2 = 14;

//Fluid global constants
const int lead_size = 8; // lead size (8mm)
//const int lead_size = 1; // lead size (1mm)
const float steps_rev = 3200; // Stepper resolution at 1/16 steps per revolution
const float syringe_area = 175; // mm^2 for 10 mL syringe
float linear_resolution = (lead_size/steps_rev)*syringe_area; //uL/step or mm^3/step

int Volume;
int Flow_rate;
int steps;
int steps_s;

void setup()
{
  pinMode(MS1, OUTPUT);    // set the MS1, MS2, MS3 and Enable as an outputs
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(Enable, OUTPUT);

  digitalWrite(MS1, HIGH); // sets the digital pin MS1, MS2, MS3 to HIGH
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);
  digitalWrite(Enable, LOW); // set the enable to LOW
  
  // Set the motor max acceleration
  pump.setAcceleration(5000)   // stp/s^2
      .setInverseRotation(false);
  
  pinMode(LSW1, INPUT_PULLUP);
  pinMode(LSW2, INPUT_PULLUP);
  pinMode(encoder_SW, INPUT_PULLUP);
  
  Serial.begin(115200);

  //Homing(); //At the end of the homing routine load the syringe
}

void loop() 
{ 

//Serial.println(linear_resolution);

  while (digitalRead(LSW2)==HIGH && digitalRead(LSW1)==HIGH)
  {

    //pump.setTargetAbs(0);
    pump.setPosition(0);
    
    Volume = 1000; 
    Flow_rate = 1000;
    
    //Convert the volume and the flow rate selected by the user into steps and steps/s
    steps = get_steps(Volume);
    steps_s = get_speed(Flow_rate);
  
    // Assign pump motor properties
    pump.setMaxSpeed(steps_s);         // stp/s
    pump.setTargetAbs(steps);  // Set target position to 1000 steps from current position
  
    //Actuate the pump
    step_controller.move(pump);    // Do the movement of the pump
  
    //Display the volume
//    Serial.print(" ");
//    Serial.print(steps);
//    Serial.print(" ");
//    Serial.println(steps_s);
  
    delay(10000);
  }
  

}

int get_speed(int flow_rate)
{
  float uL_s = flow_rate/60;
  int steps_s;
  steps_s = int(uL_s/linear_resolution);
  return steps_s;
}

int get_steps(int Volume)
{
  int linear_displacement = Volume/linear_resolution;
  return linear_displacement;
}
