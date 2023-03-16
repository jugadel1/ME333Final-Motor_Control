#ifndef UTILITIES__H__
#define UTILITIES__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "NU32DIP.h"

typedef enum mode_t{IDLE, PWM, ITEST, HOLD, TRACK}; 
volatile enum mode_t get_mode();
void set_mode(enum mode_t s);

void set_dir(int d);
void set_PWM(int d);
int get_dir();
int get_PWM();

void set_igains(float kp, float ki);
float get_igain_ki();
float get_igain_kp();

void set_ieint(int d);
int get_ieint();

void set_pgains(float kp, float ki, float kd);
float get_pgain_kp();
float get_pgain_ki();
float get_pgain_kd();

#define PLOTPTS 200        // number of data points to plot
#define DECIMATION 1       // plot every 25th point
#define NUMPOS 200         // number of points in waveform

typedef struct cont_dat{
  float iREF[PLOTPTS];
  float iADC[PLOTPTS];
  float pREF[NUMPOS];
  float pADC[NUMPOS]; 
};

volatile struct cont_dat get_cont();
void set_iADC(float s, int i);
void set_iREF(float s, int i);

void set_stor(int i);
int get_stor();


#endif // UTILITIES__H__
