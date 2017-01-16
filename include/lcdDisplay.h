/** @file lcdDisplay.h
 * @brief File for LCD menu functions and constants
 */

#ifndef LCD_DISPLAY_H

// This prevents multiple inclusion
#define LCD_DISPLAY_H

#include <API.h>

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Definition for the port that the LCD display is plugged into
 */
#define LCD_PORT uart1

/**
 * Defines the maximum length of a message printed on one line of the LCD display
 */
#define LCD_MESSAGE_MAX_LENGTH 20

/**
 * A struct that defines an item in the LCD navigation menu
 */
typedef struct menu_item {
	/**
	 * If true, the menu has no submenus and calls the function getOutput to fill the lines instead. If false, the menu has submenus and its name and description are provided in this struct.
	 */
	int isFunction;

	/**
	 * The name of the LCD menu item
	 */
	char name[LCD_MESSAGE_MAX_LENGTH - 2 + 1];

	/**
	 * A short description of the LCD menu item
	 */
	char description[LCD_MESSAGE_MAX_LENGTH + 1];

	/**
	 * The number of children
	 */
	int numChildren;

	/**
	 * The number of items in the parent menu
	 */
	int numParents;

	/**
	 * The children menus of the current menu item
	 */
	struct menu_item** children;

	/**
	 * A reference to the parent menu
	 */
	struct menu_item* parent;

	/**
	 * A function that will run when the button is pressed
	 *
	 * @param int The index of the menu item that triggered the function call
	 */
	void (*runFunction)(int);
} menu_item;

/**
 * Initializes the menus used in this program
 */
void initLCDMenu();

/**
 * Updates the LCD menu based on button inputs and changing conditions
 */
void updateLCDMenu(int dt);

#ifdef __cplusplus
}
#endif

#endif
