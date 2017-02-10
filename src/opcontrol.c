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

/**
 * Forward motion
 */
int spd;
/**
 * Horizontal motion
 */
 int horizontal;
/**
 * CW rotational motion
 */
int turn;
/**
 * Motion of the pincer
 */
int sht;
/**
 * lift motion
 */
int lift;

bool isLocked = false;

/**
 * Records joystick information into global variables for auton recorder and for robot motion
 */
void recordJoyInfo() {
	spd = joystickGetAnalog(1, 3);
	horizontal = pow(joystickGetAnalog(1, 4) / 127.0, 4) * 127 * joystickGetAnalog(1, 4) / abs(joystickGetAnalog(1, 4));
	turn = joystickGetAnalog(1, 1);

  spd = IGNORE_LOW_VAL(spd);
  horizontal = IGNORE_LOW_VAL(horizontal);
  turn = IGNORE_LOW_VAL(turn);

	if (joystickGetDigital(1, 5, JOY_UP) == true || joystickGetDigital(2, 5, JOY_UP) == true) {
		sht = 127;
	} else if (joystickGetDigital(1, 5, JOY_DOWN) == true || joystickGetDigital(2, 5, JOY_DOWN) == true) {
		sht = -127;
	} else if(joystickGetDigital(1, 8, JOY_DOWN) == true ||joystickGetDigital(2, 8, JOY_DOWN) == true) {
		sht = 27;
  } else if(joystickGetDigital(1, 8, JOY_UP) == true || joystickGetDigital(2, 8, JOY_UP) == true) {
    sht = -27;
  } else {
		sht = 0;
	}

	if (joystickGetDigital(1, 6, JOY_UP) == true || joystickGetDigital(2, 6, JOY_UP) == true) {
		lift = -1;
	} else if (joystickGetDigital(1, 6, JOY_DOWN) == true || joystickGetDigital(2, 6, JOY_DOWN) == true) {
		lift = 1;
	} else {
		lift = 0;
	}
}

/**
 * Move robot based on collected joystick information or based on replayed information from auton recorder
 */
void moveRobot() {
	setLiftMotors(lift);
	setPincerMotors(sht);
	setDriveMotors(spd, horizontal, turn);
}

/**
 * Runs the operator control loop
 */
void operatorControl() {
	while (1) {
		if (joystickGetDigital(1, 7, JOY_RIGHT) && !isOnline()) {
			recordAuton();
			saveAuton();
		}
		if (joystickGetDigital(1, 7, JOY_LEFT)) {
			loadAuton(selectAuton(false));
			playbackAuton();
		}
		updateLCDMenu(20);
		recordJoyInfo();
		moveRobot();
		delay(20);
	}
}
