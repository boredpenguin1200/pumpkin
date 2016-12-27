/*
 * uart.c
 *
 *  Created on: Dec 11, 2016
 *      Author: chrisn
 */

#include <msp430.h>
#include "espComs.h"

unsigned char uart_rx_buf_index = 0;
unsigned char uart_rx_buf[100];

void initESPUART() {

	P4SEL |= BIT5 + BIT4;                       // P3.3,4 = USCI_A0 TXD/RXD
	UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**

//	UCA1CTL1 |= UCSSEL_2;                     // SMCLK
//	UCA1BR0 = 6;                              // 1MHz 9600 (see User's Guide)
//	UCA1BR1 = 0;                              // 1MHz 9600
//	UCA1MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
											  // over sampling

		UCA1CTL1 |= UCSSEL_2;                     // SMCLK
	UCA1BR0 = 9;                              // 1MHz 115200 (see User's Guide)
	UCA1BR1 = 0;                              	// 1MHz 115200
	UCA1MCTL |= UCBRS_1 + UCBRF_0;            	// Modulation UCBRSx=1, UCBRFx=0

	UCA1CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
	UCA1IE |= UCRXIE | UCTXIE;                    // Enable USCI_A0 RX interrupt

//	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
//	__no_operation();                         // For debugger
}

// Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch (__even_in_range(UCA1IV, 4)) {
	case 0:
		// Vector 0 - no interrupt
		break;
	case 2:
		// Vector 2 - RXIFG
		espRxBuf[espRxIndex] = UCA1RXBUF;
		espRxIndex++;
		if (espRxIndex >= ESP_BUF_SIZE) {
			espRxIndex--; //no overflow
			espComsStatus |= ESP_BUF_OVERFLOW;
		}
		if (espRxBuf[espRxIndex - 1] == ESP_END_CHAR) {
			espComsStatus |= ESP_RXED_MESSAGE;
		}
//		while (!(UCA0IFG & UCTXIFG))
//			;             // USCI_A0 TX buffer ready?
//		UCA0TXBUF = UCA0RXBUF;                  // TX -> RXed character
		break;
	case 4:
		// Vector 4 - TXIFG
		if (espTxIndex < espTxLen) {
			UCA1TXBUF = espTxBuf[espTxIndex];
			espTxIndex++;
		} else {
			espTxLen = 0;
			espTxIndex = 0;
		}
		break;
	default:
		break;
	}
}
