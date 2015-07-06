#ifndef CGI_H
#define CGI_H

#include "httpd.h"
#include "as3935.h"
extern struct as3935_t as3935;
int cgias3935(HttpdConnData *connData);
int tplas3935(HttpdConnData *connData, char *token, void **arg);

#endif
