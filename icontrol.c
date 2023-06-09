#include "icontrol.h"


// extern cont_dat cont;

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) icontroller(void){  // _TIMER_2_VECTOR = 8
    int d;
    int dirxn;
    
    switch(get_mode()){
        case IDLE:{
            d = 0;
            dirxn = 0;
            OC1RS = (unsigned int) ((d/100.0) * PR3); // set duty based on PWM
            LATAbits.LATA1 = dirxn; // set direction pin   
            break; 
        }
        case PWM:{
            d = get_PWM();
            dirxn = get_dir();
            OC1RS = (unsigned int) ((d/100.0) * PR3); // set duty based on PWM
            LATAbits.LATA1 = dirxn; // set direction pin
            break;                 
        }
        case ITEST:{
            static int counter = 0; // initialize counter once
            static int plotind = 0; // index for data arrays; counts up to PLOTPTS
            static int decctr = 0; // counts to store data one every DECIMATION
            static float adcval = 0;
            static float ma = 200.0; 

            adcval = INA219_read_current();
            
            // calculate u using control values
            static int e = 0; 
            struct cont_dat curr = get_cont();
                        
            e =  ma - adcval; // calc e
            int eint = get_ieint() + e;
            if (eint > 1000){ // integrator anti-windup
                eint = 1000;
            }
            if (eint < -1000){
                eint = -1000;
            }
            set_ieint(eint);

            float iKi = get_igain_ki();
            float iKp = get_igain_kp();

            float u = iKp*e + iKi*eint; // calc u    
            float unew = u + 0; // treat u as % centered at 0%
            if (unew > 100.0){
                unew = 100.0;
                dirxn = 1;
            }
            if (unew > 0 && unew <100){
                unew = unew;
                dirxn = 1;
            }
            if (unew < 0 && unew > -100.0){
                unew = -unew;
                dirxn = 0;  //unsure
            }
            if (unew < -100.0){
                unew = 100.0;
                dirxn = 0;  //unsure
            }

            OC1RS = (unsigned int) ((unew/100.0) * PR3); // set duty based on PWM
            LATAbits.LATA1 = dirxn; // set direction pin

            if (get_stor()) {
            decctr++;
                if (decctr == DECIMATION) { // after DECIMATION control loops,
                    decctr = 0; // reset decimation counter
                    // curr.iADC[plotind] = adcval; // store data in global arrays
                    // curr.iREF[plotind] =  ma;
                    set_iADC(adcval,plotind);
                    set_iREF(ma,plotind);
                    plotind++; // increment plot data index
                }
                if (plotind == PLOTPTS) { // if max number of plot points plot is reached,
                    plotind = 0; // reset the plot index
                    set_stor(0); // tell main data is ready to be sent to python
                }
            }
            // rest of interrupt
            counter++; // add one to counter every time ISR is entered
            if (counter == PLOTPTS ){
                counter = 0; // roll the counter over when needed
            }
            if ((counter % 25)==0){
                ma = -ma;
            }
        }
        case HOLD:{
            static float adcval = 0;
            static float ma;
            ma = get_pcontrol_iref();
            adcval = INA219_read_current();
            
            // calculate u using control values
            static int e = 0;                       
            e =  ma - adcval; // calc e
            int eint = get_ieint() + e;
            if (eint > 1000){ // integrator anti-windup
                eint = 1000;
            }
            if (eint < -1000){
                eint = -1000;
            }
            set_ieint(eint);

            float iKi = get_igain_ki();
            float iKp = get_igain_kp();

            float u = iKp*e + iKi*eint; // calc u    
            float unew = u + 0; // treat u as % centered at 0%
            if (unew > 100.0){
                unew = 100.0;
                dirxn = 1;
            }
            if (unew > 0 && unew <100){
                unew = unew;
                dirxn = 1;
            }
            if (unew < 0 && unew > -100.0){
                unew = -unew;
                dirxn = 0;  //unsure
            }
            if (unew < -100.0){
                unew = 100.0;
                dirxn = 0;  //unsure
            }
            OC1RS = (unsigned int) ((unew/100.0) * PR3); // set duty based on PWM
            LATAbits.LATA1 = dirxn; // set direction pin
        }
        case TRACK:{
            static float adcval = 0;
            static float ma;
            ma = get_pcontrol_iref();
            adcval = INA219_read_current();
            
            // calculate u using control values
            static int e = 0;                       
            e =  ma - adcval; // calc e
            int eint = get_ieint() + e;
            if (eint > 1000){ // integrator anti-windup
                eint = 1000;
            }
            if (eint < -1000){
                eint = -1000;
            }
            set_ieint(eint);

            float iKi = get_igain_ki();
            float iKp = get_igain_kp();

            float u = iKp*e + iKi*eint; // calc u    
            float unew = u + 0; // treat u as % centered at 0%
            if (unew > 100.0){
                unew = 100.0;
                dirxn = 1;
            }
            if (unew > 0 && unew <100){
                unew = unew;
                dirxn = 1;
            }
            if (unew < 0 && unew > -100.0){
                unew = -unew;
                dirxn = 0;  //unsure
            }
            if (unew < -100.0){
                unew = 100.0;
                dirxn = 0;  //unsure
            }
            OC1RS = (unsigned int) ((unew/100.0) * PR3); // set duty based on PWM
            LATAbits.LATA1 = dirxn; // set direction pin
        }
    }
    IFS0bits.T2IF = 0; // clear interrupt flag
    
}

void icontrolstartup(){

    RPA0Rbits.RPA0R = 0b0101; // use pin A0 for OC1
    volatile int duty = 25;
    volatile int dir = 0;
    TRISAbits.TRISA1 = 0; // configure pin A1 to output dirxn
    
    /* 20 kHz PWM, Timer3, duty cycle 75% */
    __builtin_disable_interrupts(); // INT step 2: disable interrupts at CPU
    T3CONbits.TCKPS = 0;    // Timer3 prescaler N=1 (1:1)
    PR3 = 2400 - 1;         // PR = PBCLK(48MHz)/20KHz(Desired PWM)-1
    TMR3 = 0;               // initial TMR3 count is 0
    OC1CONbits.OCTSEL = 1; // chooses TMR3 for OC1 
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC1RS = 0;              // duty cycle = OC1RS/(PR3+1) = 75%
    OC1R = 0;               // initialize before turning OC1 on; afterward it is read-only
    T3CONbits.ON = 1;       // turn on Timer3
    OC1CONbits.ON = 1;      // turn on OC1

    // 5 Khz TMR2 ISR
    PR2 = 9600 - 1;                // set period register
    TMR2 = 0;                       // initialize count to 0
    T2CONbits.TCKPS = 0;            // set prescaler to 1
    T2CONbits.ON = 1;               // turn on Timer2
    IPC2bits.T2IP = 5;              // INT step 4: priority
    IPC2bits.T2IS = 0;              // subpriority
    IFS0bits.T2IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T2IE = 1;              // INT step 6: enable interrupt
    __builtin_enable_interrupts();  // INT step 7: enable interrupts at CPU
    // end of initialize interrupt for waveform
}

