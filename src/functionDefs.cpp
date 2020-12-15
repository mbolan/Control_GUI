#ifndef FUNCTIONDEFS
#include "functionDefs.h"
#endif

#ifndef MOTORCONTROL
#include "MotorControl.h"
#endif


SnoozeDigital digitalWakeUp;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);//Declare a tft screen object to use for the display
SnoozeBlock config(digitalWakeUp);

float p = 3.141562654;
int selectedButton = 0;//Tells the program which button in a menu is supposed to be highlighted (0 being topmost)
volatile boolean changeFlag;//Flags when a change in menu state has been triggered by the user
volatile int changeButtonDir = 0;//Flags when the user is changing the direction of their menu navigation
int activeMenu = 0;//Records which menu is is being displayed
/*
    Main Menu: 0
    Run Menu: 1
    Edit Menu: 2
    Calibrate Menu: 3
*/
int activeFrame = 0;//Records which set of buttons is being displayed as the user navigates up or down a menu, eg. activeFrame = 0 for
//buttons 0, 1, 2; activeFrame = 1 for buttons 1, 2, 3, etc.
int menuHeight = 0;
int numButtDisplayed = 3;//How many buttons are displayed on a single frame
int *ptr = &numButtDisplayed;
boolean selectFlag = false;//Flags whether a button has been selected
boolean bottFlag = false;//Flag when the printMenu() function has reached the bottom button
boolean topFlag = false;//Flag when the printMenu() function has reached the top button

char menuName[200] = "";//Holds the text that is displayed at the top of each menu, not always the name though

int frameCounts[3] = {1, 2, 2};//Array holding the total number of frames for each menu

char *menuButt[200] = {
    "Run Programs",
    "Edit Programs",
    "Calibrate",
    "SHUT DOWN",
};

//Determines which button was pushed and then calls the appropriate menu regeneration function
void genMenu(int* actMenu, int* selButt, boolean* selFlag, int* actFrame)
{
    //If the green SELECT button was pushed, determine what screen to load
    if (*selFlag)
    {
        //If the user is in the main menu, any button they select will load a new menu. Determine which one to load
        if (*actMenu == 0)
        {
            //Load the Run Menu if the first button is highlighted, reset which button is selected
            if (*selButt == 0)
            {
                *actMenu = 1;
                *selButt = 0;
                tft.fillScreen(ST77XX_BLACK);
                //blankMenu(); //Reset the button names
                genRunMenu(*selButt);
                *selFlag = false;
            }
            //Load the Edit Menu if the first button is highlighted, reset which button is selected
            else if (*selButt == 1)
            {
                *actMenu = 2;
                *selButt = 0;
                tft.fillScreen(ST77XX_BLACK);
                //blankMenu(); //Reset the button names
                genEditMenu(selButt, &numButtDisplayed);
                *selFlag = false;
            }
            //Load the Calibrate Menu if the first button is highlighted, reset which button is selected
            else if (*selButt == 2)
            {
                *actMenu = 3;
                *selButt = 0;
                tft.fillScreen(ST77XX_BLACK);
                //blankMenu(); //Reset the button names
                genCalibrateMenu();
                *selFlag = false;
            }

        }
        //If the Run Menu is active, the user will either select a program to run or choose to back out to the Main Menu
        else if (*actMenu == 1)
        {
            if (*selButt == 0)
            {
                progNum = 0;
                runMoveProg();
            }
            else if (*selButt == 1)
            {
            }
            else if (*selButt == 2)
            {
            }
            else if (*selButt == 3)
            {
            }
            else if (*selButt == 4)
            {
            }
            //Back out to the Main Menu
            else if (*selButt == 5)
            {
                *actMenu = 0;
                *selButt = 0;
                numButtDisplayed = 3; //Reset the number of buttons to be displayed on the main menu
                tft.fillScreen(ST77XX_BLACK);
                blankMenu(); //Reset the button names
                *actFrame = 0;
                genMainMenu(*selButt);
                *selFlag = false;
            }
        }
        //If the Edit Menu is active, the user will either select a program to run or choose to back out to the Main Menu
        else if (*actMenu == 2)
        {
            if (*selButt == 0)
            {
            }
            else if (*selButt == 1)
            {
            }
            else if (*selButt == 2)
            {
            }
            else if (*selButt == 3)
            {
            }
            else if (*selButt == 4)
            {
            }
            //Back out to the Main Menu
            else if (*selButt == 5)
            {
                *actMenu = 0;
                *selButt = 0;
                numButtDisplayed = 3; //Reset the number of buttons to be displayed on the main menu
                tft.fillScreen(ST77XX_BLACK);
                blankMenu(); //Reset the button names
                *actFrame = 0;
                genMainMenu(*selButt);
                *selFlag = false;
            }
        }
        //If the Calibrate Menu is active, the user will either reset the positioning of the gantry or choose to back out to the Main Menu
        else if (*actMenu == 3)
        {
            if (*selButt == 0)
            {
            }
            //Back out to the Main Menu
            else if (*selButt == 1)
            {
                *actMenu = 0;
                *selButt = 0;
                numButtDisplayed = 3; //Reset the number of buttons to be displayed on the main menu
                tft.fillScreen(ST77XX_BLACK);
                blankMenu(); //Reset the button names
                *actFrame = 0;
                genMainMenu(*selButt);
                *selFlag = false;
            }
        }
    }
    //If the green SELECT button was not pushed, the user used one of the directional arrows to navigate up and down. Regenerate the active menu to
    //reflect that change
    else
    {
        if (*actMenu == 0)
        {
            genMainMenu(*selButt);
        }
        else if (*actMenu == 1)
        {
            genRunMenu(*selButt);
        }
        else if (*actMenu == 2)
        {
            genEditMenu(selButt, &numButtDisplayed);
        }
        else if (*actMenu == 3)
        {
            genCalibrateMenu();
        }
    }
}

