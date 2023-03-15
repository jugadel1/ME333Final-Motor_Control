# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import serial
ser = serial.Serial('COM3',230400)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\ta: get motor current \tc: get encoder counts \td: get out shaft degrees \te: reset encoder counts \tr: get current mode \tz: dummy case \tq: Quit') # '\t' is a tab
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
        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('Current out shaft angle: ' + str(n_int) + '\n') # print it to the screen        
        
    elif (selection == 'e'):
        print('Resetting encoder count.')

    elif (selection == 'r'):
        print('Requesting current mode...')
        n_str = ser.read_until(b'\n');  # get the incremented number back
        print('Current mode is: ' + n_str + '\n') # print it to the screen        
    
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()      
    else:
        print('Invalid Selection ' + selection_endline)



