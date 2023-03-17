#include "pcontrol.h"

void __ISR(_TIMER_4_VECTOR, IPL4SOFT) pcontroller(void){  // _TIMER_4_VECTOR = ? 
    switch(get_mode()){
        case HOLD:{
            static int counter = 0;
            static int pos;
            static int pos_hold;
            pos = get_deg();
            pos_hold = get_hold();

            // calculate u using control values
            static float e;                         
            e = pos_hold - pos;
            e = -e;   
            // calculate eint 
            static float eint;
            eint = get_peint() + e;
            if (eint > 1000){ // integrator anti-windup
                eint = 1000;
            }
            if (eint < -1000){
                eint = -1000;
            }
            set_peint(eint);
            
            // calculate ederiv
            static float ederiv;
            ederiv = e - get_pedot();
            set_pedot(e);

            // get Kx constant gains
            float Ki = get_pgain_ki();
            float Kp = get_pgain_kp();
            float Kd = get_pgain_kd();

            float u = Kp*e + Ki*eint + Kd*ederiv; // calc u    
            float unew = (u + 0); // treat u as % centered at 0%
            if (unew > 1000.0){  // keep i under 1A
                unew = 1000.0;
            }
            else if (unew < -1000.0){
                unew = -1000.0;
            }
            static char buffer[200];
            sprintf(buffer, "current pos control: %f, e = %f, eint = %f, ed = %f\r\n", unew,e,eint,ederiv);
            NU32DIP_WriteUART1(buffer);

            set_pcontrol_iref(unew);
            counter++;
            if (counter > 1000){
                counter = 0;
                set_mode(IDLE);
            }
            break;
        }
        case TRACK:{
            static int counter = 0;
            static int pos;
            static int pos_hold;
            pos = get_deg();
            pos_hold = get_hold();

            // calculate u using control values
            static float e;                         
            e = pos_hold - pos;
            e = -e;   
            // calculate eint 
            static float eint;
            eint = get_peint() + e;
            if (eint > 1000){ // integrator anti-windup
                eint = 1000;
            }
            if (eint < -1000){
                eint = -1000;
            }
            set_peint(eint);
            
            // calculate ederiv
            static float ederiv;
            ederiv = e - get_pedot();
            set_pedot(e);

            // get Kx constant gains
            float Ki = get_pgain_ki();
            float Kp = get_pgain_kp();
            float Kd = get_pgain_kd();

            float u = Kp*e + Ki*eint + Kd*ederiv; // calc u    
            float unew = (u + 0); // treat u as % centered at 0%
            if (unew > 1000.0){  // keep i under 1A
                unew = 1000.0;
            }
            else if (unew < -1000.0){
                unew = -1000.0;
            }
            set_pcontrol_iref(unew);
            counter++;
            if (counter > 1000){
                counter = 0;
                set_mode(IDLE);
            }
            break;                 
        }
    }  
    IFS0bits.T4IF = 0; // clear interrupt flag   
}

void pcontrolstartup(){
    // 200 hz TMR4 ISR
    PR4 = 3750 - 1;                // set period register
    TMR4 = 0;                       // initialize count to 0
    T4CONbits.TCKPS = 0b110;            // set prescaler to 1
    T4CONbits.ON = 1;               // turn on Timer4
    IPC4bits.T4IP = 4;              // INT step 4: priority
    IPC4bits.T4IS = 0;              // subpriority
    IFS0bits.T4IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T4IE = 1;              // INT step 6: enable interrupt
    __builtin_enable_interrupts();  // INT step 7: enable interrupts at CPU
    // end of initialize interrupt for posn control
}

int get_deg(){
    int count;
    int deg;
    WriteUART2("a"); // request the encoder count
    while(!get_encoder_flag()){;} // wait for the Pico to respond
    set_encoder_flag(0); // clear the flag so you can read again later
    count = get_encoder_count(); // get the encoder value

    deg = count*0.107; // cnt*360(deg/rev)/3360(cnt/rev)
    return deg;
}


