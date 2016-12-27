/*
 * motor.h
 *
 *  Created on: Dec 11, 2016
 *      Author: chrisn
 */

#ifndef MOTOR_H_
#define MOTOR_H_

struct motorInfo {
	volatile unsigned char * motorDrivePortAddress;
	unsigned char motorDrivePins[5]; //bit mapping for motor pins (char[0] lowest bit mask... to 4

	volatile unsigned char * motorDirectionPortAddress;
	unsigned char motorDirectionPins[2];
};

enum motorDirEnum {
	FORWARD, BACKWARD, OFF
};

void testMotor();
void initMotor1(struct motorInfo * mi);
void initMotor2(struct motorInfo * mi);
void setMotorDriveStrength(unsigned char percentDrive, struct motorInfo * mi);
void setMotorDir(enum motorDirEnum dir, struct motorInfo * mi);
void motorEnable();
void motorDisable();
#endif /* MOTOR_H_ */
