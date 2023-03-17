#include "utilities.h"

// get/set variables
volatile enum mode_t mode;
volatile int duty = 0;
volatile int dir = 0;

volatile float iKi = 0.12;
volatile float iKp = 0.12;
volatile int ieint = 0.12;

volatile float pKp = 3;
volatile float pKi = 0.1;
volatile float pKd = 2;
volatile float pIREF = 0;
volatile int deg_hold = 0;
volatile float peint = 0;
volatile float pelast = 0;

volatile int StoringData = 1;   // if this flag = 1, currently storing

volatile struct cont_dat cont;

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

// current e_integral
void set_ieint(int d){
	ieint = d;
}
int get_ieint(){
	return ieint;
}

// position gain set/get
void set_pgains(float kp, float ki,float kd){
	pKi = ki;
	pKp = kp;
	pKd = kd;
}
float get_pgain_ki(){
	return pKi;
}
float get_pgain_kp(){
	return pKp;
}
float get_pgain_kd(){
	return pKd;
}
void set_pcontrol_iref(float d){
	pIREF = d;
}
float get_pcontrol_iref(){
	return pIREF;
}
void set_hold(float d){
	deg_hold = d;
}
float get_hold(){
	return deg_hold;
}
void set_peint(float d){
	peint = d;
}
float get_peint(){
	return peint;
}
void set_pedot(float d){
	pelast = d;
}
float get_pedot(){
	return pelast;
}


volatile struct cont_dat get_cont(){
	return cont;
}
void set_iADC(float s, int i){
	cont.iADC[i] =  s;
}
void set_iREF(float s, int i){
	cont.iREF[i] =  s;
}
void write_PosTraj(int i,float step){
	cont.pREF[i] =  step;
}

void set_stor(int i){
	StoringData = i;
}
int get_stor(){
	return StoringData;
}
void set_pADC(float s, int i){
	cont.pADC[i] =  s;
}