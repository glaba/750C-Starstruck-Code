/** @file robot.h
 * @brief Header file for motor port definitions, sensor port definitions, and basic movement functions
 */

#ifndef ROBOT_H_

// This prevents multiple inclusion
#define ROBOT_H_

#include <API.h>

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

// Motor port definitions
#define FRONT_LEFT_MOTOR 0
#define FRONT_RIGHT_MOTOR 1
#define BACK_LEFT_MOTOR 2
#define BACK_RIGHT_MOTOR 3

// Drive motion function: motion is an array of: forwards, right, clockwise from -127 to 127
void moveRobot(int* motion) {
	motorSet(FRONT_LEFT_MOTOR, motion[0] + motion[1] + motion[2]);
	motorSet(FRONT_RIGHT_MOTOR, -motion[0] + motion[1] + motion[2]);
	motorSet(BACK_LEFT_MOTOR, motion[0] - motion[1] + motion[2]);
	motorSet(BACK_RIGHT_MOTOR, -motion[0] - motion[1] + motion[2]);
}

void moveRobotFromJoystick() {
	int motion[3] = {joystickGetAnalog(1, 3), joystickGetAnalog(1, 4), joystickGetAnalog(1, 1)};
	moveRobot(motion);
}

// End C++ export structure
#ifdef __cplusplus
}
#endif

#endif
