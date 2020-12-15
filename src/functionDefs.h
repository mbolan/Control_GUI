#ifndef FUNCTIONDEFS_H
#define FUNCTIONDEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <array>
#include <iostream>
#include <Snooze.h>

#define TFT_CS 10
#define TFT_RST 14
#define TFT_DC 8

//Assign the pins for the control panel buttons
#define upArrButt 21
#define rightArrButt 18
#define downArrButt 20
#define leftArrButt 19
#define greenButt 17
#define redButt 16


extern SnoozeDigital digitalWakeUp;

extern Adafruit_ST7789 tft;//Declare a tft screen object to use for the display
extern SnoozeBlock config;

extern float p;
extern int selectedButton;//Tells the program which button in a menu is supposed to be highlighted (0 being topmost)
extern volatile boolean changeFlag;//Flags when a change in menu state has been triggered by the user
extern volatile int changeButtonDir;//Flags when the user is changing the direction of their menu navigation
extern int activeMenu;//Records which menu is is being displayed
extern int activeFrame;//Records which set of buttons is being displayed as the user navigates up or down a menu, eg. activeFrame = 0 for
//buttons 0, 1, 2; activeFrame = 1 for buttons 1, 2, 3, etc.
extern int menuHeight;
extern int numButtDisplayed;//How many buttons are displayed on a single frame
extern int *ptr;
extern boolean selectFlag;//Flags whether a button has been selected
extern boolean bottFlag;//Flag when the printMenu() function has reached the bottom button
extern boolean topFlag;//Flag when the printMenu() function has reached the top button

extern int frameCounts[];//Array holding the total number of frames for each menu
extern char menuName[];//Holds the text that is displayed at the top of each menu, not always the name though
extern char *menuButt[];


void drawText(char *text, uint16_t color, int textSize);
void testroundrects(int x, int y);
void genMenu(int* actMenu, int* selButt, boolean* selFlag, int* actFrame);
void selectButton(int selButt, int menu, int pushButt);
char **getMenuText(int currMenu);
void genMainMenu(int choseButt);
void genRunMenu(int choseButt);
void drawUnderline(int xCorn, int yCorn, int strLength, uint16_t col);
void readPushedButt();
void printMenu(int choseButt, int menuLength, int menuNum);
void blankMenuButtons(int menuLen, int xCorner, int yCorner);
void genEditMenu(int* choseButt, int* numButtDisplayed);
void runProgram(int progNum);
void editProgram(int progNum);
void genCalibrateMenu();
void blankMenu();
void writeText(char *text, uint16_t color, int textSize);

#endif