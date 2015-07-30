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
