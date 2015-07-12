/*
 * Uroš Golob <golob.uros@gmail.com> wrote this file.
 * 
 * Work was done with intention to protect electronic devices and so lower e-waste.
 * It might work or it may not, just don't sue me if your house burns down or if it kills you...
 * 
 * This software and hardware is described in my diploma thesis(work in progress). 
 *   
 * Use it for personal use as you wish, just keep this notice on it if you do! 
 * And return changes back free of charge by same restrictions and notice! 
 * 
 * For comercial use you can contact me and we can make a deal...
 *
 * Development took alot of time and money so remember me with some spare 
 * bitcoins: 1MaGphnuiMjywjQPKHKF239wzcef4KpNxX if you wish:)
 * 
 * This software uses espressif sdk software, spi library from David Ogilvy(MetalPhreak) and libesphttpd & wifi settins form Jeroen Domburg(sprite_fm). So use and rescpect their respective licencies for their work. 
 */

#include <esp8266.h>
#include "cgi.h"
#include "as3935.h"

int ICACHE_FLASH_ATTR cgias3935(HttpdConnData *connData) {
	char buff[128];
	
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	if(httpdFindArg(connData->post->buff, "boost", buff, sizeof(buff))>0){	
		as3935_set_AFE_gainboost(atoi(buff));
		goto end;	
	}
	if(httpdFindArg(connData->post->buff, "noise", buff, sizeof(buff))>0){	
		as3935_set_noise_floor_level(atoi(buff));
		goto end;	
	}
	if(httpdFindArg(connData->post->buff, "wdth", buff, sizeof(buff))>0){	
		as3935_set_watchdog_thresold(atoi(buff));	
		goto end;	
	}
	if(httpdFindArg(connData->post->buff, "clear", buff, sizeof(buff))>0){	
		as3935_clear_stat();
		goto end;		
	}
	if(httpdFindArg(connData->post->buff, "min", buff, sizeof(buff))>0){
		as3935_set_min_lightning_events(atoi(buff));
		
		if(atoi(buff)==1){
			as3935_set_min_lightning_events(0);	
		}
		else if(atoi(buff)==5){
			as3935_set_min_lightning_events(1);	
		}
		else if(atoi(buff)==9){
			as3935_set_min_lightning_events(2);	
		}
		else if(atoi(buff)==15){
			as3935_set_min_lightning_events(3);	
		}
		else
		{
			as3935_set_min_lightning_events(0);
			os_printf("\nouch i made a bubu min light events wrong\n");		
		}
		goto end;

	}
	if(httpdFindArg(connData->post->buff, "spike", buff, sizeof(buff))>0){	
		as3935_set_spike_rejection(atoi(buff));
		goto end;
	}
	if(httpdFindArg(connData->post->buff, "refresh", buff, sizeof(buff))>0){
		goto end;
	}
/*	if(httpdFindArg(connData->post->buff, "disp", buff, sizeof(buff))>0){	
		state_machine=atoi(buff);
		if(state_machine==2){
			disable_interrupt();
			as3935_enable_LCO_calibration_mode();
		}else{
			as3935_disable_LCO_calibration_mode();
		}
		goto end;
	}
	if(httpdFindArg(connData->post->buff, "div", buff, sizeof(buff))>0){	
	//	int i;	os_printf("\ntestlco %d,%d,%d testdiv\n",lcofdiv[as3935.x3.a3.LCO_FDIV],as3935.x3.a3.LCO_FDIV,atoi(buff));
		uint8_t n = atoi(buff);
		if(lcofdiv[0]==n){	
			as3935_set_lco_divider(0);
		}
		else if(lcofdiv[1]==n){	
			as3935_set_lco_divider(1);
		}
		else if(lcofdiv[2]==n){	
			as3935_set_lco_divider(2);
		}
		else if(lcofdiv[3]==n){	
			as3935_set_lco_divider(3);
		}
		goto end;
	}*/
	if(httpdFindArg(connData->post->buff, "cap", buff, sizeof(buff))>0){
		uint8_t i;
		uint8_t num=atoi(buff);
		for( i=0;i<=0xf;i++){
			os_printf("\na2i: %d, %d, %d \n",num,tuncaplookuptable[i],i );
			if(tuncaplookuptable[i]==num){	
				os_printf("ok\n");
				as3935_set_tuning_capacitor(i);
				break;
			}
			
		}	 
		goto end;
	}
	if(httpdFindArg(connData->post->buff, "distance", buff, sizeof(buff))>0){
		 threshold_distance=atoi(buff);		
		 goto end;
	}
	if(httpdFindArg(connData->post->buff, "timeout", buff, sizeof(buff))>0){
		threshold_timeout=atoi(buff);
		goto end;
	}
	if(httpdFindArg(connData->post->buff, "state", buff, sizeof(buff))>0){	
		state_machine=atoi(buff);
		if(state_machine==2){
			disable_interrupt();
			//as3935_set_tuning_capacitor(0);
			as3935_enable_LCO_calibration_mode();
		}else{
			as3935_disable_LCO_calibration_mode();
		}
		goto end;
	}
		
	as3935_chip_read();
	
end:
#define TEST
#ifdef TEST
	os_printf("\ntest\n");
	os_printf(buff);
	os_printf("\ntest\n");
#endif
	httpdRedirect(connData, "index.tpl");
	return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR tplas3935(HttpdConnData *connData, char *token, void **arg) {

	if (token==NULL) return HTTPD_CGI_DONE;
	
	//refresh data in as3935 struct...
	as3935_chip_read();
	char buff[128];
	os_strcpy(buff, "Unknown");

	if (os_strcmp(token, "timeout_threshold")==0) {
			os_sprintf(buff,"%d",threshold_timeout);
	}	
	
	if (os_strcmp(token, "distance_threshold")==0) {
			os_sprintf(buff,"%d",threshold_distance);
	}
	
	if (os_strcmp(token, "TUN_CAP")==0) {
			os_sprintf(buff,"%d",tuncaplookuptable[as3935.x8.a8.TUN_CAP]);
	}
	
/*	if (os_strcmp(token, "LCO_FDIV")==0) {
		os_sprintf(buff,"%d",lcofdiv[as3935.x3.a3.LCO_FDIV]);
	}*/
	/*
	if (os_strcmp(token, "DISP_LCO")==0) {
		if (state_machine==2){
			os_strcpy(buff, "ENABLED");
			}else{os_strcpy(buff, "DISABLED");
			}
	}*/
	
	if (os_strcmp(token, "DISTANCE")==0) {
			os_sprintf(buff,"%d",as3935.x7.a7.DISTANCE);
	}	
	
	if (os_strcmp(token, "SREJ")==0) {
	os_sprintf(buff,"%d",as3935.x2.a2.SREJ);
	}	
	
	if (os_strcmp(token, "WDTH")==0) {
	os_sprintf(buff,"%d",as3935.x1.a1.WDTH);
	}	
	
	if (os_strcmp(token, "NF_LEV")==0) {
	os_sprintf(buff,"%d",as3935.x1.a1.NF_LEV);
	}	
	
	if (os_strcmp(token, "AFE_GB")==0) {
		if(as3935.x0.a0.AFE_GB==18){
			os_strcpy(buff,"INDOOR");
		}else{
			os_strcpy(buff,"OUTDOOR");
		}
	}
	
	if (os_strcmp(token, "MIN_NUM_LIGH")==0) {
	os_sprintf(buff,"%d",min_num_ligh[as3935.x2.a2.MIN_NUM_LIGH]);
	}
	
	
	
	
	if (os_strcmp(token, "relay_state")==0) {
		switch(state_machine){
				case 0:
				os_strcpy(buff, "off");
				break;
				case 1:
				os_strcpy(buff, "on");
				break;
				case 2:
				os_strcpy(buff, "calibration");
				break;
				case 3:
				os_strcpy(buff, "listening");
				break;
				case 4:
				os_strcpy(buff, "in detection > distance ");
				break;
				case 5:
				os_strcpy(buff, "in detection > distance timeout ");
				break;
				case 6:
				os_strcpy(buff, "in detection < distance");
				break;
				case 7:
				os_strcpy(buff, "in detection < distance timeout");
				break;
			}
	}
	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

