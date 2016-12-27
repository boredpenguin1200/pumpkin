/*
 * espComs.c
 *
 *  Created on: Dec 12, 2016
 *      Author: chrisn
 */

#include "espComs.h"
#include "msp430.h"
#include "string.h"

volatile unsigned char espComsStatus;

unsigned char espRxIndex = 0;
unsigned char espTxIndex = 0;
unsigned char espTxLen = 0;
unsigned char espRxBuf[ESP_BUF_SIZE];
unsigned char espTxBuf[ESP_BUF_SIZE];

void initESPPins() {
	P4DIR |= BIT2; //chipd
	P4OUT |= BIT2; //select the esp

	P4DIR |= BIT1;
	P4OUT &= ~BIT1; //rst on esp

	P4DIR &= ~BIT0; //gpio 0 on esp
	P4OUT &= ~BIT0; //pullup on gpio 0 for esp
	P4REN |= BIT0; //pullup in gpio 0 for esp

	P3DIR &= ~BIT4; //gpio 2 n esp
	P3OUT &= ~BIT4;
	P3REN |= BIT4; //gpio 2 pull up
}

//dont use esp peice of crap
void resetESP() {
	P4DIR |= BIT1;
	P4OUT &= ~BIT1;
	__delay_cycles(10000);
//	P4OUT |= BIT1;
	__delay_cycles(10000);
}

void startESPTx() {
	UCA1TXBUF = espTxBuf[espTxIndex];
	espTxIndex = 1;
}

void espTest() {
//#define TEST_MSG "AT+CWSAP=\"ESP_9945B5\",\"abcdefgh\",11,0\x0d\x0a"
//#define TEST_MSG "AT+RST\x0d\x0a"
#define TEST_MSG "AT+GMR\x0d\x0a"
	strncpy((char *) espTxBuf, TEST_MSG, sizeof(TEST_MSG));
	espTxLen = sizeof(TEST_MSG) - 1;
	startESPTx();
}

void espStateMachine() {
	if (!espTxLen) {
		__delay_cycles(100000);
//		resetESP();
		espTest();
	}
	if (espComsStatus & ESP_RXED_MESSAGE) {
		_nop();
	}
}

