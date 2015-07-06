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
volatile uint8_t tock=1;


void user_rf_pre_init(void){
	//just becouse we have to http://bbs.espressif.com/viewtopic.php?t=486&p=1895
//initialize things that should be done before rf connection - nothing actualy
}

//user pins;1 interrupt input, 1 output
#define INT_PIN 4
#define RELAY_PIN 5

//0 off
//1 on
//2 calibration
//3 listening
//4 in detection treshold > distance //relay on
//5 in detection treshold > distance timeout //relay off we are out of danger but timeout is still pending 
//6 in detection treshold < distance //off

volatile uint8_t state_machine=0;

 uint8_t threshold_distance=10;
 uint8_t threshold_timeout=15;

 uint8_t tuncaplookuptable[]={ 0,8,16,24,32,40 ,48 ,56 ,64 ,72,80,88,96,104,112,120};
 uint16_t noise_outdoor[]={390, 630, 860, 1100, 1140, 1570, 1800, 2000};
 uint16_t noise_indoot[]={28, 45, 62, 78, 95, 112, 130, 146};
 uint8_t min_num_ligh[]={1,5,9,15};
 uint8_t lcofdiv[]={16,32,64,128};
/* 
#ifdef ESPFS_POS
CgiUploadEspfsParams espfsParams={
	.espFsPos=ESPFS_POS,
	.espFsSize=ESPFS_SIZE
};
#endif
*/


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
	if(state_machine>2){
		in_detection_counter++;//this is reseted in solid state machine 6
	}
	seconds_from_boot++;
	if(seconds_from_boot%15==0){
		os_printf("Heap: %ld\nAFE_GB[%d],NF_LEV[%d],WDTH[%d],MIN_NUM_LIGH[%d],SREJ[%d],INT[%d],LCO_FDIV[%d],MASK_DIST[%d],DISTANCE[%d],TUN_CAP[%d]\n",(unsigned long)system_get_free_heap_size(),as3935.x0.a0.AFE_GB,as3935.x1.a1.NF_LEV,as3935.x1.a1.WDTH,as3935.x2.a2.MIN_NUM_LIGH,as3935.x2.a2.SREJ,as3935.x3.a3.INT,as3935.x3.a3.LCO_FDIV,as3935.x3.a3.MASK_DIST,as3935.x7.a7.DISTANCE,as3935.x8.a8.TUN_CAP);
	}
}


void pending_interrupt(){
		interrupt_flag=1;
}

void ICACHE_FLASH_ATTR clear_interrupt(){
	//clear interrupt status
	os_printf("clear irq\n");
	
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, GPIO_REG_READ(GPIO_STATUS_ADDRESS) );
	
	as3935_chip_read();
	switch(as3935.x3.a3.INT){
		case IRQ_L:
			os_printf("as3935 lightning event \r\n");
			//check if we are within treshold
			if(threshold_distance>as3935.x7.a7.DISTANCE){
				//switch relay off
				
				GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),0);
				
				//sqedule further checking
				state_machine=6;
			}
			
		break;
		case IRQ_D:
			os_printf("as3935 disturber detected \r\n");
			
		break;
		case IRQ_NF:
			os_printf("as3935 noise lewel to high \r\n");
			//print to web page that we need to change  settings
		break;
	}
}

void ICACHE_FLASH_ATTR setup_interrupt(){
	os_printf("setup interrupt\n");
	//disable interrupt if it is enabled
	ETS_GPIO_INTR_DISABLE();
	//setup a interrupt pin for possitive edge front trigger
	gpio_pin_intr_state_set(GPIO_ID_PIN(INT_PIN),GPIO_PIN_INTR_HILEVEL);//or GPIO_PIN_INTR_POSEDGE
	
	//set callback when interrupt trigeres 
	ETS_GPIO_INTR_ATTACH(pending_interrupt, 0);
	
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
	uint8_t tmp[1000];//output buffer
	
	//reschedule
	system_os_post(procTaskPrio, 0, 0 );
	if(tick_flag==1){
		if(tock==1){
			as3935_chip_read();
			tock=0;
		}
		else{
		//	os_sprintf(tmp,"AFE_GB[%d],NF_LEV[%d],WDTH[%d],MIN_NUM_LIGH[%d],SREJ[%d],INT[%d],LCO_FDIV[%d],MASK_DIST[%d],DISTANCE[%d],TUN_CAP[%d]\n",as3935.x0.a0.AFE_GB,as3935.x1.a1.NF_LEV,as3935.x1.a1.WDTH,as3935.x2.a2.MIN_NUM_LIGH,as3935.x2.a2.SREJ,as3935.x3.a3.INT,as3935.x3.a3.LCO_FDIV,as3935.x3.a3.MASK_DIST,as3935.x7.a7.DISTANCE,as3935.x8.a8.TUN_CAP);
		//	uart0_sendStr(tmp);
			tock=1;
		}

		tick_flag=0;
		if(interrupt_flag){
			clear_interrupt();
			interrupt_flag=0;
			return;
		}
		
		if(state_machine==6){
			as3935_get_lightning_distance();
			if(as3935.x7.a7.DISTANCE<threshold_distance){
				GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),0);//off
				in_detection_counter=0; //reset counter
				uart0_sendStr("danger\n");
				return;
			}else {
				state_machine=5;
				return;
			}
		}
		
		if(state_machine==5){
			if(in_detection_counter>threshold_timeout*60){
					state_machine=4;	
			}
			uart0_sendStr("awaiting timeout\n");
			return;
		}
		
		if(state_machine==4){
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),1);	
			if(as3935.x7.a7.DISTANCE==63){
				state_machine=3;
				
				uart0_sendStr("thunderstorm passed\n");
				return;
			}
		}

		if(state_machine==1){
			as3935_disable_LCO_calibration_mode();
			ETS_GPIO_INTR_DISABLE();
			uart0_sendStr("on\n");
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),1);	
		}
		
		if(state_machine==2){
			ETS_GPIO_INTR_DISABLE();
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),0);	
			uart0_sendStr("lco cal\n");
			as3935_enable_LCO_calibration_mode();
		}
		
		if(state_machine==0){
			as3935_disable_LCO_calibration_mode();
			ETS_GPIO_INTR_DISABLE(); 
			uart0_sendStr("off\n");
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),0);	
			//uart0_sendStr("off\n");
		}
		
		if(state_machine==3){
			as3935_disable_LCO_calibration_mode();
			setup_interrupt();
			GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN),1);	
			uart0_sendStr("thunderstrike protection\n");
		}
	}//tok
}




#define SSID "wlan"
#define SSID_PASSWORD "password"
//Main routine. Initialize stdout, the I/O, filesystem and the webserver and we're done.
void user_init(void) {
	
	char ssid[32] = SSID;
    char password[64] = SSID_PASSWORD;
    struct station_config station_conf;
    
    //Set station mode (connecting to another network)
    wifi_set_opmode( 0x1 );
    //lets just make our own
    //wifi_set_opmode( 0x2 );

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
	
	//enable our working task
	system_os_post(procTaskPrio, 0, 0 );
	//state_machine=0;

	// 0x40200000 is the base address for spi ash memory mapping, ESPFS_POS is the position
	// where image is written in flash that is defined in Makefile.
#ifdef ESPFS_POS
	espFsInit((void*)(0x40200000 + ESPFS_POS));
#else
	espFsInit((void*)(webpages_espfs_start));
#endif
	httpdInit(builtInUrls, 80);

	os_printf("\nReady\n");
}


