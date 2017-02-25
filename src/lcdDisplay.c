/** @file lcdDisplay.c
 * @brief File for LCD display functons and constants
 *
 * Contains functions and variables that describe the current LCD state and draw new portions of the LCD menu
 *
 */

#include "main.h"
#include <string.h>

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
	delay(500);

	while (lcdReadButtons(LCD_PORT) == 0) {
		double primaryBatt = powerLevelMain() / 1000.0;
		//double secondaryBatt = powerLevelExpander() / 1000.0;
		lcdPrint(LCD_PORT, 1, "Main: %f", primaryBatt);
		lcdSetText(LCD_PORT, 2, "");
		//lcdPrint(LCD_PORT, 2, "Expander: %f", secondaryBatt);
		delay(20);
	}
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
 * Runs a motor until an LCD button is pressed
 *
 * @param index The index of the motor port to run (starting from 0)
 */
void runMotorUntilPress(int index) {
	delay(500);

	char lineOne[16];
	snprintf(lineOne, 16, "Running port %d", index + 1);
	lcdSetText(LCD_PORT, 1, lineOne);
	lcdSetText(LCD_PORT, 2, "Speed: 127");

	motorSet(index + 1, 127);

	while (lcdReadButtons(LCD_PORT) == 0) {
		delay(20);
	}

	motorSet(index, 0);
}

/**
 * Loads and runs an auton (running no auton is not possible)
 *
 * @param index Dummy parameter for the lcdDisplay menu
 */
void lcdPlaybackAuton(int index) {
	delay(500);

	lcdSetText(LCD_PORT, 1, "Load from?");
	loadAuton(selectAuton(false));
	playbackAuton();
}

/**
 * Downloads an autonomous routine from the computer and prompts the user on which slot to save it to
 */
void downloadAutonFromComputerWrapper() {
	delay(500);
	downloadAutonFromComputer(selectAuton(true));
}

/**
 * Uploads an autonomous routine to the computer and prompts the user on which slot to upload from
 */
void uploadAutonToComputerWrapper() {
	delay(500);
	uploadAutonToComputer(selectAuton(true));
}

/**
 * Initializes the menus used in this program
 */
void initLCDMenu() {
	initialMenuItems = (menu_item*) malloc(6 * sizeof(menu_item));

	menu_item* motorTestMenus;
	motorTestMenus = malloc(10 * sizeof(menu_item));

	menu_item batteryMenu = { .isFunction = true, .name = "Battery Info", .description = "", .numChildren = 0, .children = 0, .numParents = 0, .parentIndex = 0, .parent = 0, .runFunction = &showBatteryInfo };
	menu_item motorTest = { .isFunction = false, .name = "Motor Testing", .description = "Run chosen motor(s)", .numChildren = 10, .children = motorTestMenus, .numParents = 0, .parentIndex = 0, .parent = 0, .runFunction = 0 };
	menu_item recordAuton = { .isFunction = true, .name = "Record Auton", .description = "", .numChildren = 0, .children = 0, .numParents = 0, .parentIndex = 0, .parent = 0, .runFunction = &recordAutonWrapper };
	menu_item loadAuton = { .isFunction = true, .name = "Playback Auton", .description = "", .numChildren = 0, .children = 0, .numParents = 0, .parentIndex = 0, .parent = 0, .runFunction = &lcdPlaybackAuton };
	menu_item downloadAuton = { .isFunction = true, .name = "Download Auton", .description = "Load from computer", .numChildren = 0, .children = 0, .numParents = 0, .parentIndex = 0, .parent = 0, .runFunction = &downloadAutonFromComputerWrapper };
	menu_item uploadAuton = { .isFunction = true, .name = "Upload Auton", .description = "Save to computer", .numChildren = 0, .children = 0, .numParents = 0, .parentIndex = 0, .parent = 0, .runFunction = &uploadAutonToComputerWrapper };
	
	for (int i = 0; i < 10; i++) {
		char* name = malloc((LCD_MESSAGE_MAX_LENGTH - 2 + 1) * sizeof(char));
		snprintf(name, LCD_MESSAGE_MAX_LENGTH - 2 + 1, "Port %d", i + 1);

		menu_item curMotorTestMenu = { .isFunction = true, .name = name, .description = "", .numChildren = 0, .children = 0, .numParents = 4, .parentIndex = 1, .parent = initialMenuItems, .runFunction = &runMotorUntilPress };
		motorTestMenus[i] = curMotorTestMenu;
	}

	initialMenuItems[0] = batteryMenu;
	initialMenuItems[1] = motorTest;
	initialMenuItems[2] = recordAuton;
	initialMenuItems[3] = loadAuton;
	initialMenuItems[4] = downloadAuton;
	initialMenuItems[5] = uploadAuton;

	currentMenus = initialMenuItems;
	numMenuItems = 6;
}

