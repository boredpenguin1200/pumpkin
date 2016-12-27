/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/  ALL RIGHTS RESERVED  */

#include "LDC13xx16xx_cmd.h"
#include "LDC13xx16xx_evm.h"
#include "host_interface.h"
#include "host_menu.h"
//#include "spi_1p1.h"
#include "pll.h"
#include "i2c.h"
#include "crc8.h"


#include "HAL_FLASH.h"

/** Defaults */
#define INFOD_START (0x1800)

#pragma DATA_SECTION(FlashConfig, ".infoD")
uint16_t  FlashConfig[EVM_DEFAULTS_SIZE*sizeof(uint16_t)];

uint8_t evm_device;
uint8_t evm_rev;
uint8_t evm_device_series;
uint8_t evm_device_channels;

uint8_t EVM_STR_LDC1312[] = {'L','D','C','1','3','1','2','E','V','M','\n'};
uint8_t EVM_STR_LDC1314[] = {'L','D','C','1','3','1','4','E','V','M','\n'};
uint8_t EVM_STR_LDC1612[] = {'L','D','C','1','6','1','2','E','V','M','\n'};
uint8_t EVM_STR_LDC1614[] = {'L','D','C','1','6','1','4','E','V','M','\n'};
uint8_t EVM_STR_FDC2114[] = {'F','D','C','2','1','1','2','E','V','M','\n'};
uint8_t EVM_STR_FDC2214[] = {'F','D','C','2','2','1','4','E','V','M','\n'};
uint8_t EVM_STR_UNKNOWN[] = {'U','N','K','N','O','W','N','E','V','M','\n'};


/** DRDY */
static volatile uint8_t dataReady;
static uint16_t allData[8];

uint8_t default_addr;

static void evm_Delay_Ms(uint16_t ms) {
	uint16_t i;
	for (i = 0; i < ms; i ++) {
		__delay_cycles(EVM_TIME_1MS);
	}
}

/** LEDS */
static uint8_t led_status = 0;
uint8_t evm_LED_Set(evm_led_state_t state) {
	switch (state) {
	case ALLTOGGLE:
		led_status ^= 0x03;
		break;
	case CYCLE:
		led_status += 1;
		led_status &= 0x03;
		break;
	case GREEN:
		led_status = 0x01;
		break;
	case RED:
		led_status = 0x02;
		break;
	case ALLON:
		led_status = 0x03;
		break;
	case ALLOFF:
		led_status = 0x00;
		break;
	default:
		break;
	}
	if (led_status & 0x01) {
		EVM_GRN_LED_ON();
	}
	else {
		EVM_GRN_LED_OFF();
	}
	if (led_status & 0x02) {
		EVM_RED_LED_ON();
	}
	else {
		EVM_RED_LED_OFF();
	}
	return led_status;
}

uint8_t evm_LED_State() {
	return led_status;
}

uint8_t evm_readDefaults(uint8_t offset,uint8_t * buffer,uint8_t size) {
	memcpy(&buffer[0],(uint8_t*)&FlashConfig[offset],size*sizeof(uint16_t));
	return size;
}

/** Default Values */
void evm_saveDefaults(uint8_t * buffer) {
//	uint8_t x;
	// write flash
	//Erase INFOD
//	do {
		//Erase INFOD
		Flash_SegmentErase((uint16_t*)INFOD_START);
//		x = Flash_EraseCheck((uint16_t*)INFOD_START,128/2);
//	} while (x == FLASH_STATUS_ERROR);

	//Write calibration data to INFOD
	FlashWrite_8(
				 (uint8_t*)buffer,
				 (uint8_t*)FlashConfig,
				 EVM_DEFAULTS_SIZE*sizeof(uint16_t)
				 );
}


void InitMCU(void);

