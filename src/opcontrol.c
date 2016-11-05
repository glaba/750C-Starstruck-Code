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
#include "robot.h"
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

#define FRONT_LEFT_MOTOR 1
#define FRONT_RIGHT_MOTOR 2
#define BACK_LEFT_MOTOR 3
#define BACK_RIGHT_MOTOR 4

#define LIFT_BOTTOM_RIGHT_MOTOR 5
#define LIFT_TOP_RIGHT_MOTOR 6
#define LIFT_BOTTOM_LEFT_MOTOR 7
#define LIFT_TOP_LEFT_MOTOR 8

#define SHOOTER_BACK_LEFT_MOTOR 9
#define SHOOTER_FRONT_LEFT_MOTOR 10
#define SHOOTER_BACK_RIGHT_MOTOR 11
#define SHOOTER_FRONT_RIGHT_MOTOR 12

#define MOTOR_SPEED 127

void moveRobot() {
	int forward = joystickGetAnalog(1, 3);
	int right = joystickGetAnalog(1, 4);
	int turn = joystickGetAnalog(1, 1);

	motorSet(FRONT_LEFT_MOTOR, forward + right + turn);
	motorSet(FRONT_RIGHT_MOTOR, -forward + right + turn);
	motorSet(BACK_LEFT_MOTOR, forward - right + turn);
	motorSet(BACK_RIGHT_MOTOR, -forward - right + turn);
}

void moveLift() {
	bool upPressed = joystickGetDigital(1, 6, JOY_UP);
	bool downPressed =  joystickGetDigital(1, 6, JOY_DOWN);

	if (upPressed) {
		setLiftMotors(-1);
	} else if (downPressed) {
		setLiftMotors(1);
	} else {
		setLiftMotors(0);
	}
}

void moveShooter(){
	bool upPressed = joystickGetDigital(1, 7, JOY_UP);
	bool downPressed = joystickGetDigital(1, 7, JOY_DOWN);

	if (upPressed){
		setShooterMotors(1);
	} else if (downPressed){
		setShooterMotors(-1);
	}
}
void operatorControl() {
	while (1) {
		moveRobot();
		moveLift();
		moveShooter();
		delay(20);
	}
}
