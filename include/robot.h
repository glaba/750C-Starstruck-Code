#ifndef ROBOT_H_

// This prevents multiple inclusion, which isn't bad for this file but is good practice
#define ROBOT_H_

#include <API.h>

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

#define FRONT_LEFT_MOTOR 4
#define FRONT_RIGHT_MOTOR 2
#define BACK_LEFT_MOTOR 3
#define BACK_RIGHT_MOTOR 1

#define LIFT_BOTTOM_RIGHT_MOTOR 5
#define LIFT_TOP_RIGHT_MOTOR 6
#define LIFT_BOTTOM_LEFT_MOTOR 7
#define LIFT_TOP_LEFT_MOTOR 10

#define SHOOTER_BACK_LEFT_MOTOR 9
#define SHOOTER_FRONT_LEFT_MOTOR 9
#define SHOOTER_BACK_RIGHT_MOTOR 8
#define SHOOTER_FRONT_RIGHT_MOTOR 8

#define MOTOR_SPEED 127

inline void moveRobot(int joystick) {
	int forward = joystickGetAnalog(joystick, 3);
	int right = joystickGetAnalog(joystick, 4);
	int turn = joystickGetAnalog(joystick, 1);

	motorSet(FRONT_LEFT_MOTOR, forward + right + turn);
	motorSet(FRONT_RIGHT_MOTOR, -forward + right + turn);
	motorSet(BACK_LEFT_MOTOR, forward - right + turn);
	motorSet(BACK_RIGHT_MOTOR, -forward - right + turn);
}

inline void setShooterMotors(int speedVariant){
  motorSet(SHOOTER_BACK_LEFT_MOTOR, speedVariant * MOTOR_SPEED);
  motorSet(SHOOTER_FRONT_LEFT_MOTOR, speedVariant * MOTOR_SPEED);
  motorSet(SHOOTER_BACK_RIGHT_MOTOR, -speedVariant * MOTOR_SPEED);
  motorSet(SHOOTER_FRONT_RIGHT_MOTOR, -speedVariant * MOTOR_SPEED);
}

inline void setLiftMotors(int speedVariant) {
  motorSet(LIFT_BOTTOM_RIGHT_MOTOR,  speedVariant * MOTOR_SPEED);
  motorSet(LIFT_TOP_RIGHT_MOTOR, speedVariant * MOTOR_SPEED);
  motorSet(LIFT_BOTTOM_LEFT_MOTOR, -speedVariant * MOTOR_SPEED);
  motorSet(LIFT_TOP_LEFT_MOTOR, -speedVariant * MOTOR_SPEED);
}

#ifdef __cplusplus
}
#endif

#endif