//This function determines which button press triggered the interrupt that called it and and records the necessary menu changes to be 
//implemented by genMenu()
void readPushedButt()
{
    //This function will only progress if changeFlag has not been tripped already. Otherwise this function would be called multiple
    //times for each button press.
    if (!changeFlag)
    {
        //If the user pushed the up button, move the menu selection up and flag that a change has occurred
        if (digitalRead(upArrButt))
        {
            selectedButton--;
            changeFlag = true;
            changeButtonDir = -1;
        }
        //If the user pushed the down button, move the menu selection down and flag that a change has occurred
        else if (digitalRead(downArrButt))
        {
            selectedButton++;
            changeFlag = true;
            changeButtonDir = 1;
        }
        //Flag that the user wants to select an on-screen button and prevent the menu from moving up or down
        else if (digitalRead(greenButt))
        {
            changeFlag = true;
            selectFlag = true;
            changeButtonDir = 0;
        }
    }
}

//Contains the text of every button in each menu, called upon menu generation
char **getMenuText(int currMenu)
{
    if (currMenu == 0)
    {
        menuButt[0] = "Run Programs";
        menuButt[1] = "Edit Programs";
        menuButt[2] = "Calibrate";
    }
    else if (currMenu == 1)
    {
        menuButt[0] = "Program 1";
        menuButt[1] = "Program 2";
        menuButt[2] = "Program 3";
        menuButt[3] = "Program 4";
    }
    return menuButt;
}

void blankMenuButtons(int menuLen, int xCorner, int yCorner)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        tft.setCursor(xCorner, yCorner + (55 * i));
        tft.fillRoundRect(xCorner - 5, yCorner + (55 * i) - 30 + (menuHeight * 20), 220, 35, 5, ST77XX_BLACK);
    }
}

void blankMenu()
{
    for (int i = 0; i < 200; i++)
    {
        menuButt[i] = "";
    }
}