/** Initialization */
uint8_t evm_init() {
	// Turn on all LEDs to signify that initization is taking place
	evm_LED_Set(ALLON);

	uint8_t retVal = TRUE;

	dataReady = 0;

	init_Clock_prePLL();
	_disable_interrupts();

//	// LEDs
//	EVM_GRN_LEDDIR |= EVM_GRN_LEDBIT;
//	EVM_RED_LEDDIR |= EVM_RED_LEDBIT;

	// Board ID as inputs and turn on pull ups
	EVM_BOARD_ID_DIR &= ~(EVM_BOARD_ID_R1 | EVM_BOARD_ID_R2 | EVM_BOARD_ID_R3 | EVM_BOARD_ID_R4 | EVM_BOARD_ID_R5);
	EVM_BOARD_ID_RPU |=  (EVM_BOARD_ID_R1 | EVM_BOARD_ID_R2 | EVM_BOARD_ID_R3 | EVM_BOARD_ID_R4 | EVM_BOARD_ID_R5);
	EVM_BOARD_ID_OUT |=  (EVM_BOARD_ID_R1 | EVM_BOARD_ID_R2 | EVM_BOARD_ID_R3 | EVM_BOARD_ID_R4 | EVM_BOARD_ID_R5);

//	// Set device I2C address to 0x2A (ADDR low)
//	EVM_ADDR_DIR |= EVM_ADDR_BIT;
//	EVM_ADDR_OUT &= ~EVM_ADDR_BIT;
//	default_addr = EVM_DEFAULT_I2CADDR;

	// Take the device out of shutdown mode and wait 10ms
	EVM_SHUTDOWN_OUT &= ~EVM_SHUTDOWN_BIT;
	EVM_SHUTDOWN_DIR |= EVM_SHUTDOWN_BIT;
	evm_Delay_Ms(10);

	// Check the board ID for Rev B boards
	uint8_t board_id = EVM_BOARD_ID_IN & EVM_BOARD_ID_MASK;
	board_id = EVM_BOARD_ID_FDC2214;

	// Check which EVM to act as
	evm_LED_Set(ALLON);
	evm_device = 0;
	evm_rev = EVM_FDC21xx22;	// Assume rev b initially
	evm_device_channels = 0;
	evm_device_series = 0;

	switch(board_id){
	case EVM_BOARD_ID_LDC1312:
		evm_device = HOST_DEVICE_LDC1312;
		evm_device_channels = 2;
		evm_device_series = 13;
		HOST_CONTROLLER_TYPE = EVM_STR_LDC1312;
		break;
	case EVM_BOARD_ID_LDC1314:
		evm_device = HOST_DEVICE_LDC1314;
		evm_device_channels = 4;
		evm_device_series = 13;
		HOST_CONTROLLER_TYPE = EVM_STR_LDC1314;
		break;
	case EVM_BOARD_ID_LDC1612:
		evm_device = HOST_DEVICE_LDC1612;
		evm_device_channels = 2;
		evm_device_series = 16;
		HOST_CONTROLLER_TYPE = EVM_STR_LDC1612;
		break;
	case EVM_BOARD_ID_LDC1614:
		evm_device = HOST_DEVICE_LDC1614;
		evm_device_channels = 4;
		evm_device_series = 16;
		HOST_CONTROLLER_TYPE = EVM_STR_LDC1614;
		break;
	case EVM_BOARD_ID_FDC2114:
		evm_device = HOST_DEVICE_FDC2114;
		evm_device_channels = 4;
		evm_device_series = 21;
		HOST_CONTROLLER_TYPE = EVM_STR_FDC2114;
		evm_rev = EVM_FDC21xx22;
		break;
	case EVM_BOARD_ID_FDC2214:
		evm_device = HOST_DEVICE_FDC2214;
		evm_device_channels = 4;
		evm_device_series = 22;
		HOST_CONTROLLER_TYPE = EVM_STR_FDC2214;
		evm_rev = EVM_FDC21xx22;
		break;
	default:
		evm_device = 0;
		evm_device_channels = 0;
		evm_device_series = 0;
		evm_rev = EVM_REV_A;
		HOST_CONTROLLER_TYPE  = EVM_STR_UNKNOWN;
		pll_init();
	}


	//evm_device = HOST_DEVICE_LDC1614;
	//evm_device_channels = 4;
	//evm_device_series = 16;
	//HOST_CONTROLLER_TYPE = EVM_STR_LDC1614;

	// initialize I2C
	i2c_setup();
	_enable_interrupts();
	InitMCU();
	evm_Delay_Ms(10);

	// software reset
	smbus_writeWord(default_addr,LDC13xx16xx_CMD_RESET_DEVICE,0x8000);
	evm_Delay_Ms(10);

	//evm_LED_Set(ALLON);


	// For board identification, first configure for 4 channel device
	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_REF_COUNT_CH0,0xFFFF); // 1048560 clock periods
	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_REF_COUNT_CH1,0xFFFF);
	if(evm_device_channels != 2){
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_REF_COUNT_CH2,0xFFFF);
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_REF_COUNT_CH3,0xFFFF);
	}

	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_OFFSET_CH0,0x0000);
	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_OFFSET_CH1,0x0000);
	if(evm_device_channels != 2){
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_OFFSET_CH2,0x0000);
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_OFFSET_CH3,0x0000);
	}

	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_SETTLE_COUNT_CH0,0x0400); // 16384 clock periods
	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_SETTLE_COUNT_CH1,0x0400);
	if(evm_device_channels != 2){
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_SETTLE_COUNT_CH2,0x0400);
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_SETTLE_COUNT_CH3,0x0400);
	}

	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH0,0x1001); // All dividers set to 1
	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH1,0x1001);
	if(evm_device_channels != 2){
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH2,0x1001);
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH3,0x1001);
	}

	// If Rev B
	if(evm_rev == EVM_REV_B){
		// I Drive should be decimal 17 = 0x11
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH0,0x8800);
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH1,0x8800);
		if(evm_device_channels != 2){
			retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH2,0x8800);
			retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH3,0x8800);
		}
	}
	else{
		// I Drive should be decimal 17 = 0x11
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH0,0x8800);
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH1,0x8800);
		if(evm_device_channels != 2){
			retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH2,0x8800);
			retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_DRIVE_CURRENT_CH3,0x8800);
		}
	}

	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_ERROR_CONFIG,0x0001); // report only DRDYs to INT

