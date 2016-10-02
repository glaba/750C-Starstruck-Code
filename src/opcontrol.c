/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"
#include "events.h"

// Variable containing the up or down state for all joystick buttons; Array indices: joystick number, button group starting from 5, button type (top, bottom, etc)
bool joystickStates[2][4][4];

// Initializes joystickStates to unpressed by default
void initializeJoystickStates() {
	for (int joystick = 1; joystick <= 2; joystick++) {
		for (int buttonGroup = 5; buttonGroup <= 8; buttonGroup++) {
			for (int button = 1; button <= 4; button++) {
				joystickStates[joystick - 1][buttonGroup - 5][button - 1] = false;
			}
		}
	}
}

// Updates joystickStates based on current value and calls events if the values change
void updateJoystickStates() {
	for (int joystick = 1; joystick <= 2; joystick++) {
		for (int buttonGroup = 5; buttonGroup <= 8; buttonGroup++) {
			for (int button = 1; button <= 4; button++) {
				bool curValue = joystickGetDigital(joystick, buttonGroup, button);
				if (joystickStates[joystick - 1][buttonGroup - 5][button - 1] != curValue) {
					// Call event handler
					eventHandlerP curHandler = buttonEventHandlers[joystick - 1][buttonGroup - 5][button - 1][curValue];
					if (curHandler)
						curHandler(buttonEventHandlerParameters[joystick - 1][buttonGroup - 5][button - 1][curValue]);

					// Update joystick state value
					joystickStates[joystick - 1][buttonGroup - 5][button - 1] = curValue;
				}
			}
		}
	}
}

// Runs all the timed events
void runTimedEvents() {
	for (int i = 0; i < NUM_TIMED_EVENTS; i++) {
		if (timedEventHandlers[i])
			timedEventHandlers[i]();
		else
			return;
	}
}

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
void operatorControl() {
	initializeJoystickStates();

	while (true) {
		updateJoystickStates();
		runTimedEvents();
		delay(20);
	}
}
