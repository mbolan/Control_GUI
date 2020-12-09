#include "functionDefs.h"
#include "MotorControl.h"


void setup(void)
{
    Serial.begin(9600);

    pinMode(upArrButt, INPUT);
    pinMode(downArrButt, INPUT);

    digitalWakeUp.pinMode(greenButt, INPUT, RISING);

    tft.init(240, 240);

    uint16_t time = millis();
    tft.fillScreen(ST77XX_BLACK);
    time = millis() - time;

    Serial.println(time, DEC);
    delay(500);

    int activeMenuButton = 0; //The button currently highlighted on the GUI
    int pushedButton = 2;     //The button pushed on the control panel by the user

    //selectButton(pushedButton, activeMenu, activeMenuButton);
    genMenu(&activeMenu, &selectedButton, &selectFlag, &activeFrame);
    attachInterrupt(upArrButt, readPushedButt, HIGH);
    attachInterrupt(downArrButt, readPushedButt, HIGH);
    attachInterrupt(greenButt, readPushedButt, HIGH);


    //Motor control block:
    stepper.setEnablePin(0);
	stepper.enableOutputs(); //These stepper drivers (TB6600) are enabled with EN+ low. enableOutputs() drives the signal high,
	//so you must invert the function to achieve the desired results. Here we want to disable the stepper motors.
	Serial.begin(9600);
	stepper.setMaxSpeed(maxSpd);
	stepper.setSpeed(targetSpeed);
	//stepper.move(targetDist);
	stepper.setAcceleration(targetAccel);

	pinMode(inAPin, OUTPUT);
	pinMode(inBPin, OUTPUT);

	pinMode(inputButton, INPUT);
}

void loop()
{
    int j;

    //If the gantry has been flagged to move, call the moveGantry() function to execute the step
    if (moveState)
	{
		moveGantry();
	}
    //If a change in menu state has been flagged, call the genMenu() function to determine and execute what changed
    else if (changeFlag)
    {
        genMenu(&activeMenu, &selectedButton, &selectFlag, &activeFrame);
        changeFlag = false;//Flag that the menu change has been executed
        delay(250);//Delay to prevent the function from being called multiple times from a single button press
    }
}







void runProgram(int progNum)
{
}

void editProgram(int progNum)
{
}