//Prints the active menu's title and buttons
void printMenu(int choseButt, int menuLength, int menuNum)
{
    int i;

    //Define the starting point of any menu
    int xCorner = 15;
    int yCorner = 50;

    //If the selected button is beyond the end of the visible frame, decide whether to increase the frame based on whether the selected button
    //exceeds the total number of buttons or not
    if (selectedButton == numButtDisplayed + activeFrame)
    {
        //If the selected button does not exceed the menu's length, move to a higher frame number
        if (numButtDisplayed + activeFrame < menuLength)
        {
            activeFrame++;
            selectFlag = true;
        }
    }
    //If the user has navigated above the given frame, decide whether to bump to a lower frame
    else if (selectedButton < activeFrame)
    {
        //As long as the base frame isn't the one currently displayed, bump to a lower frame
        if (activeFrame > 0)
        {
            activeFrame--;
            selectFlag = true;
        }
    }

    //If a new menu has been selected, erase the current one
    if (selectFlag)
    {
        blankMenuButtons(menuLength, xCorner, yCorner);
        selectFlag = false;
    }

    //Write the menu's name starting in the top left corner
    tft.setCursor(xCorner, 10);
    writeText(menuName, ST77XX_BLUE, 2);

    //Loop through the buttons, printing them where appropriate
    for (i = 0; i < numButtDisplayed; i += 1)
    {
        //If the selected button is already at the bottom of the screen and the user wants to move down, decide whether this is possible
        if (bottFlag & changeButtonDir == 1)
        {
            //If the selected button will exceed the length of this menu, reselect the last button in the menu for selectedButton
            if (selectedButton > menuLength - 1)
            {
                selectedButton = menuLength - 1;
            }
        }
        //If the selected button exceeds the upper bound of the active frame, decide whether it's possible to move up
        else if ((selectedButton < activeFrame & changeButtonDir == -1) & i == 0)
        {
            //If the selected button is less than the topmost allowed button, reselected the topmost button for selectedButton
            if (selectedButton < 0)
            {
                selectedButton = 0;
            }
        }
        //If the current iteration's button matches with the user's selected button, print a highlighted button to show that it is selected
        else if (i + activeFrame == selectedButton)
        {
            tft.fillRoundRect(xCorner - 5, yCorner - 20 + (55 * i) - 10 + (menuHeight * 20), 220, 35, 5, 0x96C6);

            //Flag whether this button is at the top of a frame, the bottom, or neither
            if (i + activeFrame == menuLength - 1)
            {
                bottFlag = true;
                topFlag = false;
            }
            else if (i == 0)
            {
                topFlag = true;
                bottFlag = false;
            }
            else
            {
                bottFlag = false;
                topFlag = false;
            }
        }
        //If the current iteration's button is above the selected button and the user is moving downwards, fill in this button with black
        else if (i + activeFrame == selectedButton - 1 & changeButtonDir == 1)
        {
            tft.fillRoundRect(xCorner - 5, yCorner + (55 * i) - 30 + (menuHeight * 20), 220, 35, 5, ST77XX_BLACK);
            //Serial.println("Filling button above with black");
        }
        //If the current iteration's button is below the selected button and the user is moving upwards, fill in this button with black
        else if (i + activeFrame == selectedButton + 1 & changeButtonDir == -1)
        {
            tft.fillRoundRect(xCorner - 5, yCorner + (55 * i) - 30 + (menuHeight * 20), 220, 35, 5, ST77XX_BLACK);
            //Serial.println("Filling button below with black");
        }

        //If the current iteration's button is within the acceptable length of the menu, print its text
        if (i + 1 + activeFrame <= frameCounts[menuNum] + 4)
        {
            tft.setCursor(xCorner, yCorner + (55 * i) + (menuHeight * 20) - 20);
            Serial.println(menuButt[i + activeFrame]);
            drawText(menuButt[i + activeFrame], ST77XX_BLUE, 2);
            testroundrects(xCorner - 10, yCorner + (55 * i) - 35 + (menuHeight * 20));
        }

        //If the menu is shorter than the number of buttons displayed, black out the upward indicator triangle
        if (menuLength <= numButtDisplayed)
        {
            tft.fillTriangle(220, 10, 205, 25, 235, 25, ST77XX_BLACK);
        }
        //If the selected button is at the top of the menu but the menu extends off-screen, add a downward indicator triangle
        else if (activeFrame == 0 & topFlag)
        {
            tft.fillTriangle(220, 235, 205, 220, 235, 220, ST77XX_WHITE);
            tft.fillTriangle(220, 10, 205, 25, 235, 25, ST77XX_BLACK);
        }
        //If the active frame is below the topmost frame and above the bottommost, display both the upward and downward indicator triangles
        else if (activeFrame > 0 & activeFrame < menuLength - numButtDisplayed)
        {
            tft.fillTriangle(220, 10, 205, 25, 235, 25, ST77XX_WHITE);
            tft.fillTriangle(220, 235, 205, 220, 235, 220, ST77XX_WHITE);
        }
        //If the bottommost frame is being displayed, display the upward indicator triangle
        else if (activeFrame == menuLength - numButtDisplayed)
        {
            tft.fillTriangle(220, 10, 205, 25, 235, 25, ST77XX_WHITE);
            tft.fillTriangle(220, 235, 205, 220, 235, 220, ST77XX_BLACK);
        }
    }
}

