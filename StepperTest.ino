/****************************************************************************** 
SparkFun Easy Driver Basic Demo
Toni Klopfenstein @ SparkFun Electronics
March 2015
https://github.com/sparkfun/Easy_Driver

Simple demo sketch to demonstrate how 5 digital pins can drive a bipolar stepper motor,
using the Easy Driver (https://www.sparkfun.com/products/12779). Also shows the ability to change
microstep size, and direction of motor movement.

Development environment specifics:
Written in Arduino 1.6.0

This code is beerware; if you see me (or any other SparkFun employee) at the local, and you've found our code helpful, please buy us a round!
Distributed as-is; no warranty is given.

Example based off of demos by Brian Schmalz (designer of the Easy Driver).
http://www.schmalzhaus.com/EasyDriver/Examples/EasyDriverExamples.html
******************************************************************************/

#include <stdio.h>
#include <ctype.h>

//Declare pin functions on Redboard
#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define EN  6

//Declare variables for functions
//char user_input;
//int x;
int y;
int state;

void setup() {
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);
  resetEDPins(); //Set step, direction, microstep and enable pins to default states
  Serial.begin(9600); //Open Serial connection for debugging
  Serial.println("Begin motor control");
  Serial.println();
}

const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;

char *token = NULL;
char cmd[2] = "";
char asciiSteps[10] = ""; 
unsigned long targetSteps = 0;

void loop() 
{ 
  digitalWrite(EN, LOW); //Pull enable pin low to allow motor control
  receiveSerialInput();
  processSerialInput();
  resetEDPins();
}

//Reset Easy Driver pins to default states
void resetEDPins()
{
  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(EN, HIGH);
}

void receiveSerialInput() 
{
 static byte ndx = 0;
 char endMarker = '\n';
 char rc;
 
 while (Serial.available() > 0 && newData == false) 
 {
   rc = Serial.read();

   if (rc != endMarker) 
   {
     receivedChars[ndx] = rc;
     ndx++;
     if (ndx >= numChars) 
     {
      ndx = numChars - 1;
     }
   }
   else 
   {
     receivedChars[ndx] = '\0'; // terminate the string
     ndx = 0;
     newData = true;
    }
  }
}

void processSerialInput()
{
  char *ptr;

  if(newData == true) 
  {
    //Serial.print("This just in ... ");
    //Serial.println(receivedChars);
    
    token = strtok(receivedChars," ");
    strcpy(cmd,token);
 //   Serial.println(cmd);
    
    token = strtok(NULL," ");
    strcpy(asciiSteps,token);
//    Serial.println(asciiSteps);

    targetSteps = strtoul(asciiSteps,&ptr,10);

    Serial.println(targetSteps);
    switch(tolower(cmd[0]))
    {
      case 'f':
        stepForward(targetSteps);
      break;
      case 'r':
        stepReverse(targetSteps);
      break;
      default:
        Serial.println("Invalid option entered.");
      break;
    }
    newData = false;
  }
}

void stepForward(unsigned long steps)
{
  unsigned long i;
  Serial.print("Moving forward ");
  Serial.print(steps);
  Serial.println(" steps");

  digitalWrite(dir, LOW); //Pull direction pin low to move "forward"
  for(i = 1; i<steps; ++i)  //Loop the forward stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step forward
    delay(1);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  Serial.println("Enter new option");
  Serial.println();
}

void stepReverse(unsigned long steps)
{
  unsigned long i;
  Serial.print("Moving reverse ");
  Serial.print(steps);
  Serial.println(" steps");
  
  digitalWrite(dir, HIGH); //Pull direction pin high to move in "reverse"
  for(i = 1; i<steps; ++i)  //Loop the stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step
    delay(1);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  Serial.println("Enter new option");
  Serial.println();
}

// 1/8th microstep foward mode function
void SmallStepMode()
{
  unsigned long i;
  
  Serial.println("Stepping at 1/8th microstep mode.");
  digitalWrite(dir, LOW); //Pull direction pin low to move "forward"
  digitalWrite(MS1, HIGH); //Pull MS1, and MS2 high to set logic to 1/8th microstep resolution
  digitalWrite(MS2, HIGH);
  for(i = 1; i < 1000; i++)  //Loop the forward stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step forward
    delay(1);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  Serial.println("Enter new option");
  Serial.println();
}

//Forward/reverse stepping function
void ForwardBackwardStep()
{
  unsigned long i;
  Serial.println("Alternate between stepping forward and reverse.");
  for(i = 1; i < 5; i++)  //Loop the forward stepping enough times for motion to be visible
  {
    //Read direction pin state and change it
    state=digitalRead(dir);
    if(state == HIGH)
    {
      digitalWrite(dir, LOW);
    }
    else if(state ==LOW)
    {
      digitalWrite(dir,HIGH);
    }
    
    for(y=1; y<1000; y++)
    {
      digitalWrite(stp,HIGH); //Trigger one step
      delay(1);
      digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
      delay(1);
    }
  }
  Serial.println("Enter new option:");
  Serial.println();
}
