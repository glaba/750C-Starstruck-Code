/** @file inputs.h
 * @brief Header file for keeping track of events and what triggers them
 */

#ifndef EVENTS_H_

// This prevents multiple inclusion
#define EVENTS_H_

#include <API.h>

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

// Useful joystick constants
#define PRESSED true
#define UNPRESSED false
#define PRIMARY_JOY 1
#define PARTNER_JOY 2

// Number of timed events
#define NUM_TIMED_EVENTS 10

// Typedef for the general event handler used in this program
typedef void (*eventHandlerP)(int*);
typedef void (*eventHandler)();

// Variable containing the up or down state for all joystick buttons; Array indices: joystick number, button group starting from 5, button type (top, bottom, etc)
extern bool joystickStates[2][4][4];

// Array of event handlers; Array indices: first three same as joystickStates, pressed / unpressed
extern eventHandlerP buttonEventHandlers[2][4][4][2];

// Array of events that must be called every 20 ms
extern eventHandler timedEventHandlers[NUM_TIMED_EVENTS];

// Array of parameters for the button events
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
