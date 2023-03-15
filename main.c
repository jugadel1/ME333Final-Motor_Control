#include "nu32dip.h" // constants, funcs for startup and UART
#include "encoder.h"
#include "i2c_master_noint.h"
#include "ina219.h"
#include "utilities.h"
#include "icontrol.h"

#define BUF_SIZE 200

int main(){
  char buffer[BUF_SIZE];
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init

  NU32DIP_YELLOW = 1;  // turn off the LEDs
  NU32DIP_GREEN = 1;

  __builtin_disable_interrupts(); // in future, initialize modules or peripherals here
  
  UART2_Startup(); // initialize PICO encoder UART comm
  INA219_Startup(); // initialize ina219 current I2C comm  
  set_mode(IDLE); //initialize mode at IDLE
  icontrolstartup(); //initialize timers 

  __builtin_enable_interrupts();
  makeWaveform();          // Creates REF waveform square wave 
  
  while (1){
    NU32DIP_ReadUART1(buffer, BUF_SIZE); // we expect the next character to be a menu command
    NU32DIP_YELLOW = 1;               // clear the error LED
    switch (buffer[0]){
    case 'a':{ // Read current         
      float curr;
      curr = INA219_read_current();
      sprintf(buffer, "%f\r\n", curr);
      NU32DIP_WriteUART1(buffer);
      break;
    }    
    case 'c':{ // Read encoder (counts)          
      int count;
      WriteUART2("a"); // request the encoder count

      while(!get_encoder_flag()){;} // wait for the Pico to respond
      set_encoder_flag(0); // clear the flag so you can read again later
      count = get_encoder_count(); // get the encoder value

      sprintf(buffer, "%d\r\n", count);
      NU32DIP_WriteUART1(buffer);
      break;
    }

    case 'd':{ // output motor pos in deg
      int count;
      int deg;
      WriteUART2("a"); // request the encoder count

      while(!get_encoder_flag()){;} // wait for the Pico to respond
      set_encoder_flag(0); // clear the flag so you can read again later
      count = get_encoder_count(); // get the encoder value

      deg = count*0.107; // cnt*360(deg/rev)/3360(cnt/rev)
      sprintf(buffer, "%d\r\n", deg);
      NU32DIP_WriteUART1(buffer);
      break;
    }

    case 'e':{ // reset the encoder count to 32768
      // NU32DIP_WriteUART1("Resetting encoder count.\r\n"); 
      WriteUART2("b");
      break;       
    }

    case 'f':{ // (Set PWM (-100 to 100)).
      int pwmval;
      int proc;
      NU32DIP_WriteUART1('Enter your desired PWM duty (-100 to 100):');
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%d", &pwmval);  
      
      if (pwmval <= -100){
        pwmval = -100;
      }
      else if (pwmval >= 100){
        pwmval = 100;
      }

      if (pwmval >= 0){
        set_dir(1);
        proc = pwmval;
      }
      else{
        set_dir(0);
        proc = -pwmval;
      }

      set_PWM(proc);
      set_mode(PWM);
      
      sprintf(buffer, "%d\r\n", get_PWM()); // return the number + 1
      NU32DIP_WriteUART1(buffer);
      sprintf(buffer, "%d\r\n", get_dir()); // return the number + 1
      NU32DIP_WriteUART1(buffer);
      break;       
    }
    case 'g':{ // Set current gains.
      int kp;
      int ki;
      NU32DIP_WriteUART1('Enter your desired Kp current gain [recommended: _]:\r\n');
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f", &kp);        
      
      NU32DIP_WriteUART1('Enter your desired Ki current gain [recommended: _]:\r\n');
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f", &ki);        

      sprintf(buffer, "Sending Kp = %f and Ki = %f to the current controller.\r\n",kp,ki);
      NU32DIP_WriteUART1(buffer);
      set_igains(kp,ki);
      break;       
    }
    case 'h':{ // Get current gains.
      int kp;
      int ki; 

      sprintf(buffer, "The current controller is using Kp = %f and Ki = %f.\r\n",kp,ki);
      NU32DIP_WriteUART1(buffer);
      break;       
    }
    case 'g':{ // Set position gains.
      int kp;
      int ki;
      NU32DIP_WriteUART1('Enter your desired Kp position gain [recommended: _]:\r\n');
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f", &kp);        
      
      NU32DIP_WriteUART1('Enter your desired Ki position gain [recommended: _]:\r\n');
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f", &ki);        

      sprintf(buffer, "Sending Kp = %f and Ki = %f to the position controller.\r\n",kp,ki);
      NU32DIP_WriteUART1(buffer);
      set_pgains(kp,ki);
      break;       
    }
    case 'k':{ // unpower motor
      set_mode(ITEST);
      
      static volatile int eint = 0; // calc eint
      StoringData = 1;                // message to ISR to start storing data
      while (StoringData){            // wait until ISR says data storing is done
          _nop(); // do nothing 
      }

      for (i = 0; i < PLOTPTS; i++){  // send plot data to MATLAB
                                      // when first number sent = 1, MATLAB knows we’re done
      sprintf(message, "%d %d %d\r\n", PLOTPTS - i, ADCarray[i], REFarray[i]);
      NU32DIP_WriteUART1(message);
      break;       
    }
    case 'p':{ // unpower motor
      set_mode(IDLE);
      set_PWM(0);
      set_dir(0);
      break;       
    }
    case 'r':{ // get mode
      enum mode_t m; // IDLE, PWM, ITEST, HOLD, TRACK
      m = get_mode();
      if (m == IDLE){
        NU32DIP_WriteUART1("IDLE\r\n"); 
      }
      if (m == PWM){
        NU32DIP_WriteUART1("PWM\r\n"); 
      }
      if (m == ITEST){
        NU32DIP_WriteUART1("ITEST\r\n"); 
      }
      if (m == HOLD){
        NU32DIP_WriteUART1("HOLD\r\n"); 
      } 
      if (m == TRACK){
        NU32DIP_WriteUART1("TRACK\r\n"); 
      }     
      break;
    }
    case 'z':{ // dummy function (in+1)  
      int n = 0;
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%d", &n);
      sprintf(buffer, "%d\r\n", n + 1); // return the number + 1
      NU32DIP_WriteUART1(buffer);
      break;
    }
    case 'q':{
      // handle q for quit. Later you may want to return to IDLE mode here.
      set_mode(IDLE);
      break;
    }
    default:{
      NU32DIP_YELLOW = 0; // turn on LED2 to indicate an error
      break;
    }
    }
  }
  return 0;
}