//Generates the Main Menu, specifying the length and title/button text. Calls the printMenu() function to display it
void genMainMenu(int choseButt)
{
    int mainMenuLength = 3;
    menuHeight = 1;
    strcpy(menuName, "Main Menu");

    menuButt[0] = "Run Programs";
    menuButt[1] = "Edit Programs";
    menuButt[2] = "Calibrate";

    printMenu(choseButt, mainMenuLength, 0);
}

//Generates the Run Menu, specifying the length and title/button text. Calls the printMenu() function to display it
void genRunMenu(int choseButt)
{
    int runMenuLength = 6;
    menuHeight = 1;
    numButtDisplayed = 3;
    strcpy(menuName, "Edit Programs");

    menuButt[0] = "Program 1";
    menuButt[1] = "Program 2";
    menuButt[2] = "Program 3";
    menuButt[3] = "Program 4";
    menuButt[4] = "Program 5";
    menuButt[5] = "BACK";

    printMenu(choseButt, runMenuLength, 1);
}
//Generates the Edit Menu, specifying the length and title/button text. Calls the printMenu() function to display it
void genEditMenu(int *choseButt, int *numButtDisplayed)
{
    int runMenuLength = 6;
    *numButtDisplayed = 3;
    menuHeight = 1;

    strcpy(menuName, "Edit Programs");

    menuButt[0] = "Program 1";
    menuButt[1] = "Program 2";
    menuButt[2] = "Program 3";
    menuButt[3] = "Program 4";
    menuButt[4] = "Program 5";
    menuButt[5] = "BACK";

    printMenu(*choseButt, runMenuLength, 2);
}

//Generates the Calibrate Menu, specifying the length and title/button text. Calls the printMenu() function to display it
void genCalibrateMenu()
{
    int xCorner = 5;
    int yCorner = 50;
    int calibrateMenuLength = 2;
    numButtDisplayed = 2;
    menuHeight = 4;
    strcpy(menuName, "Use the left and right arrow buttons to move gantry to starting position:");

    menuButt[0] = "Accept calibration";
    menuButt[1] = "BACK";
    menuButt[2] = "";
    menuButt[3] = "";
    menuButt[4] = "";
    menuButt[5] = "";

    printMenu(0, calibrateMenuLength, 3);
}

//selectButton is never called, consider deleting