/**
 * Updates the LCD menu based on button inputs and changing conditions
 *
 * @param dt The amount of time that has passed since the function was last called
 */
void updateLCDMenu(int dt) {
	if (((lcdReadButtons(LCD_PORT) & LCD_BTN_CENTER) != 0) && prevLCDCenter == 0) {
		if (currentMenus[currentMenuIndex].isFunction) {
			currentMenus[currentMenuIndex].runFunction(currentMenuIndex);
		} else {
			numMenuItems = currentMenus[currentMenuIndex].numChildren;
			currentMenus = currentMenus[currentMenuIndex].children;
			currentMenuIndex = 0;
		}
	} else if (((lcdReadButtons(LCD_PORT) & LCD_BTN_RIGHT) != 0) && prevLCDRight == 0) {
		if (currentMenuIndex == numMenuItems - 1) {
			if (currentMenus[currentMenuIndex].parent != 0) {
				numMenuItems = currentMenus[currentMenuIndex].numParents;
				int newIndex = currentMenus[currentMenuIndex].parentIndex;
				currentMenus = currentMenus[currentMenuIndex].parent;
				currentMenuIndex = newIndex;
			} else {
				menuTimeout = 5000;
			}
		} else {
			currentMenuIndex++;
		}
	} else if (((lcdReadButtons(LCD_PORT) & LCD_BTN_LEFT) != 0) && prevLCDLeft == 0) {
		if (currentMenuIndex == 0) {
			if (currentMenus[currentMenuIndex].parent != 0) {
				numMenuItems = currentMenus[currentMenuIndex].numParents;
				int newIndex = currentMenus[currentMenuIndex].parentIndex;
				currentMenus = currentMenus[currentMenuIndex].parent;
				currentMenuIndex = newIndex;
			} else {
				menuTimeout = 5000;
			}
		} else {
			currentMenuIndex--;
		}
	}

	menu_item curMenu = currentMenus[currentMenuIndex];

	int firstChar = 247; // Left arrow
	int secondChar = 246; // Right arrow
	if ((currentMenuIndex == 0) && (curMenu.parent != 0)) {
		firstChar = 197; // Up arrow
	} else if ((currentMenuIndex == 0) && curMenu.parent == 0) {
		firstChar = ' ';
	}
	if ((currentMenuIndex == numMenuItems - 1) && (curMenu.parent != 0)) {
		secondChar = 197; // Up arrow
	} else if ((currentMenuIndex == numMenuItems - 1) && (curMenu.parent == 0)) {
		secondChar = ' ';
	}

	int totalNumSpaces = LCD_MESSAGE_MAX_LENGTH - 2 - strlen(curMenu.name);
	int line1Padding1 = totalNumSpaces / 2 + (totalNumSpaces % 2 != 0);

	char output[LCD_MESSAGE_MAX_LENGTH + 1];
	memset(output, ' ', LCD_MESSAGE_MAX_LENGTH);
	output[LCD_MESSAGE_MAX_LENGTH] = 0;
	output[0] = firstChar;
	output[LCD_MESSAGE_MAX_LENGTH - 1] = secondChar;

	for (int i = 0; i < strlen(curMenu.name); i++) {
		output[1 + line1Padding1 + i] = curMenu.name[i];
	}

	lcdSetText(LCD_PORT, 1, output);
	lcdSetText(LCD_PORT, 2, curMenu.description);

	prevLCDCenter = lcdReadButtons(LCD_PORT) & LCD_BTN_CENTER;
	prevLCDRight = lcdReadButtons(LCD_PORT) & LCD_BTN_RIGHT;
	prevLCDLeft = lcdReadButtons(LCD_PORT) & LCD_BTN_LEFT;

	// Finish when splash screen is implemented
	//menuTimeout += dt;
}
