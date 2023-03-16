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

def read_score():
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
    return score

def sendgain(kp, ki):
    str = 'g'
    # send the command to the PIC32
    ser.write(str.encode()); # .encode() turns the string into a char array
    ser.write(kp.encode()); # send the number
    ser.write((ki + '\n').encode()); # send the number


def startk():
    str = 'k'
    # send the command to the PIC32
    ser.write(str.encode()); # .encode() turns the string into a char array
    t = read_score()
    return t

kp_mat = []
ki_mat = []
score_mat = []
kp0 = 0.1
ki0 = 0.1
for i in range(5):
    for j in range(5):
        kptemp = str(kp0 + i*0.01);
        kitemp = str(ki0+j*0.01)
        sendgain(kptemp,kitemp)
        ti = startk()
        score_mat.append(ti)
        kp_mat.append(kptemp)
        ki_mat.append(kitemp)

min_val = min(score_mat)
print(score_mat)
print(min(score_mat))
print(kptemp.index(min_val))
print(kitemp.index(min_val))


        
        
