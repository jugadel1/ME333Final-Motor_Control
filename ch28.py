# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

from genref import genRef
import serial
import matplotlib.pyplot as plt 
from statistics import mean 

ser = serial.Serial('COM3',230400)
print('Opening port: ')
print(ser.name)

def read_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str.decode()) # turn it into an int

    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        # print(dat_str.decode())
        dat_f = list(map(float,dat_str.split())) # now the data is a list
        ref.append(dat_f[0])
        data.append(dat_f[1])
        data_received = data_received + 1
    
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\ta: get motor current \t c: get encoder counts \t  d: get out shaft degrees\n \
    \te: reset encoder counts\t f: Set PWM (-100 to 100) g: Set current gains\n \
    \th: Get current gains \t i: Set position gains \t  j: Get position gains\n \
    \tk: Test current control  l: Go to angle (deg) \t  m: Load step trajectory\n \
    \tn: Load cubic trajectory o: Execute trajectory \t  p: Unpower the motor\n\
    \tr: get current mode \t z: dummy case \t\t  q: Quit') # '\t' is a tab
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'z'):
        # example operation
        n_str = input('Enter number: ') # get the number to send
        n_int = int(n_str) # turn it into an int
        print('number = ' + str(n_int)) # print it to the screen to double check

        ser.write((str(n_int)+'\n').encode()); # send the number
        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('Got back: ' + str(n_int) + '\n') # print it to the screen
    
    elif (selection == 'a'):
        print('Requesting motor current...')
        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = float(n_str) # turn it into an int
        print('Current motor current: ' + str(n_int) + ' mA' + '\n') # print it to the screen        
    
    elif (selection == 'c'):
        print('Requesting encoder count...')
        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('Current encoder count: ' + str(n_int) + '\n') # print it to the screen        
    
    elif (selection == 'd'):
        print('Requesting out shaft angle...')
        n_str = ser.read_until(b'\n');
        n_int = int(n_str) # turn it into an int
        print('Current out shaft angle: ' + str(n_int) + '\n') # print it to the screen        
        
    elif (selection == 'e'):
        print('Resetting encoder count.')
    
    elif (selection == 'f'): # 
        # n_str = ser.read_until(b'\n');  # get the incremented number back
        # print(n_str + '\n') # print it to the screen  
        
        n_str = input('Enter your desired PWM duty (-100 to 100):') # get the number to send
        n_int = int(n_str) # turn it into an int
        print('PWM = ' + str(n_int)) # print it to the screen to double check

        ser.write((str(n_int)+'\n').encode()); # send the number

        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('Current PWM is: ' + str(n_int) + '\n') # print it to the screen  
        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = int(n_str) # turn it into an int    
        print('Current direction is: ' + str(n_int) + '\n') # print it to the screen          
    
    elif (selection == 'g'): # Set Current Gains
        n_str = input('Enter your desired Kp current gain [recommended: 0.12]:\r\n') # get the number to send
        n_int = float(n_str) # turn it into an int
        print('Kp = ' + str(n_int)) # print it to the screen to double check
        ser.write((str(n_int)).encode()); # send the number

        n_str = input('Enter your desired Ki current gain [recommended: 0.10]:\r\n') # get the number to send
        n_int = float(n_str) # turn it into an int
        print('Ki = ' + str(n_int)) # print it to the screen to double check
        ser.write((str(n_int)+'\n').encode()); # send the number
        
        n_str = ser.read_until(b'\n');  # get the incremented number back
        print(n_str.decode() + '\n') # print it to the screen  
    elif (selection == 'h'):
        n_str = ser.read_until(b'\n');  # get the incremented number back
        print(n_str.decode() + '\n') # print it to the screen  
    
    elif (selection == 'i'):
        n_str = input('Enter your desired Kp position gain [recommended: _]:\r\n') # get the number to send
        n_int = float(n_str) # turn it into an int
        print('Kp = ' + str(n_int)) # print it to the screen to double check
        ser.write((str(n_int)).encode()); # send the number

        n_str = input('Enter your desired Ki position gain [recommended: _]:\r\n') # get the number to send
        n_int = float(n_str) # turn it into an int
        print('Ki = ' + str(n_int)) # print it to the screen to double check
        ser.write((str(n_int)+'\n').encode()); # send the number

        n_str = input('Enter your desired Kd position gain [recommended: _]:\r\n') # get the number to send
        n_int = float(n_str) # turn it into an int
        print('Ki = ' + str(n_int)) # print it to the screen to double check
        ser.write((str(n_int)+'\n').encode()); # send the number
        
        n_str = ser.read_until(b'\n');  # get the incremented number back
        print(n_str.decode() + '\n') # print it to the screen  
    
    elif (selection == 'j'):
        n_str = ser.read_until(b'\n');  # get the incremented number back
        print(n_str.decode() + '\n') # print it to the screen      
    
    elif (selection == 'k'):
        print('Starting I control gains testing')
        read_plot_matrix()  
      
    elif (selection == 'p'):
        print('Unpowering motor, setting mode=IDLE.\n')

    
    elif (selection == 'r'):
        print('Requesting current mode...')
        n_str = ser.read_until(b'\n');  # get the incremented number back
        print('Current mode is: ' + n_str.decode() + '\n') # print it to the screen        
    
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()      
    else:
        print('Invalid Selection ' + selection_endline)



# kp, ki
# 0.12, 0.1, 60
# 0.1, 0.1, 67.36
# 0.1, 0.12, 61.98
# 0.11, 0.1, 63.96
# 0.12, 0.12, 63.08
# 0.11, 0.1, 63.37
# 0.12, 0.095, 70.39
# 0.115, 0.1, 65.5
# 0.115, 0.15, 65.9
# 0.115, 0.12, 62.88
# 0.115, 0.095, 64.625
# 0.115, 0.115, 63.69