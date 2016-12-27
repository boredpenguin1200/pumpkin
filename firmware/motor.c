/*
 * motor.c
 *
 *  Created on: Dec 11, 2016
 *      Author: chrisn
 */

#include "motor.h"

#include "pins.h"

void motorDisable() {
	MOTOR_RESET_PORT &= ~MOTOR_RESET;
}

void motorEnable() {
	MOTOR_RESET_PORT |= MOTOR_RESET;
}

void initMotor1(struct motorInfo * mi) {
	(*mi).motorDrivePortAddress =
			(volatile unsigned char *) MOTOR_1_DRIVE_PORT_ADDRESS;
	(*mi).motorDrivePins[0] = MOTOR_1_DRIVE_0;
	(*mi).motorDrivePins[1] = MOTOR_1_DRIVE_1;
	(*mi).motorDrivePins[2] = MOTOR_1_DRIVE_2;
	(*mi).motorDrivePins[3] = MOTOR_1_DRIVE_3;
	(*mi).motorDrivePins[4] = MOTOR_1_DRIVE_4;

	(*mi).motorDirectionPortAddress =
			(volatile unsigned char *) MOTOR_1_DIRECTION_PORT_ADDRESS;
	(*mi).motorDirectionPins[0] = MOTOR_1_DIRECTION_1;
	(*mi).motorDirectionPins[1] = MOTOR_1_DIRECTION_2;
}

void initMotor2(struct motorInfo * mi) {
	(*mi).motorDrivePortAddress =
			(volatile unsigned char *) MOTOR_2_DRIVE_PORT_ADDRESS;
	(*mi).motorDrivePins[0] = MOTOR_2_DRIVE_0;
	(*mi).motorDrivePins[1] = MOTOR_2_DRIVE_1;
	(*mi).motorDrivePins[2] = MOTOR_2_DRIVE_2;
	(*mi).motorDrivePins[3] = MOTOR_2_DRIVE_3;
	(*mi).motorDrivePins[4] = MOTOR_2_DRIVE_4;

	(*mi).motorDirectionPortAddress =
			(volatile unsigned char *) MOTOR_2_DIRECTION_PORT_ADDRESS;
	(*mi).motorDirectionPins[0] = MOTOR_2_DIRECTION_1;
	(*mi).motorDirectionPins[1] = MOTOR_2_DIRECTION_2;
}

void testMotor() {

//	MOTOR_1_PORT |= MOTOR_1_DRIVE_0 | MOTOR_1_DRIVE_1;
//	MOTOR_1_PORT |= MOTOR_1_DIRECTION_1;
//	MOTOR_1_PORT &= ~MOTOR_1_DIRECTION_2;
//
//	MOTOR_2_DRIVE_PORT |= MOTOR_2_DRIVE_0 | MOTOR_2_DRIVE_1;
//	MOTOR_2_DRIVE_PORT |= MOTOR_2_DIRECTION_1;
//	MOTOR_2_DRIVE_PORT &= ~MOTOR_2_DIRECTION_2;
//
//	motorEnable();
//	__delay_cycles(10000000);
//	motorDisable();

}

void setMotorDriveStrength(unsigned char percentDrive, struct motorInfo * mi) {
	unsigned char value = (percentDrive * 0x1F) / 100; //scale 100% to 5 bits
	int i;
	for (i = 0; i < 5; i++) {
		if (value & (0x1 << i)) {
			(*(*mi).motorDrivePortAddress) |= (*mi).motorDrivePins[i];
		} else {
			(*(*mi).motorDrivePortAddress) &= ~(*mi).motorDrivePins[i];
		}
	}
}

void setMotorDir(enum motorDirEnum dir, struct motorInfo * mi) {
	if (dir == FORWARD) {
		(*(*mi).motorDirectionPortAddress) |= (*mi).motorDirectionPins[0];
		(*(*mi).motorDirectionPortAddress) &= ~(*mi).motorDirectionPins[1];
	} else if (dir == BACKWARD) {
		(*(*mi).motorDirectionPortAddress) &= ~(*mi).motorDirectionPins[0];
		(*(*mi).motorDirectionPortAddress) |= (*mi).motorDirectionPins[1];
	} else if (dir == OFF) {
		(*(*mi).motorDirectionPortAddress) &= ~(*mi).motorDirectionPins[0];
		(*(*mi).motorDirectionPortAddress) &= ~(*mi).motorDirectionPins[1];
	} else {
		//turn off
		(*(*mi).motorDirectionPortAddress) &= ~(*mi).motorDirectionPins[0];
		(*(*mi).motorDirectionPortAddress) &= ~(*mi).motorDirectionPins[1];
	}
}
