/*
 * Uroš Golob <golob.uros@gmail.com> wrote this file.
 * 
 * Work was done with intention to protect electronic devices and so lower e-waste.
 * It might work or it may not, just don't sue me if your house burns down or if it kills you...
 * 
 * This software and hardware is described in my diploma thesis(work in progress). 
 * When it will be finished and published you can read it but just learn Slovenian first if you wish to read it:)
 *  
 * Use it for personal use as you will, just keep this notice on it if you do! 
 * And return changes back free of charge by same restrictions and notice! 
 * 
 * For comercial use you can contact me and we can make a deal...
 *
 * Development took alot of time and money so remember me with some spare 
 * bitcoins: 1MaGphnuiMjywjQPKHKF239wzcef4KpNxX if you wish:)
 * 
 * This software uses expresif sdk software, spi library from David Ogilvy(MetalPhreak) and libesphttpd & wifi settins form Jeroen Domburg(sprite_fm). So use and rescpect their respective licencies for their work. 
 */

#include "as3935.h"

struct as3935_t as3935;

#define HSPI_INIT_STUF 	{spi_init(HSPI);spi_init_gpio(HSPI, SPI_CLK_USE_DIV);spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);spi_rx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);} 

#define GPIO_OUTPUT_SET(gpio_no, bit_value)	gpio_output_set(bit_value<<gpio_no, ((~bit_value)&0x01)<<gpio_no, 1<<gpio_no,0)
//spi_transaction(spi_no, cmd_bits, cmd_data, addr_bits, addr_data, dout_bits, dout_data, din_bits, dummy_bits)
//spi_no: SPI or HSPI
//cmd_bits: length of command. Set to 0 to disable command component.
//cmd_data: Command data to send.
//addr_bits: length of address. Set to 0 to disable address component.
//addr_data: Address data to send.
//dout_bits: Length of data payload to send. Set to 0 to disable data out component.
//dout_data: Data payload to send.
//din_bits: Length of data payload to receive. Set to 0 if not receiving any data.
//dummy_bits: Number of dummy bits to insert.
#define spi_write(addr_data,dout_data) spi_transaction(HSPI, 2, 0, 6, (addr_data), 8, (uint32)(dout_data), 0, 0)
#define spi_read(addr_data) (uint8)spi_transaction( HSPI, 2, 1, 6, (addr_data), 0, 0, 8, 0)

void  ICACHE_FLASH_ATTR as3935_chip_read(){
	uint8_t tm[100];
	int chip;
	
	HSPI_INIT_STUF;
	
	for(chip=0;chip<9;chip++){
		switch(chip){
			case 0:
				as3935.x0.d0 = spi_read(chip);
			break;
			case 1:
				as3935.x1.d1 = spi_read(chip);
			break;
			case 2:
				as3935.x2.d2 = spi_read(chip);
			break;
			case 3:
				as3935.x3.d3 = spi_read(chip);
			break;
			case 4:
				as3935.x4.d4 = spi_read(chip);
			break;
			case 5:
				as3935.x5.d5 = spi_read(chip);
			break;
			case 6:
				as3935.x6.d6 = spi_read(chip);
			break;
			case 7:
				as3935.x7.d7 = spi_read(chip);
			break;
			case 8:
				as3935.x8.d8 = spi_read(chip);
			break;
		}
	}
/*		uint8_t tmp[1000];
os_sprintf(tmp,"[AFE_GB %d],[NF_LEV %d],[WDTH %d],[MIN_NUM_LIGH %d],[SREJ %d],[INT %d],[LCO_FDIV %d],[MASK_DIST %d],[DISTANCE %d],[TUN_CAP %d]\n",as3935.x0.a0.AFE_GB,as3935.x1.a1.NF_LEV,as3935.x1.a1.WDTH,as3935.x2.a2.MIN_NUM_LIGH,as3935.x2.a2.SREJ,as3935.x3.a3.INT,as3935.x3.a3.LCO_FDIV,as3935.x3.a3.MASK_DIST,as3935.x7.a7.DISTANCE,as3935.x8.a8.TUN_CAP);
			uart0_sendStr(tmp);*/
			uart0_sendStr(".");
}


//direct command
uint8_t ICACHE_FLASH_ATTR  as3935_dc(uint8_t dc){
	HSPI_INIT_STUF;
	spi_write(0x3d,0x96);
}

uint32_t ICACHE_FLASH_ATTR as3935_get_lightning_energy(){
	HSPI_INIT_STUF;
	as3935.x4.d4=spi_read(4);	
	as3935.x5.d5=spi_read(5);
	as3935.x6.d6=spi_read(6);
	return ((uint32_t)as3935.x6.a6.S_LIG_MM<<16)+((uint32_t)as3935.x5.a5.S_LIG_M<<8)+(uint32_t)as3935.x4.a4.S_LIG_L;
}

