/*
 * pins.c
 *
 *  Created on: Dec 11, 2016
 *      Author: chrisn
 */

#include "pins.h"

void setupPins(){
	//motor 1
	MOTOR_1_DRIVE_PORT &= ~MOTOR_1_MASK;
	MOTOR_1_DIR |= MOTOR_1_MASK;

	//motor 2
	MOTOR_2_DRIVE_PORT &= ~MOTOR_2_MASK;
	MOTOR_2_DIR |= MOTOR_2_MASK;

	MOTOR_RESET_PORT &= ~MOTOR_RESET;
	MOTOR_RESET_DIR |= MOTOR_RESET;
}
