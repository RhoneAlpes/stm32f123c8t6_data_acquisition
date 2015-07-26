#! /usr/bin/env python2.7

# python 2.7 because not utf8 encoding
#
# Jean-Pierre Redonnet - July 25, 2015
# inphilly@gmail.com
# Licence: GNU GPL2
#

import serial
import matplotlib.pyplot as plt
import pygtk
pygtk.require('2.0')
import gtk,gobject
import math

channel1=[]
channel2=[]
time=[]

def plotGraph(w):
    plt.plot(time,channel1, 'r--', time,channel2, 'b--')
    plt.xlabel('time microseconds')
    plt.ylabel('value mV')
    plt.show()
    return

def analyze(w):
    storetime=[]
    end=len(channel1)
    i=0
    notend=1;
    while notend:
        if channel1[i] <= channel1[i+1]:
            while channel1[i] <= channel1[i+1]:
                if i < end-2: i=i+1
                else:
                    notend=0;
                    break
            storetime.append(time[i])

        if notend:
            while channel1[i] >= channel1[i+1]:
                if i < end-2: i=i+1
                else:
                    notend=0
                    break

    average=0
    i=0
    while i < len(storetime)-1:
        try:
            average= average + storetime[i+1]-storetime[i]
            i=i+1
        except:
            break
    average=average/i

    std=0
    i=0
    while i < len(storetime)-1:
        try:
            std=std + pow(average - (storetime[i+1]-storetime[i]),2)
            i=i+1
        except:
            break
    std=math.sqrt(std/(len(storetime)-1))
    
    freq=1000000/average
    
    title='Analyze'
    message="Period=%.1f micros\n" % average
    message=message+"Std=%.1f micros\n" % std
    message=message+"Freq=%.1f Hz\n" % freq
    
    dia = gtk.Dialog(title,
        None,  #the toplevel wgt of your app
        gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,  #binary flags or'ed together
        (gtk.STOCK_OK, gtk.RESPONSE_CLOSE))
    dia.vbox.pack_start(gtk.Label('\n'+message+'\n'))
    dia.show_all()
    result = dia.run()
    dia.destroy()
    return

def StartADC(w):
    del(channel1[:])
    del(channel2[:])
    del(time[:])
    
    device=chksumentry.get_text().strip()
    try:
        ser=serial.Serial(device)
    except:
        label.set_text('Check serial port!')
        return
    
    print 'Start ADC'
    ser.write('startADC\r\n')
    print 'Wait for data'
    while True:
        #print ser.readline()
        if "Duration" in ser.readline():
            break
    duration=ser.readline()
    sampling=int(duration)/1000.0
    label.set_text('Sampling time='+str(sampling)+' microseconds')
    
    a=ser.readline().split('-')

    t=0
    for suba in a:
        try:
            c,d = suba.split('/')
        except:
            print 'Done'

        channel1.append(int(c))
        channel2.append(int(d))
        time.append(t)
        t=t+sampling
    return
        
#the main windowframe2.add(label)
w = gtk.Window()
w.set_title("ADC")
w.set_default_size(280, 300)
vbox = gtk.VBox(False, 0)
w.add(vbox)

frame1 = gtk.Frame('device')
chksumentry=gtk.Entry()
chksumentry.set_text('/dev/ttyUSB0')
frame1.add(chksumentry)
vbox.pack_start(frame1, False, False, 0)

label = gtk.Label()
frame2 = gtk.Frame('Status/Result')
label.set_line_wrap(True)
label.set_text('\n\n\n\n')
frame2.add(label)
vbox.pack_start(frame2, False, False, 0)

frame3 = gtk.Frame('Start ADC')
button1 = gtk.Button("_Start",gtk.STOCK_OK)
frame3.add(button1)
vbox.pack_start(frame3, False, False, 0)

frame4 = gtk.Frame('Plot Graph')
button2 = gtk.Button("_Start",gtk.STOCK_OK)
frame4.add(button2)
vbox.pack_start(frame4, False, False, 0)

frame5 = gtk.Frame('Analyze data')
button3 = gtk.Button("_Start",gtk.STOCK_OK)
frame5.add(button3)
vbox.pack_start(frame5, False, False, 0)


button1.connect("clicked",StartADC);
button2.connect("clicked",plotGraph);
button3.connect("clicked",analyze);

w.show_all()

gtk.main()

