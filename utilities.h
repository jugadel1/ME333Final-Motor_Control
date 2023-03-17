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

// typedef struct p_cont{
//   float kp;
//   float ki;
//   float kd;
//   float iRef;
//   int eint;
//   int elast;
// };

void set_pgains(float kp, float ki, float kd);
float get_pgain_kp();
float get_pgain_ki();
float get_pgain_kd();
float get_pcontrol_iref();
void set_pcontrol_iref(float d);
void set_hold(float d);
float get_hold();
void set_peint(float d);
float get_peint();
void set_pedot(float d);
float get_pedot();

#define PLOTPTS 200        // number of data points to plot
#define DECIMATION 1       // plot every 25th point
#define NUMPOS 1000         // number of points in waveform

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
void write_PosTraj(int i,float step);

#endif // UTILITIES__H__
