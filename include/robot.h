/** @file robot.h
 * @brief File for robot constants and motion functions
 */

#ifndef ROBOT_H_

// This prevents multiple inclusion, which isn't bad for this file but is good practice
#define ROBOT_H_

#include <API.h>
#include <Math.h>

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define IGNORE_LOW_VAL(V) ((((V) > -10) && ((V) < 10)) ? (0) : (V))

/**
 * Definition for the front left drive motor
 */
#define FRONT_LEFT_MOTOR 1
/**
 * Definition for the front right drive motor
 */
#define FRONT_RIGHT_MOTOR 10
/**
 * Definition for the back left drive motor
 */
#define BACK_LEFT_MOTOR 7
/**
 * Definition for the back right drive motor
 */
#define BACK_RIGHT_MOTOR 2

/**
* Definition for the right top lift motor
*/
#define LIFT_TOP_Y_MOTOR 3
/**
* Definition for the middle left lift motor
*/
#define LIFT_MIDDLE_LEFT_MOTOR 4
/**
* Definition for the right middle lift motor
*/
#define LIFT_MIDDLE_RIGHT_MOTOR 9
/**
* Definition for the bottom left lift motor
*/
#define LIFT_BOTTOM_LEFT_MOTOR 8
/**
* Definition for the right bottom lift motor
*/
#define LIFT_BOTTOM_RIGHT_MOTOR 5

#define PINCER_Y_MOTOR 6

#define MOTOR_SPEED 127
#define LCD_PORT uart1

/**
 * Sets the drive motors for the tank drive based on left and right sides
 *
 * @param left sets the power to the left motors
 * @param right sets the power to the right motors
 */
inline void setDriveMotors(int forward, int horizontal, int turn) {
	motorSet(FRONT_LEFT_MOTOR, -(forward + horizontal + turn));
	motorSet(FRONT_RIGHT_MOTOR, -(-forward + horizontal + turn));
	motorSet(BACK_LEFT_MOTOR, forward - horizontal + turn);
	motorSet(BACK_RIGHT_MOTOR, -forward - horizontal + turn);
}

/**
 * Sets the speed of the pincer motors
 *
 * @param pincer sets the power to the pincer motors
 */
inline void setPincerMotors(int pincer){
  motorSet(PINCER_Y_MOTOR, pincer);
}

/**
 * Sets the speed of the lift motors such that they rotate to move the lift in the same direction
 *
 * @param spd sets the power to the lift motors on a continuum from -1 to 1
 */
inline void setLiftMotors(int spd) {
	motorSet(LIFT_TOP_Y_MOTOR, spd * MOTOR_SPEED);
	motorSet(LIFT_MIDDLE_LEFT_MOTOR, -spd * MOTOR_SPEED);
	motorSet(LIFT_MIDDLE_RIGHT_MOTOR, spd * MOTOR_SPEED);
 	motorSet(LIFT_BOTTOM_LEFT_MOTOR, -spd * MOTOR_SPEED);
	motorSet(LIFT_BOTTOM_RIGHT_MOTOR,  spd * MOTOR_SPEED);
}

#ifdef __cplusplus
}
#endif

#endif