//This function takes as input: the physical button the user pushed, the menu currently displayed, and the button highlighted on the menu.
//It evaluates whether to open a new menu, allow for program entry, run a program, etc.
void selectButton(int pushButt, int menu, int selButt)
{

    if (changeButtonDir == 0)
    {
    }
    else
    {
    }

    //Main menu
    if (menu == 0)
    {
        //If the green select button was pushed, execute the button highlighted on the menu
        //Otherwise, call a function to move move the selected menu button
        //Red button does nothing on this menu
        if (pushButt == 0)
        {
            genMainMenu(selButt);
        }
        else if (pushButt == 1)
        {
            genMainMenu(selButt);
        }
        else if (pushButt == 3)
        {
            menu += 1 + 1 * selButt;
            getMenuText(menu);
            tft.fillScreen(ST77XX_BLACK);
            genRunMenu(selButt);
        }
        else
        {
            genMainMenu(selButt);
        }
    }
    else if (menu == 0) //Run Program menu
    {
        //If the green select button was pushed, execute the button highlighted on the menu
        //Otherwise, call a function to move move the selected button
        //Red button does nothing on this menu
        if (pushButt == 0)
        {
            genRunMenu(selButt);
        }
        else if (pushButt == 1)
        {
            genRunMenu(selButt);
        }
        else if (pushButt == 3)
        {
            menu += 1 + 1 * selButt;
            getMenuText(menu);
            tft.fillScreen(ST77XX_BLACK);
            genRunMenu(selButt);
        }
        else
        {
            genMainMenu(selButt);
        }
    }
}

//Takes the appropriate size and colour and prints a line of text wherever the cursor is pointing
void drawText(char *text, uint16_t color, int textSize)
{

    tft.setTextSize(textSize);
    tft.setTextColor(color);
    tft.setTextWrap(true);
    tft.println(text);
}

//Writes wrapped text. Used for paragraph writing, bases line length on the 240 x 240 tft 
void writeText(char *text, uint16_t color, int textSize)
{
    int i = 0;
    int j = 0;
    int charCounter = 0;
    int wordLen = 0;
    int lineCharCounter = 0;
    int menuLen = strlen(text);

    tft.setTextSize(textSize);
    tft.setTextColor(color);


    for (i = 0; i < menuLen; i++)
    {

        //If a space is detected, determine whether to print it
        if (text[i] == ' ')
        {
            //If the space is at the end of a line, skip over it
            if (i % 19 == 1)
            {
                charCounter++;
            }
            //If the space is not at the start of a line, print it
            else if (i % 19 != 0)
            {
                tft.print(text[i]);
                charCounter++;
                lineCharCounter++;
            }
            else
            {
                tft.print('\n');
                lineCharCounter = 0;
            }
        }
        //If the character is a null character, end the print loop
        else if (text[i] == '\0')
        {
            tft.println(text[charCounter]); //Print the final character
            i = menuLen;                    //Exit the loop
        }
        else
        {
            //Iterate through a word to determine its length
            while (text[i + j] != ' ' & text[i + j] != '\0')
            {
                wordLen++;
                j++;
            }

            //If the word will not spill over the line character limit, print it
            if (wordLen < (19 - (lineCharCounter % 19)) & (lineCharCounter % 19) != 0)
            {
                tft.print(text[i]);
                lineCharCounter++;
            }
            //If nothing has been printed on this line yet, print the word
            else if (lineCharCounter == 0)
            {
                tft.print(text[i]);
                lineCharCounter++;
            }
            //If the word won't fit on the line, start a new line and print it
            else
            {
                tft.print('\n');
                tft.print(text[i]);
                lineCharCounter = 0;
            }
            wordLen = 0;
            j = 0;
        }
    }
}

void testroundrects(int x, int y)
{
    uint16_t color = 100;
    int i;
    int t;
    int w = 230;
    int h = 45;
    for (i = 0; i <= 4; i += 1)
    {
        tft.drawRoundRect(x, y, w, h, 5, ST77XX_BLUE);
        x += 1;
        y += 1;
        w -= 2;
        h -= 2;
    }
}

void drawUnderline(int xCorn, int yCorn, int strLength, uint16_t col)
{
    int i;
    //A character in size 2 font is ~11.67 pixels wide, on average. Multiply number of characters by
    //12 to generate underline
    for (i = 0; i < 4; i++)
    {
        tft.drawLine(xCorn - 3, yCorn + 20 + 1 * i, xCorn + 12 * strLength, yCorn + 20 + 1 * i, ST77XX_RED);
    }
}
