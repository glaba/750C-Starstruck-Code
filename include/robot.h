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
#define LCD_PORT uart1

inline void setDriveMotors(int forward, int lateral, int rotation) {
	motorSet(FRONT_LEFT_MOTOR, forward + lateral + rotation);
	motorSet(FRONT_RIGHT_MOTOR, -forward + lateral + rotation);
	motorSet(BACK_LEFT_MOTOR, forward - lateral + rotation);
	motorSet(BACK_RIGHT_MOTOR, -forward - lateral + rotation);
}

inline void setShooterMotors(int shooter){
  motorSet(SHOOTER_BACK_LEFT_MOTOR, shooter * MOTOR_SPEED);
  motorSet(SHOOTER_FRONT_LEFT_MOTOR, shooter * MOTOR_SPEED);
  motorSet(SHOOTER_BACK_RIGHT_MOTOR, -shooter * MOTOR_SPEED);
  motorSet(SHOOTER_FRONT_RIGHT_MOTOR, -shooter * MOTOR_SPEED);
}

inline void setLiftMotors(int L, int R) {
  motorSet(LIFT_BOTTOM_RIGHT_MOTOR,  R * MOTOR_SPEED);
  motorSet(LIFT_TOP_RIGHT_MOTOR, R * MOTOR_SPEED);
  motorSet(LIFT_BOTTOM_LEFT_MOTOR, -L * MOTOR_SPEED);
  motorSet(LIFT_TOP_LEFT_MOTOR, -L * MOTOR_SPEED);
}

#ifdef __cplusplus
}
#endif

#endif
