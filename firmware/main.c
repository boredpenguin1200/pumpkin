#include <msp430.h> 
#include <stdint.h>

#include "pins.h"
#include "motor.h"
#include "uart.h"
#include "espComs.h"
#include "scheduler.h"

#include <intrinsics.h>
#include <string.h>

#include <stdint.h>
#include <cstring>
#include <stdio.h>

//#include "LDC13xx16xx_cmd.h"
//#include "LDC13xx16xx_evm.h"

#include "scheduler.h"
//#include "host_interface.h"
//#include "host_packet.h"

#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/types.h"               //Basic Type declarations
#include "USB_API/USB_Common/usb.h"                 //USB-specific functions

#include "F5xx_F6xx_Core_Lib/HAL_UCS.h"
#include "F5xx_F6xx_Core_Lib/HAL_PMM.h"

#include "USB_API/USB_CDC_API/UsbCdc.h"
//#include "usbConstructs.h"
#include "USB_config/descriptors.h"
#include "usbConstructs.h"

volatile uint8_t bCDCDataReceived_event = FALSE; //Indicates data has been received without an open rcv operation
volatile uint16_t usbEvents = (kUSB_VbusOnEvent + kUSB_VbusOffEvent
        + kUSB_receiveCompletedEvent + kUSB_dataReceivedEvent
        + kUSB_UsbSuspendEvent + kUSB_UsbResumeEvent +
        kUSB_UsbResetEvent);

/** USB Communication
 Handles USB Comms
 */
void USBCommunicationTask(void)
{
    uint16_t bytesSent, bytesReceived;
    uint8_t error = 0, status;

    switch (USB_connectionState())
    {
    case ST_ENUM_ACTIVE:
        status = USBCDC_intfStatus(CDC0_INTFNUM, &bytesSent, &bytesReceived);
        if (status & kUSBCDC_waitingForSend)
        {
            error = 1;
        }
        if (bCDCDataReceived_event || (status & kUSBCDC_dataWaiting))
        {
            bCDCDataReceived_event = FALSE;
//            processCmdPacket();
        }
        else
        {
//            evm_processDRDY(); // process DRDY added here for speed (not scheduler-assigned)
            executeTasks();
        }
        break;

    case ST_USB_DISCONNECTED:
    case ST_USB_CONNECTED_NO_ENUM:
    case ST_ENUM_IN_PROGRESS:
    case ST_NOENUM_SUSPENDED:
    case ST_ERROR:
        removeAllTasks();
        break;
    default:
        ;
    }                                              //end of switch-case sentence
    if (error)
    {
        error = 0;
        _nop();
        //TO DO: User can place code here to handle error
    }
}

/** Initialize Clock
 Initializes all clocks: ACLK, MCLK, SMCLK.
 */
void Init_Clock(void)
{
    //Initialization of clock module
    if (USB_PLL_XT == 2)
    {
#if defined (__MSP430F552x) || defined (__MSP430F550x)
        P5SEL |= 0x0C;                               //enable XT2 pins for F5529
#elif defined (__MSP430F563x_F663x)
                P7SEL |= 0x0C;
#endif

#if 0
        //use REFO for FLL and ACLK
        UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (SELREF__REFOCLK);
        UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);

        //MCLK will be driven by the FLL (not by XT2), referenced to the REFO
        Init_FLL_Settle(USB_MCLK_FREQ / 1000, USB_MCLK_FREQ / 32768);//Start the FLL, at the freq indicated by the config
                                                                     //constant USB_MCLK_FREQ
        XT2_Start(XT2DRIVE_0);//Start the "USB crystal"
#endif
        // for USB2ANY which has XT2 crytstal = 24MHz
        UCSCTL4 = SELA_5 + SELS_5 + SELM_5;  // ACLK=XT2  SMCLK=XT2   MCLK=XT2
        XT2_Start(XT2DRIVE_3);
    }
    else
    {
#if defined (__MSP430F552x) || defined (__MSP430F550x)
        P5SEL |= 0x10;                                      //enable XT1 pins
#endif
        //Use the REFO oscillator to source the FLL and ACLK
        UCSCTL3 = SELREF__REFOCLK;
        UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);

        //MCLK will be driven by the FLL (not by XT2), referenced to the REFO
        Init_FLL_Settle(USB_MCLK_FREQ / 1000, USB_MCLK_FREQ / 32768); //set FLL (DCOCLK)

        XT1_Start(XT1DRIVE_0);                         //Start the "USB crystal"
    }
}

