#include "utilities.h"

// get/set variables
volatile enum mode_t mode;
volatile int duty = 0;
volatile int dir = 0;
volatile int iKi = 0;
volatile int iKp = 0;
volatile int pKi = 0;
volatile int pKp = 0;

// mode set/get
volatile enum mode_t get_mode(void){
	return mode;
}
void set_mode(enum mode_t s){
	mode = s;
}

// PWM set/get
void set_PWM(int d){
	duty = d;
}
void set_dir(int d){
	dir = d;
}
int get_dir(){
	return dir;
}
int get_PWM(){
	return duty;
}

// current gain set/get
void set_igains(float kp, float ki) {
	iKi = ki;
	iKp = kp;
}
float get_igain_ki(){
	return iKi;
}
float get_igain_kp(){
	return iKp;
}

// position gain set/get
void set_pgains(float kp, float ki) {
	pKi = ki;
	pKp = kp;
}
float get_pgain_ki(){
	return pKi;
}
float get_pgain_kp(){
	return pKp;
}

void makeWaveform(){
    int i = 0, center = 0, A = 200; // square wave, changes from -200mA to 200 mA
    for (i = 0; i < NUMSAMPS; ++i){
        if (i < NUMSAMPS / 2){
            Waveform[i] = center + A;
        }
        else{
            Waveform[i] = center - A;
        }
    }
}