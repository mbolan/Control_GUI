#ifndef MOTORCONTROL
#include "MotorControl.h"

#ifndef FUNCTIONDEFS
#include "functionDefs.h"
#endif



AccelStepper stepper(1, 2, 1); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
//setEnablePin(9);
float maxSpd = 1000;
float targetSpeed = 1000;
long targetDist = 5000;
long heatingDist = -12502; //12502 to get to position for 900C holding temp. 13239.5 to get to position for 1000C holding temp
float targetAccel = 2000;
int moveCount = 0;
int moveStage = 0;
int moveState = 0;
int progNum = 0;

IntervalTimer linAcTimer;

void runMoveProg()
{
	//Run the first program
	if (progNum == 0)
	{
		//If the gantry is still in the starting position, set the first target distance to enter the furnace
		if (moveStage == 0)
		{
			targetDist = heatingDist; //12502 to get to position for 900C holding temp. 13239.5 to get to position for 1000C holding temp
									  //targetDist = -1250;
									  //Serial.println("Moving to pickup piece");
		}
		//If the blank has been inserted into the furnace, move back to the cooling position
		else if (moveStage == 1)
		{
			targetDist = -(heatingDist + 2500);
			//targetDist = 625;
			Serial.println("Moving to drop piece");
		}
		//If the blank has been cooled, move back to the starting position
		else if (moveStage == 2)
		{
			targetDist = -(heatingDist + targetDist);
			//targetDist = 625;
			Serial.println("Moving to away from press");
		}
		else
		{
			targetDist = 0;
		}
		stepper.disableOutputs(); //These stepper drivers (TB6600) are enabled with EN+ low. enableOutputs() drives the signal high,
		//so you must invert the function to achieve the desired results. Here we want to disable the stepper motors.
		Serial.println("Enabling outputs");
		stepper.move(targetDist);
		moveState = 1;//Flag that the gantry is moving
	}
	else if (progNum == 1)
	{
	}
	//delay(250);
}

void moveGantry()
{
	//Execute a step as long as the gantry hasn't reached the target
	if (abs(stepper.currentPosition()) < abs(targetDist))
	{
		//Serial.println("Running");
		stepper.run();
	}
	//Otherwise (if it has reached the target) reset the position and disable the motors to prevent electrical interference with the thermocouple
	else
	{
		stepper.setCurrentPosition(0);
		moveState = 0;//Flag that the gantry is nonmoving
		moveStage++;
		stepper.enableOutputs(); //These stepper drivers (TB6600) are enabled with EN+ low. enableOutputs() drives the signal high,
		//so you must invert the function to achieve the desired results. Here we want to disable the stepper motors.
		Serial.println("Disabling outputs");
		changeFlag = 0;
		delay(1500);
	}
}

void actuateJaws(int jawPos)
{
	if (jawPos < 3)
	{
		Serial.println(moveStage);
		if (jawPos == 0)
		{
			digitalWrite(inAPin, HIGH);
			Serial.println("Actuator is opening fully");
			digitalWrite(inBPin, LOW);
			analogWrite(pwmPin, 255);
			delay(7000);

			digitalWrite(inAPin, LOW);
			Serial.println("Actuator is adjusting opening width");
			digitalWrite(inBPin, HIGH);
			analogWrite(pwmPin, 255);
			delay(3500);
			Serial.print("jawPos: ");
			Serial.println(jawPos);
		}
		else if (jawPos == 1)
		{
			digitalWrite(inAPin, LOW);
			Serial.println("Actuator is closing around work piece");
			digitalWrite(inBPin, HIGH);
			analogWrite(pwmPin, 255);
			delay(2000);
		}
		else if (jawPos == 2)
		{
			digitalWrite(inAPin, HIGH);
			Serial.println("Actuator is opening to release work piece");
			digitalWrite(inBPin, LOW);
			analogWrite(pwmPin, 255);
			delay(1500);
		}
		digitalWrite(inAPin, LOW);
		Serial.println("Actuator is stopping");
		digitalWrite(inBPin, LOW);
		analogWrite(pwmPin, 255);
		delay(1500);
	}
}


#endif