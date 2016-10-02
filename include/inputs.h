/** @file inputs.h
 * @brief Header file for keeping track of events and what triggers them
 */

#ifndef INPUTS_H_

// This prevents multiple inclusion
#define INPUTS_H_

#include <API.h>

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

// Variable containing the up or down state for all joystick buttons; Array indices: joystick number, button group starting from 5, button type (top, bottom, etc)
extern bool joystickStates[2][4][4];

// Array of event handlers; Array indices: first three same as joystickStates, pressed / unpressed
extern void (*buttonEventHandlers[2][4][4][2]) (int*);

// Array of parameters for the button event handlers
extern int *buttonEventHandlerParameters[2][4][4][2];

// Initializes event handlers based on which functions are needed and defined
void initializeEventHandlers();

// Updates joystickStates based on current value and calls events if the values change
void updateJoystickStates();

// End C++ export structure
#ifdef __cplusplus
}
#endif

#endif
