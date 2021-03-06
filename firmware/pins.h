/*
 * pins.h
 *
 *  Created on: Dec 11, 2016
 *      Author: chrisn
 */

#ifndef PINS_H_
#define PINS_H_

#include <msp430.h>

//motor 2 pins
#define MOTOR_2_DIRECTION_1 BIT5
#define MOTOR_2_DIRECTION_2 BIT6
#define MOTOR_2_DRIVE_0 BIT4
#define MOTOR_2_DRIVE_1 BIT3
#define MOTOR_2_DRIVE_2 BIT2
#define MOTOR_2_DRIVE_3 BIT1
#define MOTOR_2_DRIVE_4 BIT0
#define MOTOR_2_DRIVE_PORT P2OUT
#define MOTOR_2_DRIVE_PORT_ADDRESS 0x0203 //P2OUT address
#define MOTOR_2_DIR P2DIR
#define MOTOR_2_DIRECTION_PORT P2OUT
#define MOTOR_2_DIRECTION_PORT_ADDRESS 0x0203 //P2OUT address
#define MOTOR_2_MASK (MOTOR_2_DRIVE_0 | MOTOR_2_DRIVE_1 | MOTOR_2_DRIVE_2 |MOTOR_2_DRIVE_3 |MOTOR_2_DRIVE_4 |MOTOR_2_DIRECTION_1 |MOTOR_2_DIRECTION_2)

//motor 1 pins
#define MOTOR_1_DIRECTION_1 BIT5
#define MOTOR_1_DIRECTION_2 BIT6
#define MOTOR_1_DRIVE_0 BIT4
#define MOTOR_1_DRIVE_1 BIT3
#define MOTOR_1_DRIVE_2 BIT2
#define MOTOR_1_DRIVE_3 BIT1
#define MOTOR_1_DRIVE_4 BIT0
#define MOTOR_1_DRIVE_PORT P1OUT
#define MOTOR_1_DRIVE_PORT_ADDRESS 0x0202 //P1OUT address
#define MOTOR_1_DIR P1DIR
#define MOTOR_1_DIRECTION_PORT P1OUT
#define MOTOR_1_DIRECTION_PORT_ADDRESS 0x0202 //P1OUT address
#define MOTOR_1_MASK (MOTOR_1_DRIVE_0 | MOTOR_1_DRIVE_1 | MOTOR_1_DRIVE_2 |MOTOR_1_DRIVE_3 |MOTOR_1_DRIVE_4 |MOTOR_1_DIRECTION_1 |MOTOR_1_DIRECTION_2)

#define MOTOR_RESET BIT7
#define MOTOR_RESET_PORT P1OUT
#define MOTOR_RESET_DIR P1DIR

void setupPins();

#endif /* PINS_H_ */
