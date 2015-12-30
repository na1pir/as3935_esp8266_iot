/*
 * Copyright (C) 2015 Uroš Golob <golob.uros@gmail.com>
 * 
 * Work was done with intention to protect electronic devices and so lower e-waste.
 * It might work or it may not, just don't sue me if your house burns down or if it kills you...
 * 
 * This software and hardware is described in my diploma thesis(work in progress). 
 *   
 * Use it for personal use as you wish, just keep this notice on it if you do! 
 * And return changes back free of charge by same restrictions and this notice! 
 * 
 * For comercial use you can contact me and we can make apropriate licencing deal...
 *
 * Development took alot of time and money so remember me with some spare 
 * bitcoins: 1MaGphnuiMjywjQPKHKF239wzcef4KpNxX if you wish:)
 * 
 * This software uses espressif sdk software, spi library from David Ogilvy(MetalPhreak) and libesphttpd & wifi settins from Jeroen Domburg(sprite_fm). So use and rescpect their respective licencies for their work. 
 */
 #include <stdint.h>
#include <esp8266.h>
#include "httpd.h"
#include "httpdespfs.h"
#include "cgi.h"
#include "cgiwifi.h"
#include "cgiflash.h"
#include "stdout.h"
#include "espfs.h"
#include "captdns.h"
#include "webpages-espfs.h"

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "uart.h"
#include "uart_register.h"
#include "user_interface.h"
#include "ip_addr.h"
#include "espconn.h"
#include "as3935.h"
#include "spi_flash.h"
#include "mem.h"
#include "espconn.h"
#include "driver/spi.h"
#include "driver/spi_register.h"

uint32_t i;


volatile uint32_t in_detection_counter=0;

volatile uint8_t tick_flag = 1;
volatile uint8_t interrupt_flag=0;
volatile uint8_t interrupt_set=0;
volatile uint8_t tock=1;


void user_rf_pre_init(void){
	//just becouse we have to http://bbs.espressif.com/viewtopic.php?t=486&p=1895
//initialize things that should be done before rf connection - nothing actualy
}

//0 off
//1 on
//2 calibration
//3 listening
//4 in detection treshold > distance //relay on
//5 in detection treshold > distance timeout //relay off we are out of danger but timeout is still pending 
//6 in detection treshold < distance //off

volatile uint8_t state_machine=0;

 uint8_t threshold_distance=30;
 uint8_t threshold_timeout=15;

 uint8_t tuncaplookuptable[]={ 0,8,16,24,32,40 ,48 ,56 ,64 ,72,80,88,96,104,112,120};
 uint16_t noise_outdoor[]={390, 630, 860, 1100, 1140, 1570, 1800, 2000};
 uint16_t noise_indoor[]={28, 45, 62, 78, 95, 112, 130, 146};
 uint8_t min_num_ligh[]={1,5,9,15};
 uint8_t lcofdiv[]={16,32,64,128};


