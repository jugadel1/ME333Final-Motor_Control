#include "nu32dip.h" // constants, funcs for startup and UART
#include "encoder.h"
#include "i2c_master_noint.h"
#include "ina219.h"
#include "utilities.h"
#include "icontrol.h"
#include "pcontrol.h"
#define BUF_SIZE 200

int main(){
  char buffer[BUF_SIZE];
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init

  NU32DIP_YELLOW = 0;  // turn on ON LED
  NU32DIP_GREEN = 1;   // turn off problem LED

  __builtin_disable_interrupts(); // in future, initialize modules or peripherals here
  UART2_Startup(); // initialize PICO encoder UART comm
  INA219_Startup(); // initialize ina219 current I2C comm  
  set_mode(IDLE); //initialize mode at IDLE
  icontrolstartup(); //initialize PWM (TM3) and I control (TMR2)
  pcontrolstartup(); //initialize Posn control TMR4
  __builtin_enable_interrupts();

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
      WriteUART2("b");
      break;       
    }

    case 'f':{ // (Set PWM (-100 to 100)).
      int pwmval;
      int proc;
      // NU32DIP_WriteUART1('Enter your desired PWM duty (-100 to 100):');
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
      float kp;
      float ki;
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f %f", &kp, &ki);  

      sprintf(buffer, "Sending Kp = %f and Ki = %f to the current controller.\r\n",kp,ki);
      NU32DIP_WriteUART1(buffer);
      set_igains(kp,ki);
      break;       
    }
    case 'h':{ // Get current gains.
      float kp;
      float ki; 
      
      kp = get_igain_kp();
      ki = get_igain_ki();
      sprintf(buffer, "The current controller is using Kp = %f and Ki = %f.\r\n",kp,ki);
      NU32DIP_WriteUART1(buffer);
      break;       
    }
    case 'i':{ // Set position gains.
      float kp;
      float ki;
      float kd;
      
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f %f %f", &kp, &ki, &kd);       
      
      sprintf(buffer, "Sending Kp = %f and Ki = %f and Kd = %f to the position controller.\r\n",kp,ki,kd);
      NU32DIP_WriteUART1(buffer);
      set_pgains(kp,ki,kd);
      break;       
    }
    case 'j':{ // Get position gains.
      float kp;
      float ki; 
      float kd;      
      kp = get_pgain_kp();
      ki = get_pgain_ki();
      kd = get_pgain_kd();
      sprintf(buffer, "The position controller is using Kp = %f and Ki = %f and Kd = %f.\r\n",kp,ki,kd);
      NU32DIP_WriteUART1(buffer);
      break;       
    }
    case 'k':{ // test current gains
      sprintf(buffer, "200\r\n");
      NU32DIP_WriteUART1(buffer);
      set_ieint(0);     
      set_mode(ITEST);

      set_stor(1);
      while (get_stor()){            // wait until ISR says data storing is done
          _nop(); // do nothing 
      }

      struct cont_dat curr = get_cont();
      for (int i = 0; i < PLOTPTS; i++){  // send plot data to MATLAB
                                      // when first number sent = 1, MATLAB knows we’re done
      sprintf(buffer, "%f %f\r\n", curr.iREF[i], curr.iADC[i]);
      NU32DIP_WriteUART1(buffer);
      }
      set_mode(IDLE);
      break;       
    }
    case 'l':{ // go to hold angle 
      WriteUART2("b");
      int des_deg;
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%d", &des_deg);                 
      
      sprintf(buffer, "Sending desired angle = %d to the position controller.\r\n",des_deg);
      NU32DIP_WriteUART1(buffer);
      set_peint(0);
      set_pedot(0);
      set_hold(des_deg);
      set_mode(HOLD);
      break;       
    }
    case 'm':{ // load step trajectory
      int len;
      float step;
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%d", &len);     
      for(int i=0; i<len; i++){
        NU32DIP_ReadUART1(buffer, BUF_SIZE);
        sscanf(buffer, "%f", &step);
        write_PosTraj(i,step);     
      }
      sprintf(buffer, "Received Step Trajectory.\r\n");
      NU32DIP_WriteUART1(buffer);
      break;       
    }
    case 'n':{ // load cubic trajectory
      int len;
      float step;
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%d", &len);     
      for(int i=0; i<len; i++){
        NU32DIP_ReadUART1(buffer, BUF_SIZE);
        sscanf(buffer, "%f", &step);
        write_PosTraj(i,step);     
      }
      sprintf(buffer, "Received Cubic Trajectory.\r\n");
      NU32DIP_WriteUART1(buffer);
      break;         
    }
    case 'o':{ // execute trajectory
      WriteUART2("b");
      sprintf(buffer, "1000\r\n");
      NU32DIP_WriteUART1(buffer);

      set_pedot(0);
      set_peint(0);     
      set_mode(TRACK);

      set_stor(1);
      while (get_stor()){            // wait until ISR says data storing is done
          _nop(); // do nothing 
      }

      struct cont_dat curr = get_cont();
      for (int i = 0; i < NUMPOS; i++){  // send plot data to MATLAB
                                      // when first number sent = 1, MATLAB knows we’re done
      sprintf(buffer, "%f %f\r\n", curr.pREF[i], curr.pADC[i]);
      NU32DIP_WriteUART1(buffer);
      }
      set_mode(IDLE);
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
  } //end switch
  } //end while
  return 0;
} // end main