//0b111111 means that it is out of range for calculation
//otherwise it is value in km
uint8_t ICACHE_FLASH_ATTR as3935_get_lightning_distance(){
	HSPI_INIT_STUF;
	as3935.x7.d7=spi_read(7);
	return as3935.x7.a7.DISTANCE;
}

//returns:
// 1 for INT_NH noise level to high
// 4 for INT_D disturber detected 
// 8 for INT_L lightning detected
uint8_t ICACHE_FLASH_ATTR as3935_interrupt_source(){//get interrupt source but first wait for 2ms for chip to calculate what trigerred it
	//this won't work
	//os_delay_us(2000);
	
	HSPI_INIT_STUF;

	as3935.x3.d3=spi_read(3);
	return as3935.x3.a3.INT;
}

void ICACHE_FLASH_ATTR as3935_clear_stat(){
	HSPI_INIT_STUF;
	//REG0x02[6] 1->0->1
	spi_read(2);
	as3935.x2.a2.CL_STAT=1;
	spi_write(2,as3935.x2.d2);
	as3935.x2.a2.CL_STAT=0;
	spi_write(2,as3935.x2.d2);
	as3935.x2.a2.CL_STAT=1;
	spi_write(2,as3935.x2.d2);
}

//if we get interrupt for disturber  (IRQ_NOISE_FLOOR 0001)increes for 1, def:010, min:000, max:111

//returns values from 0 to 7 
uint8_t ICACHE_FLASH_ATTR as3935_get_noise_floor_level(){
	HSPI_INIT_STUF;
	spi_read(1);
	return as3935.x1.a1.NF_LEV;
} 

//values from 0 to 7 are aceptable
void ICACHE_FLASH_ATTR as3935_set_noise_floor_level(uint8_t noise){
	HSPI_INIT_STUF;
	as3935.x1.a1.NF_LEV=noise;
	spi_write(1,as3935.x1.d1);
}
	
//before we acknolage thunderstorm
uint8_t ICACHE_FLASH_ATTR as3935_get_min_lightning_events(){
	HSPI_INIT_STUF;
	spi_read(2);
	return as3935.x2.a2.MIN_NUM_LIGH;
} 


//value in register-> 	interpretation of minimal lightning  events before chip decides thet there is storm nearby
//REG0x02[5] REG0x02[4] 0->1,1->5,2->9,3->16
void ICACHE_FLASH_ATTR as3935_set_min_lightning_events(uint8_t num){
	HSPI_INIT_STUF;
	as3935.x2.a2.MIN_NUM_LIGH=num;
	spi_write(2,as3935.x2.d2);
}

//look if reporting of disturbers on interrupt pin is enabled
unsigned char ICACHE_FLASH_ATTR as3935_get_mask_disturbers(){
	HSPI_INIT_STUF;
	spi_read(3);
	return as3935.x3.a3.MASK_DIST;
}

//enable or disable reporting of disturbers 
//should be on only at setup and trublesuting
void ICACHE_FLASH_ATTR as3935_set_mask_disturbers(uint8_t enabled){
	HSPI_INIT_STUF;
	as3935.x3.d3=spi_read(3);
	as3935.x3.a3.MASK_DIST=enabled;
	spi_write(3,as3935.x3.d3);
}

//#define INDOOR 0b10010
//#define OUTDOOR 0b01110
//indoor == 0b10010 or outdoor == 0b01110
uint8_t ICACHE_FLASH_ATTR as3935_get_AFE_gainboost(){
	HSPI_INIT_STUF;
	spi_read(0);
	return as3935.x0.a0.AFE_GB;
}

//#define INDOOR 0b10010
//#define OUTDOOR 0b01110
//indoor == 0b10010 or outdoor == 0b01110
void ICACHE_FLASH_ATTR as3935_set_AFE_gainboost(uint8_t gain){
	HSPI_INIT_STUF;
	as3935.x0.d0=spi_read(0);
	as3935.x0.a0.AFE_GB=gain;
	spi_write(0,as3935.x0.d0);
}

uint8_t as3935_get_watchdog_thresold(){
	HSPI_INIT_STUF;
	spi_read(1);
	return as3935.x1.a1.WDTH;
}

//default 0001, if incresed we lose ability to detect at long range but
// get better rejection of disturbers 
void ICACHE_FLASH_ATTR as3935_set_watchdog_thresold(uint8_t threshold){
	HSPI_INIT_STUF;
	as3935.x1.d1=spi_read(1);
	as3935.x1.a1.WDTH=threshold;
	spi_write(1,as3935.x1.d1);	
}

uint8_t ICACHE_FLASH_ATTR as3935_get_spike_rejection(){
	HSPI_INIT_STUF;
	spi_read(2);
	return as3935.x2.a2.SREJ;
}

