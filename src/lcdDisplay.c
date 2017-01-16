/** @file lcdDisplay.c
 * @brief File for LCD display functons and constants
 *
 * Contains functions and variables that describe the current LCD state and draw new portions of the LCD menu 
 *
 */

#include "main.h"

/**
 * The first level of menu items that show up on the LCD after the splash screen
 */ 
menu_item* initialMenuItems;

/**
 * The amount of milliseconds that the menu has been inactive (after 5000 ms it goes to splash screen)
 */
int menuTimeout = 0;

/**
 * The a pointer to the first menu in the layer of menus currently displaying on the LCD screen
 */ 
menu_item* currentMenus;

/**
 * The index of the current menu in the currentMenus array
 */ 
int currentMenuIndex = 0;

/**
 * The number of items in the current level of menus
 */ 
int numMenuItems;

/**
 * The previous value for the Left LCD button 
 */
int prevLCDLeft = 0;

/**
 * The previous value for the Right LCD button 
 */
int prevLCDRight = 0;

/**
 * The previous value for the Center LCD button 
 */
int prevLCDCenter = 0;


/**
 * Shows battery information for the primary and secondary battery
 *
 * @param index Dummy parameter for the lcdDisplay menu
 */
void showBatteryInfo(int index) {
	double primaryBatt = powerLevelMain() / 1000.0;
	double secondaryBatt = powerLevelExpander() / 1000.0;
	lcdPrint(LCD_PORT, 1, "Main: %f", primaryBatt);
	lcdPrint(LCD_PORT, 2, "Expander: %f", secondaryBatt);
}

/**
 * Wrapper for the recordAuton function that has an int parameter
 *
 * @param index Dummy parameter for the lcdDisplay menu
 */
void recordAutonWrapper(int index) {
	recordAuton();
	saveAuton();
}

/**
 * Runs a motor until the center LCD button is pressed
 *
 * @param index The index of the motor port to run (starting from 0)
 */
void runMotorUntilPress(int index) {
	char lineOne[16];
	snprintf(lineOne, 16, "Running port %d", index + 1);
	lcdSetText(LCD_PORT, 1, lineOne);
	lcdSetText(LCD_PORT, 2, "Speed: 127");

	motorSet(index, 127);
	
	while (lcdReadButtons(LCD_PORT) == 0) {
		delay(20)
	}

	motorSet(index, 0);
}

/**
 * Loads and runs an auton (running no auton is not possible)
 *
 * @param index Dummy parameter for the lcdDisplay menu
 */
void lcdPlaybackAuton(int index) {
	lcdSetText(LCD_PORT, 1, "Load from?");
	loadAuton(selectAuton());
	playbackAuton(autonFlipped);
}

/**
 * Initializes the menus used in this program
 */
void initLCDMenu() {
	initialMenuItems = (menu_item*) malloc(4 * sizeof(menu_item));

	menu_item batteryMenu, motorTest, recordAuton, loadAuton;
	menu_item* motorTestMenus;

	motorTestMenus = malloc(10 * sizeof(menu_item));
	for (int i = 0; i < 10; i++) {
		char name[8];
		snprintf(name, 8, "Port %d", i + 1);

		motorTestMenus[i] = (menu_item) { .isFunction = true, .name = name, .description = "", .numChildren = 0, .children = 0, .numParents = 4, .parent = &motorTest, .runFunction = &runMotorUntilPress };
	}

	batteryMenu = (menu_item) { .isFunction = true, .name = "Battery Info", .description = "", .numChildren = 0, .children = 0, .numParents = 0, .parent = 0, .runFunction = showBatteryInfo };
	motorTest = (menu_item) { .isFunction = false, .name = "Motor Testing", .description = "Run chosen motor(s)", .numChildren = 10, .children = motorTestMenus, .numParents = 0, .parent = 0, .runFunction = 0 };
	recordAuton = (menu_item) { .isFunction = true, .name = "Record auton", .description = "", .numChildren = 0, .children = 0, .numParents = 0, .parent = 0, .runFunction = recordAutonWrapper };
	loadAuton = (menu_item) { .isFunction = false, .name = "Playback auton", .description = "", .numChildren = 0, .children = 0, .numParents = 0, .parent = 0, .runFunction = lcdPlaybackAuton };

	initialMenuItems[0] = batteryMenu;
	initialMenuItems[1] = motorTest;
	initialMenuItems[2] = recordAuton;
	initialMenuItems[3] = loadAuton;

	currentMenu = initialMenuItems;
	numMenuItems = 4;
}

/**
 * Updates the LCD menu based on button inputs and changing conditions
 *
 * @param dt The amount of time that has passed since the function was last called
 */
void updateLCDMenu(int dt) {
	if (lcdReadButtons(LCD_PORT) & LCD_BTN_CENTER != 0 && prevLCDCenter == 0) {
		if (currentMenus[currentMenuIndex].isFunction) {
			currentMenus[currentMenuIndex].runFunction();
		} else {
			prevNumMenuItems = numMenuItems;
			numMenuItems = currentMenus[currentMenuIndex].numChildren;
			currentMenus = currentMenus[currentMenuIndex].children;
			currentMenuIndex = 0;
		}
	} else if (lcdReadButtons(LCD_PORT) & LCD_BTN_RIGHT != 0 && prevLCDRight == 0) {
		if (currentMenuIndex == numMenuItems - 1) {
			if (currentMenus[currentMenuIndex].parent != 0) {
				numMenuItems = currentMenus[currentMenuIndex].numParents;
				currentMenus = currentMenus[currentMenuIndex].parent;
				currentMenuIndex = 0;
			} else {
				menuTimeout = 5000;
			}
		} else {
			currentMenuIndex++;
		}
	} else if (lcdReadButtons(LCD_PORT) & LCD_BTN_LEFT != 0 && prevLCDLeft == 0) {
		if (currentMenuIndex == 0) {
			if (currentMenus[currentMenuIndex].parent != 0) {
				numMenuItems = currentMenus[currentMenuIndex].numParents;
				currentMenus = currentMenus[currentMenuIndex].parent;
				currentMenuIndex = 0;
			} else {
				menuTimeout = 5000;
			}
		} else {
			currentMenuIndex--;
		}
	}

	menu_item curMenu = currentMenus[currentMenuIndex];

	char firstChar = '<';
	char secondChar = '>';
	if (currentMenuIndex == 0 && curMenu.parent != 0) {
		firstChar = '^';
	} 
	if (currentMenuIndex == numMenuItems && curMenu.parent != 0) {
		secondChar = '^';
	}

	int line1Padding1 = (18 - strlen(curMenu.name)) / 2 + ((18 - strlen(curMenu.name)) % 2 != 0);
	int line1Padding2 = (18 - strlen(curMenu.name)) / 2; 
	lcdPrint(LCD_PORT, 1, "%c%*s%s%*s%c", firstChar, line1Padding1, "", curMenu.name, line1Padding2, "", secondChar);
	lcdSetText(LCD_PORT, 2, curMenu.description);

	prevLCDCenter = lcdReadButtons(LCD_PORT) & LCD_BTN_CENTER;
	prevLCDRight = lcdReadButtons(LCD_PORT) & LCD_BTN_RIGHT;
	prevLCDLeft = lcdReadButtons(LCD_PORT) & LCD_BTN_LEFT;

	// Finish when splash screen is implemented
	//menuTimeout += dt;
}