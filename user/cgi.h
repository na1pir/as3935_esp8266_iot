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
#ifndef CGI_H
#define CGI_H

#include "httpd.h"
#include "as3935.h"

struct settings_t{
  uint8 data0;
  uint8 data1;
  uint8 data2;
  uint8 data8;
  uint8 state;
  uint8 threshdistance;
  uint8 timeout;
  uint8 padd;
  };
  

extern struct as3935_t as3935;
struct settings_t settings;
int cgias3935(HttpdConnData *connData);
int tplas3935(HttpdConnData *connData, char *token, void **arg);

#endif
