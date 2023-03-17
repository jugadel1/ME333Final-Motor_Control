#ifndef PCONTROL__H__
#define PCONTROL__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "NU32DIP.h"

#include "utilities.h"
#include "NU32DIP.h" // constants, functions for startup and UART
#include "encoder.h"
#include "i2c_master_noint.h"
#include "ina219.h"
#include "icontrol.h"

void pcontrolstartup();
int get_deg();
#endif // UTILITIES__H__
