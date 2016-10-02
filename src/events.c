/** @file events.c
 * @brief File for button event handlers
 *
 * This file contains event handlers for buttons being pressed and released on the joystick and on the robot as well as timed events
 *
 */

#include "main.h"
#include "events.h"
#include "robot.h"

// Array of event handlers; Array indices: first three same as joystickStates, pressed / unpressed
eventHandlerP buttonEventHandlers[2][4][4][2];

// Array of parameters for the button events
int *buttonEventHandlerParameters[2][4][4][2];

// Array of events that must be called every 20 ms
eventHandler timedEventHandlers[NUM_TIMED_EVENTS];

// Sets an event handler for a joystick button press
void setEventHandler(eventHandlerP handler, int *parameters, int joystick, int buttonGroup, int button, int pressedState) {
	buttonEventHandlers[joystick - 1][buttonGroup - 5][button - 1][pressedState] = handler;
	buttonEventHandlerParameters[joystick - 1][buttonGroup - 5][button - 1][pressedState] = parameters;
}

// Adds an event to the list of functions set to be run every 20 ms
void addTimedEvent(eventHandler handler) {
	int nextIndex = 0;
	while (nextIndex < NUM_TIMED_EVENTS && timedEventHandlers[nextIndex])
		nextIndex++;

	if (nextIndex < NUM_TIMED_EVENTS) {
		timedEventHandlers[nextIndex] = handler;
	}
}

// Initializes event handlers based on which functions are needed and defined
void initializeEventHandlers() {
	// Initially set all event handlers to NULL
	for (int joystick = 1; joystick <= 2; joystick++) {
		for (int buttonGroup = 5; buttonGroup <= 8; buttonGroup++) {
			for (int button = 1; button <= 4; button++) {
				for (int pressedState = 0; pressedState <= 1; pressedState++) {
					buttonEventHandlers[joystick - 1][buttonGroup - 5][button - 1][pressedState] = NULL;
				}
			}
		}
	}
	for (int i = 0; i < NUM_TIMED_EVENTS; i++) {
		timedEventHandlers[i] = NULL;
	}

	// Set values of defined button event handlers
	int forward[3] = {127, 0, 0};
	int stop[3] = {0, 0, 0};
	setEventHandler(moveRobot, forward, PRIMARY_JOY, 5, JOY_UP, PRESSED);
	setEventHandler(moveRobot, stop, PRIMARY_JOY, 5, JOY_UP, UNPRESSED);

	// Set values of timed event handlers
	addTimedEvent(moveRobotFromJoystick);
}
