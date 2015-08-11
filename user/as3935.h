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

#ifndef __AS3935_H__
#define __AS3935_H__


#include "c_types.h"
#include "ip_addr.h"
#include "ets_sys.h"
#include "espconn.h"
#include "osapi.h"
#include "mem.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "c_types.h"
#include "osapi.h"
#include "driver/spi.h"
#include "driver/spi_register.h"
#include "gpio.h"
#include "uart.h"
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"



#define HSPI_INIT_STUF 	{spi_init(HSPI);spi_init_gpio(HSPI, SPI_CLK_USE_DIV);spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);spi_rx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);WRITE_PERI_REG(SPI_CTRL2(HSPI), 2<<SPI_MOSI_DELAY_MODE_S);} 

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
#define spi_write(addr_data,out_data) spi_transaction(HSPI, 2, 0, 6, (addr_data), 8, (uint32_t)(out_data), 0, 0)
#define spi_read(addr_data) (uint8_t)spi_transaction( HSPI, 2, 1, 6, (addr_data), 0, 0, 8, 0)




//structure that maps all registers
//so we can acces all data from chip as its meaning and read and write as byte
struct as3935_t{
	union data_t0 {
		uint8_t d0;
		struct as3935_data0_t{
			uint8_t PWD:1;
			uint8_t AFE_GB:5;
			uint8_t reserved:2;
		}a0;
	}x0;
	
	union data_t1 {
		uint8_t d1;
		struct as3935_data1_t{
		uint8_t WDTH:4;
		uint8_t NF_LEV:3;
		uint8_t reserved:1;
		
		}a1;
	}x1;
	
	union data_t2 {
		uint8_t d2;
		struct as3935_data2_t{
		uint8_t SREJ:4;
		uint8_t MIN_NUM_LIGH:2;
		uint8_t CL_STAT:1;
		uint8_t reserved:1;
		}a2;
	}x2;
	
	union data_t3 {
		uint8_t d3;
		struct as3935_data3_t{
		uint8_t INT:4;
		uint8_t reserved:1;
		uint8_t MASK_DIST:1;
		uint8_t LCO_FDIV:2;
		
		}a3;
	}x3;
		
	union data_t4 {
		uint8_t d4;
		struct as3935_data4_t{
		uint8_t S_LIG_L:8;
		}a4;
	}x4;
	
	union data_t5 {
		uint8_t d5;
		struct as3935_data5_t{
			uint8_t S_LIG_M:8;
		}a5;
	}x5;
	
	union data_t6 {
		uint8_t d6;
		struct as3935_data6_t{
				uint8_t S_LIG_MM:5;
				uint8_t reserved:3;
		
		}a6;
	}x6;
	
	union data_t7 {
		uint8_t d7;
		struct as3935_data7_t{
			
			uint8_t DISTANCE:6;
			uint8_t reserved:2;
		}a7;
	}x7;		
	union data_t8 {
		uint8_t d8;
		struct as3935_data8_t{
			uint8_t TUN_CAP:4;
			uint8_t reserved:1;
			uint8_t DISP_TRCO:1;
			uint8_t DISP_SRCO:1;
			uint8_t DISP_LCO:1;
		}a8;
	}x8;
};

extern struct as3935_t as3935;
extern struct settings_t settings;

void pending_interrupt();
void ICACHE_FLASH_ATTR disable_interrupt();

void ICACHE_FLASH_ATTR setup_interrupt();
void ICACHE_FLASH_ATTR clear_interrupt();

//user pins;1 interrupt input, 1 output

#define INT_PIN 4
#define RELAY_PIN 5

//Constants used for Register Values
#define MAX_NOISE_FLOOR 0x07
#define LCO_FDIV16 0
#define LCO_FDIV32 1
#define LCO_FDIV64 2
#define LCO_FDIV128 3
#define DIRECT_COMMAND 0x96 //to registers: 0x3d 0x3c
#define CALIB_RCO 0x3d
#define PRESET_DEFAULT 0x3c
#define IRQ_NF 1
#define IRQ_D 4
#define IRQ_L 8
#define INDOOR 0b10010 //18
#define OUTDOOR 0b01110 //14


//0 off
//1 on
//2 calibration
//3 listening
//4 in detection > distance 
//5 in detection > distance timeout 
//6 in detection < distance
//7 in detection < distance timeout


extern volatile uint32_t in_detection_counter;

extern volatile uint8_t tick_flag;
extern volatile uint8_t interrupt_flag;
extern volatile uint8_t interrupt_set;
extern volatile uint8_t tock;

extern volatile uint8_t state_machine;
extern uint8_t threshold_distance;
extern uint8_t threshold_timeout;

extern uint8_t tuncaplookuptable[];
extern uint16_t noise_outdoor[];
extern uint16_t noise_indoor[];
extern uint8_t min_num_ligh[];
extern uint8_t lcofdiv[];

uint8_t ICACHE_FLASH_ATTR  as3935_dc(uint8_t dc);
uint32_t ICACHE_FLASH_ATTR as3935_get_lightning_energy();
uint8_t ICACHE_FLASH_ATTR as3935_get_lightning_distance();
uint8_t ICACHE_FLASH_ATTR as3935_interrupt_source();//first wait 2ms!!!!! then,read the Interrupt register.
void  ICACHE_FLASH_ATTR as3935_chip_read();
uint8_t ICACHE_FLASH_ATTR as3935_get_noise_floor_level(); //if we get interrupt for disturber  (IRQ_NOISE_FLOOR 0001)increes for 1, def:010, min:000, max:111
void ICACHE_FLASH_ATTR as3935_set_noise_floor_level(uint8_t noise);
uint8_t ICACHE_FLASH_ATTR as3935_get_min_lightning_events(); /*before we acknolage lightningstorm*/
void as3935_set_min_lightning_events(uint8_t num);/*REG0x02[5] REG0x02[4] 1(0),5(1),9(2),16(3)*/
uint8_t ICACHE_FLASH_ATTR as3935_get_mask_disturbers();
void ICACHE_FLASH_ATTR as3935_set_mask_disturbers(uint8_t enabled);/*after we are satified with operation just turn off*/
uint8_t ICACHE_FLASH_ATTR as3935_get_AFE_gainboost();
void ICACHE_FLASH_ATTR as3935_set_AFE_gainboost(uint8_t gain);
uint8_t ICACHE_FLASH_ATTR as3935_get_tuning_capacitor();
void ICACHE_FLASH_ATTR as3935_set_tuning_capacitor(uint8_t cap);
uint8_t as3935_get_watchdog_thresold();
void ICACHE_FLASH_ATTR as3935_set_watchdog_thresold(uint8_t threshold);
uint8_t ICACHE_FLASH_ATTR as3935_get_spike_rejection();
void ICACHE_FLASH_ATTR as3935_clear_stat();
void ICACHE_FLASH_ATTR as3935_set_spike_rejection(uint8_t threshold);
void ICACHE_FLASH_ATTR as3935_set_lco_divider(uint8_t divider);
uint8_t ICACHE_FLASH_ATTR  as3935_get_lco_divider();
void ICACHE_FLASH_ATTR as3935_set_lco_calibration(uint8_t mode);
void ICACHE_FLASH_ATTR as3935_power_down();
void ICACHE_FLASH_ATTR as3935_power_up();
void ICACHE_FLASH_ATTR as3935_TRCO_calibration();
void ICACHE_FLASH_ATTR as3935_enable_LCO_calibration_mode();
void ICACHE_FLASH_ATTR as3935_disable_LCO_calibration_mode();
void ICACHE_FLASH_ATTR as3935_init();


#endif
