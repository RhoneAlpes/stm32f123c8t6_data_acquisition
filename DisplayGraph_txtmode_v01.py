#! /usr/bin/env python2.7

# for python 2.7
#
# Jean-Pierre Redonnet - July 25, 2015
# inphilly@gmail.com
# Licence: GNU GPL2
#

import serial
import matplotlib.pyplot as plt

channel1=[]
channel2=[]
time=[]

def StartADC():
    try:
        ser=serial.Serial('/dev/ttyUSB0',9600)
    except:
        print 'Check serial port'
        return
        
    print 'Start ADC'
    ser.write('startADC\r\n')
    print 'Wait for data'
    while True:
        #print ser.readline()
        if "Duration" in ser.readline():
            break
    duration=ser.readline()
    sampling=int(duration)/1000
    print 'Sampling time=',sampling,'microseconds'

    a=ser.readline().split('-')

    t=0
    for suba in a:
        try:
            c,d = suba.split('/')
        except:
            print('Done')

        channel1.append(int(c))
        channel2.append(int(d))
        time.append(t)
        t=t+sampling

    plt.plot(time,channel1, 'r--', time,channel2, 'bs')
    plt.xlabel('time microseconds')
    plt.ylabel('value mV')
    plt.show()

StartADC()