/** Initialze MCU
 Initializes the MSP430 peripherals and modules.
 */
void InitMCU(void)
{
    __disable_interrupt();      // Disable global interrupts
    SetVCore(3);
    Init_Clock();               //Init clocks
    USB_init();                 //Init USB

    USB_setEnabledEvents((WORD) usbEvents);

    //Check if we're already physically attached to USB, and if so, connect to it
    //This is the same function that gets called automatically when VBUS gets attached.
    if (USB_connectionInfo() & kUSB_vbusPresent)
    {
        USB_handleVbusOnEvent();
    }
    __enable_interrupt();                            // enable global interrupts

}

/*
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    __bis_SR_register(GIE);		//enable global interupts
    setupPins();

    initESPPins();
//	initESPUART();
    resetESP();
    InitMCU();
    USB_handleVbusOnEvent();
    _enable_interrupts();
    initHostInterface();

#define BUF_SIZE 10
    uint8_t msgBuf[BUF_SIZE]; //"M1 1 100\n"
//    uint8_t bufPtr = 0;
    uint8_t bytesRead = 0;
    uint8_t i = 0;
    uint8_t motorVal = 0;

    struct motorInfo m1;
    struct motorInfo m2;

    initMotor1(&m1);
    initMotor2(&m2);

    //    motorEnable();
    setMotorDir(FORWARD, &m1);
    setMotorDir(FORWARD, &m2);

    motorEnable();

    setMotorDriveStrength(0, &m1);
    setMotorDriveStrength(0, &m2);

    while (1)
    {

        bytesRead += cdcReceiveDataInBuffer(&msgBuf[bytesRead], BUF_SIZE, 0);
        if (bytesRead > 8)
        {
            if (msgBuf[0] == 'M')
            {
                //M1 +100\n
                motorVal = 0;
                motorVal += 100 * (msgBuf[4] - 48);
                motorVal += 10 * (msgBuf[5] - 48);
                motorVal += (msgBuf[6] - 48);
                if (msgBuf[1] == '1')
                {
                    if (msgBuf[3] == '+')
                    {
                        setMotorDir(FORWARD, &m1);
                    }
                    else if (msgBuf[3] == '-')
                    {
                        setMotorDir(BACKWARD, &m1);
                    }
                    setMotorDriveStrength(motorVal, &m1);
                }
                else if (msgBuf[1] == '2')
                {
                    if (msgBuf[3] == '+')
                    {
                        setMotorDir(FORWARD, &m2);
                    }
                    else if (msgBuf[3] == '-')
                    {
                        setMotorDir(BACKWARD, &m2);
                    }
                    setMotorDriveStrength(motorVal, &m2);
                }
                cdcSendDataWaitTilDone("ok\n", sizeof("ok\n"), 0, 0xFF);

            }
            for (i = 0; i < BUF_SIZE; i++)
            {
                msgBuf[i] = 0;
            }
            bytesRead = 0;
        }
        if (msgBuf[bytesRead - 1] == '>')
        {
            for (i = 0; i < BUF_SIZE; i++)
            {
                msgBuf[i] = 0;
            }
            bytesRead = 0;
            cdcSendDataWaitTilDone("ok\n", sizeof("ok\n"), 0, 0xFF);
        }
        __delay_cycles(100000);

//#define MSG "hello world!\n"
//        cdcSendDataWaitTilDone(MSG, sizeof(MSG), 0, 0xFF);
//        __delay_cycles(100000);
//        // USB Comms
////        USBCommunicationTask();
    }
//
//	__delay_cycles(100000);
//
//	espTest();
//
//	while (1) {
//		espStateMachine();
//	}

//	struct motorInfo m1;
//	struct motorInfo m2;
//
//	initMotor1(&m1);
//	initMotor2(&m2);
//
////	motorEnable();
//	setMotorDir(FORWARD, &m1);
//	setMotorDir(BACKWARD, &m2);
//
//	setMotorDriveStrength(100, &m1);
//	setMotorDriveStrength(100, &m2);
//
//	__delay_cycles(1000000);
//	motorDisable();
////	testMotor();
    return 0;
}