//	if(evm_device_channels == 2){
//		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_MUX_CONFIG,0xE20C); // autoscan, measure 0 and 1, deglitch 3.3Mhz
//	}
//	else{
//		// Assume four channel device
//		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_MUX_CONFIG,0xC20C); // autoscan, measure 0, 1, 2, and 3, deglitch 3.3Mhz
//	}

//	if(evm_device_channels != 2){
		// Assume four channel device
//		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_MUX_CONFIG,0xC20C); // autoscan, measure 0, 1, 2, and 3, deglitch 3.3Mhz
//	}
//	else{
		//evm_LED_Set(RED);
		// Assume two channel device since EVMS only have two sensors
		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_MUX_CONFIG,0x820C); // autoscan, measure 0 and 1, deglitch 3.3Mhz
//	}

	// Put the device back in sleep mode
	retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_CONFIG,0x1E01);
	evm_Delay_Ms(10);


	// Check if the device was not properly set
	if(evm_device == 0){
		evm_rev = EVM_REV_A;

		// Get the device ID
		uint16_t dev_id;
		smbus_readWord(default_addr, LDC13xx16xx_CMD_DEVID, &dev_id);

		// Check if 13 or 16 series
		if(dev_id == LDC13xx_DEV_ID){
			evm_device_series = 13;
		}
		else{// if(dev_id == LDC16xx_DEV_ID){
			// Assume series 16 device if not detected
			evm_device_series = 16;
		}

		// Assume that a four channel device is attached
		evm_device_channels = 4;

		// Set the evm_device variable
		if(evm_device_series == 13){
			if(evm_device_channels == 2){
				evm_device = HOST_DEVICE_LDC1312;
				HOST_CONTROLLER_TYPE = EVM_STR_LDC1312;
			}
			else if(evm_device_channels == 4){
				evm_device = HOST_DEVICE_LDC1314;
				HOST_CONTROLLER_TYPE = EVM_STR_LDC1314;
			}
		}
		else if(evm_device_series == 16){
			if(evm_device_channels == 2){
				evm_device = HOST_DEVICE_LDC1612;
				HOST_CONTROLLER_TYPE = EVM_STR_LDC1612;
			}
			else if(evm_device_channels == 4){
				evm_device = HOST_DEVICE_LDC1614;
				HOST_CONTROLLER_TYPE = EVM_STR_LDC1614;
			}
		}

	}

	host_devices[0] = HOST_NUM_DEVICES;
	host_devices[1] = evm_device;




	//retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_CONFIG,0x1E01); // Exit sleep mode, use external clock



	// If device is actually two channels reconfigure the measurement settings
//	if(evm_device_channels == 2){
		// Two channel device
//		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_MUX_CONFIG,0xE20C); // autoscan, measure 0 and 1, deglitch 3.3Mhz
	//}
	//else{

	//}

	// software reset
	//smbus_writeWord(default_addr,LDC13xx16xx_CMD_RESET_DEVICE,0x8000);

	// delay ~10ms
	//evm_Delay_Ms(10);


	// TODO: put addrs in a const array; iterate over array
	// if default data has invalid crc use below and recalibrate
