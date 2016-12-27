/*
 * espComs.h
 *
 *  Created on: Dec 12, 2016
 *      Author: chrisn
 */

#ifndef ESPCOMS_H_
#define ESPCOMS_H_

#define ESP_BUF_SIZE 100

#define ESP_END_CHAR '\n'

/**
 * esp coms state machine status
 */
#define ESP_BUF_OVERFLOW 	0x01
#define ESP_RXED_MESSAGE 	0x02
#define ESP_TXING			0x04
#define ESP_RXING			0x08
extern volatile unsigned char espComsStatus;

extern unsigned char espRxIndex;
extern unsigned char espTxIndex;
extern unsigned char espTxLen;
extern unsigned char espRxBuf[ESP_BUF_SIZE];
extern unsigned char espTxBuf[ESP_BUF_SIZE];

void espTest();
void espStateMachine();
void resetESP();
void initESPPins();
#endif /* ESPCOMS_H_ */