void ICACHE_FLASH_ATTR as3935_set_spike_rejection(uint8_t threshold){
	HSPI_INIT_STUF;
	as3935.x2.d2=spi_read(2);
	as3935.x2.a2.SREJ=threshold;
	spi_write(2,as3935.x2.d2);
}

//main resonance frequency shall be divided with 16/32/64/128 then we can mesure it with osciloscope 
//#define LCO_FDIV16 0
//#define LCO_FDIV32 1
//#define LCO_FDIV64 2
//#define LCO_FDIV128 3
void ICACHE_FLASH_ATTR as3935_set_lco_divider(uint8_t newdivider){
	HSPI_INIT_STUF;
	as3935.x3.d3=spi_read(3);
	as3935.x3.a3.LCO_FDIV=newdivider;
	spi_write(3,as3935.x3.d3);
}


//before running this disable gpio interrupt trigger
void ICACHE_FLASH_ATTR as3935_set_lco_calibration(uint8_t mode){
	HSPI_INIT_STUF;
	as3935.x8.d8=spi_read(8);
	as3935.x8.a8.DISP_LCO=mode;
	spi_write(8,as3935.x8.d8);	
}


void ICACHE_FLASH_ATTR as3935_enable_LCO_calibration_mode(){
	ETS_GPIO_INTR_DISABLE();
	as3935_set_lco_calibration(1);
}

void ICACHE_FLASH_ATTR as3935_disable_LCO_calibration_mode(){
	as3935_set_lco_calibration(0);
	as3935_TRCO_calibration();
}

uint8_t ICACHE_FLASH_ATTR as3935_get_LCO_calibration(){
	HSPI_INIT_STUF;
	spi_read(8);
	return as3935.x8.a8.DISP_LCO;
}


//returns values 
//#define LCO_FDIV16 0
//#define LCO_FDIV32 1
//#define LCO_FDIV64 2
//#define LCO_FDIV128 3
uint8_t ICACHE_FLASH_ATTR as3935_get_lco_divider(){
	HSPI_INIT_STUF;
	spi_read(3);
	return as3935.x3.a3.LCO_FDIV;
} 


	
//i don't belive that we will ever need this if then we need turn on as well 
void ICACHE_FLASH_ATTR as3935_power_down(){
	HSPI_INIT_STUF;
	as3935.x0.d0=spi_read(0);
	as3935.x0.a0.PWD=0;
	spi_write(0,as3935.x0.d0);
}

void ICACHE_FLASH_ATTR power_up(){
	HSPI_INIT_STUF;
	as3935.x0.d0=spi_read(0);
	as3935.x0.a0.PWD=1;
	spi_write(0,as3935.x0.d0);
}
	
void ICACHE_FLASH_ATTR as3935_TRCO_calibration(){
	HSPI_INIT_STUF;
	as3935.x8.d8=spi_read(8);

	spi_write(0x3d,0x96);//dc CALIB_RCO
	as3935.x8.a8.DISP_TRCO=1;	
	spi_write(8,as3935.x8.d8);
	//this won't work
	//os_delay_us(2000);
	
	as3935.x8.a8.DISP_TRCO=0;	
	spi_write(8,as3935.x8.d8);
}

uint8_t ICACHE_FLASH_ATTR as3935_get_tuning_capacitor(){
	HSPI_INIT_STUF;
	spi_read(8);
	return as3935.x8.a8.TUN_CAP;
}

void ICACHE_FLASH_ATTR as3935_set_tuning_capacitor(uint8_t cap){
	HSPI_INIT_STUF;
	as3935.x8.d8=spi_read(8);
	as3935.x8.a8.TUN_CAP=cap;
	spi_write(8,as3935.x8.d8);		
}


//let us just set everithing in its defoult values
//init function 
//sets pins
//sets ...

void ICACHE_FLASH_ATTR as3935_init(){
	//we can only use uart for debugging no jtag in this iot devil
	//uart_div_modify(0, 115200);
	uart0_sendStr("as3935_init function\n");
	
	//we use i2c for communication with as3935
	//justinitialize it in read and write functions
	
	//first inicialize chip
	spi_write(0x3c,0x96);//dc	PRESET_DEFAULT
	spi_write(0x3d,0x96);//dc CALIB_RCO
	as3935_TRCO_calibration();
	as3935_set_min_lightning_events(0);//REG0x02[5] REG0x02[4] 1(0),5(1),9(2),16(3)
	as3935_set_mask_disturbers(0);//after we are satified with operation just turn off
	as3935_set_AFE_gainboost(INDOOR);
	as3935_set_watchdog_thresold(1);
	as3935_set_spike_rejection(2);
	as3935_set_noise_floor_level(2);
}