//	if (!calcCRC8((uint8_t *)&FlashConfig[0], EVM_DEFAULTS_SIZE*sizeof(uint16_t))) { // valid crc write defaults
//		for (i = 0x08; i < 0x19; i++) {
//			retVal &= smbus_writeWord(default_addr,i,FlashConfig[i-8]);
//		}
//		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_CONFIG,FlashConfig[i-8]);
//		retVal &= smbus_writeWord(default_addr,LDC13xx16xx_CMD_MUX_CONFIG,FlashConfig[i-7]);
//		for (i = 0x1D; i < 0x22; i++) {
//			retVal &= smbus_writeWord(default_addr,i,FlashConfig[i-10]);
//		}
//	}

	// setup DRDY pin interrupt
	EVM_INT_DIR &= ~EVM_INT_BIT;							// INPUT
	EVM_INT_IE  |=  EVM_INT_BIT;							// interrupt enabled
	EVM_INT_IES |=  EVM_INT_BIT;							// Hi->Lo Edge
	EVM_INT_IFG &= ~EVM_INT_BIT;							// Clear IFG

	return retVal;
}

// TODO: use DMA to transfer data
// interrupt service routine, DRDY
#pragma vector=EVM_INT_VECTOR
__interrupt void DRDY()
{
  if (!(EVM_INT_IN & EVM_INT_BIT)) {
	  dataReady = 1;
  }
  EVM_INT_IFG &= ~EVM_INT_BIT;       // IFG cleared
}

void evm_processDRDY() {
	if (dataReady) {
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_MSB_CH0,&allData[0]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_LSB_CH0,&allData[1]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_MSB_CH1,&allData[2]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_LSB_CH1,&allData[3]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_MSB_CH2,&allData[4]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_LSB_CH2,&allData[5]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_MSB_CH3,&allData[6]);
	  smbus_readWord(default_addr,LDC13xx16xx_CMD_DATA_LSB_CH3,&allData[7]);
	  dataReady = 0;
	}
}

// TODO: use DMA to transfer data
// ch 0-3
uint8_t evm_readFreq(uint8_t ch, uint8_t * buffer) {
	buffer[0] = allData[ch*2] >> 8;
	buffer[1] = allData[ch*2] & 0xFF;
	buffer[2] = allData[ch*2+1] >> 8;
	buffer[3] = allData[ch*2+1] & 0xFF;
	return 4;
}

uint8_t evm_changeAddr(uint8_t addr) {
	if (default_addr == addr) return default_addr;
	if (addr == EVM_MAX_I2CADDR) {
		EVM_ADDR_OUT |= EVM_ADDR_BIT;
		default_addr = EVM_MAX_I2CADDR;
	}
	else if (addr == EVM_MIN_I2CADDR) {
		EVM_ADDR_OUT &= ~EVM_ADDR_BIT;
		default_addr = EVM_MIN_I2CADDR;
	}
	return default_addr;
}

//void evm_setLDCLK(evm_ldclk_state_t state) {
//	uint16_t reg;
//	switch (state) {
//	case OFF:
//		EVM_LDCLK_DIR &= ~EVM_LDCLK_BIT;   // Set to input (hiz)
//		EVM_LDCLK_SEL &= ~EVM_LDCLK_BIT;   // disable function
//		clockState = state;
//		break;
//	case DIV4: case DIV8: case DIV16: case DIV32:
//		EVM_LDCLK_DIR |= EVM_LDCLK_BIT;   // LDC CLK for Freq counter (set to output ACLK)
//		EVM_LDCLK_SEL |= EVM_LDCLK_BIT;   // LDC CLK for freq counter (set to output ACLK)
//		reg = EVM_LDCLK_REG;
//		reg &= ~EVM_LDCLK_MASK;
//		reg |= state;
//		EVM_LDCLK_REG = reg;
//		clockState = state;
//		break;
//	default:
//		break;
//	}
//}

//evm_ldclk_state_t evm_readLDCLK() {
//	return clockState;
//}

void init_Clock_prePLL() {

	UCSCTL3 |= SELREF_2;                      // Set DCO FLL reference = REFO
	  UCSCTL4 |= SELA_3;                        // Set ACLK = REFO

	  __bis_SR_register(SCG0);                  // Disable the FLL control loop
	  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
	  UCSCTL1 = DCORSEL_5;                      // Select DCO range 24MHz operation
	  UCSCTL2 = FLLD_1 + 374;                   // Set DCO Multiplier for 12MHz
	                                            // (N + 1) * FLLRef = Fdco
	                                            // ( 374+1) * 32k = 12MHz
	                                            // Set FLL Div = fDCOCLK/2
	  __bic_SR_register(SCG0);                  // Enable the FLL control loop

	  // Worst-case settling time for the DCO when the DCO range bits have been
	  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
	  // UG for optimization.
	  // 32 x 32 x 12 MHz / 32,768 Hz = 375000 = MCLK cycles for DCO to settle
	  __delay_cycles(375000);

	  // Loop until XT1,XT2 & DCO fault flag is cleared
	  do
	  {
	    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
	                                            // Clear XT2,XT1,DCO fault flags
	    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
	  } while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
}