HttpdBuiltInUrl builtInUrls[]={
	{"/", cgiRedirect, "/index.tpl"},
	{"/index.tpl", cgiEspFsTemplate, tplas3935},
	{"/as3935.cgi", cgias3935, NULL},

	//Routines to make the /wifi URL and everything beneath it work.

	{"/wifi", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/wifiscan.cgi", cgiWiFiScan, NULL},
	{"/wifi/wifi.tpl", cgiEspFsTemplate, tplWlan},
	{"/wifi/connect.cgi", cgiWiFiConnect, NULL},
	{"/wifi/connstatus.cgi", cgiWiFiConnStatus, NULL},
	{"/wifi/setmode.cgi", cgiWiFiSetMode, NULL},

	{"*", cgiEspFsHook, NULL}, //Catch-all cgi function for the filesystem
	{NULL, NULL, NULL}
};

uint64_t seconds_from_boot=0;

void ICACHE_FLASH_ATTR ticker_timer(void *arg)
{
	tick_flag=1;
	if(state_machine>4){//2
		in_detection_counter++;//this is reseted in solid state machine 6
	}
	seconds_from_boot++;
	//if(seconds_from_boot%15==0){
		os_printf("Heap: %ld\nAFE_GB[%d],NF_LEV[%d],WDTH[%d],MIN_NUM_LIGH[%d],SREJ[%d],INT[%d],LCO_FDIV[%d],MASK_DIST[%d],DISTANCE[%d],TUN_CAP[%d], STATE[%d]\n",(unsigned long)system_get_free_heap_size(),as3935.x0.a0.AFE_GB,as3935.x1.a1.NF_LEV,as3935.x1.a1.WDTH,as3935.x2.a2.MIN_NUM_LIGH,as3935.x2.a2.SREJ,as3935.x3.a3.INT,as3935.x3.a3.LCO_FDIV,as3935.x3.a3.MASK_DIST,as3935.x7.a7.DISTANCE,as3935.x8.a8.TUN_CAP,state_machine);
	//}
}



#define procTaskPrio        0
#define procTaskQueueLen    1

os_timer_t some_timer;
os_event_t    procTaskQueue[procTaskQueueLen];

//0 off
//1 on
//2 calibration
//3 listening
//4 in detection treshold > distance //relay on
//5 in detection treshold > distance timeout //relay off we are out of danger but timeout is still pending 
//6 in detection treshold < distance //off
static void ICACHE_FLASH_ATTR procTask(os_event_t *events)
{

	//reschedule
	system_os_post(procTaskPrio, 0, 0 );
	
	
	if(tick_flag==1){
		//todo if interrups start to work anytime soon you only read periodicly when in detection 
		if(tock==1){
			if(state_machine>2){
				as3935_chip_read();
			}
			tock=0;
		}
		else{//alternative to interrupts
		if(GPIO_INPUT_GET(GPIO_ID_PIN(INT_PIN))){//instead of interrupt just read
			pending_interrupt();
		}
			tock=1;
			if(state_machine>2){
				if(as3935.x7.a7.DISTANCE<threshold_distance){
					uart0_sendStr("atention:\n");
					state_machine=6;
				}
			}
				
		}

		tick_flag=0;
		if(interrupt_flag){
			clear_interrupt();
			interrupt_flag=0;
			//return;
		}
		
		if(state_machine==6){
			if(!interrupt_set){
				setup_interrupt();
			}
			as3935_get_lightning_distance();
			if(as3935.x7.a7.DISTANCE<threshold_distance){
				GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),0);//off
				in_detection_counter=0; //reset counter
				uart0_sendStr("danger\n");
				//return;
			}else {
				state_machine=5;
				//return;
			}
		}
		
		if(state_machine==5){
			if(!interrupt_set){
				setup_interrupt();
			}
			if(in_detection_counter>threshold_timeout*60){
					state_machine=4;	
			}
			uart0_sendStr("awaiting timeout\n");
			//return;
		}
		
		if(state_machine==4){
			if(!interrupt_set){
				setup_interrupt();
			}
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),1);	
			if(as3935.x7.a7.DISTANCE==63){
				state_machine=3;
				
				uart0_sendStr("thunderstorm passed\n");
				//return;
			}
		}

		if(state_machine==1){
			as3935_disable_LCO_calibration_mode();
			disable_interrupt();
			uart0_sendStr("on\n");
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),1);	
		}
		
		if(state_machine==2){
			if(interrupt_set){
				disable_interrupt();
			}
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),0);	
			uart0_sendStr("lco cal\n");
			as3935_enable_LCO_calibration_mode();
		}
		
		if(state_machine==0){
			as3935_disable_LCO_calibration_mode();
			if(interrupt_set){
				disable_interrupt();
			}
			uart0_sendStr("off\n");
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),0);	
			//uart0_sendStr("off\n");
		}
		
		if(state_machine==3){
			as3935_disable_LCO_calibration_mode();
			if(!interrupt_set){
				setup_interrupt();
			}
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),1);	
			uart0_sendStr("thunderstrike protection\n");
		}
	}//tok
}




#define SSID "wlan"
#define SSID_PASSWORD "password"


void user_init(void) {
	
	char ssid[32] = SSID;
    char password[64] = SSID_PASSWORD;
    struct station_config station_conf;
    
    //Set station mode (connecting to another network great for testing)
    //wifi_set_opmode( 0x1 );
    
    //or lets just make our own - for offline
   // wifi_set_opmode( 0x2 );
    
    //station+softap 
	wifi_set_opmode( 0x3 ); //warning power hungry

    //Set ap settings
    os_memcpy(&station_conf.ssid, ssid, 32);
    os_memcpy(&station_conf.password, password, 64);
    wifi_station_set_config(&station_conf);
	
	stdoutInit();

	//initialize chip
	as3935_init();
	
	//enable "clock" couter
	system_os_task(procTask, procTaskPrio, procTaskQueue, procTaskQueueLen);
	os_timer_disarm(&some_timer);
	os_timer_setfn(&some_timer, (os_timer_func_t *)ticker_timer, NULL);
	os_timer_arm(&some_timer, 1000, 1); 
	
	//sqedule worker task
	system_os_post(procTaskPrio, 0, 0 );

	// 0x40200000 is the base address for spi fash memory mapping, ESPFS_POS is the position
	// where image is written in flash that is defined in Makefile.
#ifdef ESPFS_POS
	espFsInit((void*)(0x40200000 + ESPFS_POS));
#else
	espFsInit((void*)(webpages_espfs_start));
#endif
	httpdInit(builtInUrls, 80);
	
	load_setings();

	os_printf("\nInit done!\n");
}


