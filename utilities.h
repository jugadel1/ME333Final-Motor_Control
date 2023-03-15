#ifndef UTILITIES__H__
#define UTILITIES__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "NU32DIP.h"

typedef enum mode_t {IDLE, PWM, ITEST, HOLD, TRACK}; 
volatile enum mode_t get_mode();
void set_mode(enum mode_t s);

void set_dir(int d);
void set_PWM(int d);
int get_dir();
int get_PWM();

void set_igains(float kp, float ki);
float get_igain_ki();
float get_igain_kp();

void set_pgains(float kp, float ki);
float get_pgain_ki();
float get_pgain_kp();

#endif // UTILITIES__H__
