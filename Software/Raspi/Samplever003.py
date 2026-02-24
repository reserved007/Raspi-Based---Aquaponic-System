"""
This Code is belong to SME Dehraun. for any query write to schematicslab@gmail.com

"""

import BlynkLib
import RPi.GPIO as GPIO
from BlynkTimer import BlynkTimer

#import Adafruit_DHT
import time

import serial
#import time
from RPLCD import *
from RPLCD.i2c import CharLCD
lcd = CharLCD('PCF8574', 0X27)
#import RPi.GPIO as GPIO
import smbus
outPin1=11
outPin2=13
GPIO.setwarnings(False) 
GPIO.setmode(GPIO.BOARD)
#GPIO.setmode(GPIO.BCM)
GPIO.setup(outPin1,GPIO.OUT)
GPIO.setup(outPin2,GPIO.OUT)
GPIO.output(outPin1,GPIO.LOW)
GPIO.output(outPin2,GPIO.LOW)
pumpState1=1
pumpState1=2
#Set LCD SIZE
lcd_columns = 20
lcd_rows = 4

sdata = serial.Serial('/dev/ttyUSB0',9600,timeout=1.0)
time.sleep(2)

sdata.reset_input_buffer()



#DHT_SENSOR = Adafruit_DHT.DHT11
#DHT_PIN = 4
#BLYNK_AUTH_TOKEN = 'kt85ldXRL362juNQiymMClUUxdsuygTN'
BLYNK_AUTH_TOKEN = 'au4kucEgbbGuntWr47M8U69xn6hTgX4S'

# Initialize Blynk
blynk = BlynkLib.Blynk(BLYNK_AUTH_TOKEN)

# Create BlynkTimer Instance
timer = BlynkTimer()

print("Arduino Connected")
lcd.clear()
lcd.cursor_pos = (0,0)
lcd.write_string('Temp(C):')
lcd.cursor_pos = (0,9)
lcd.write_string('0.0')
lcd.cursor_pos = (1,0)
lcd.write_string('pH Level:')
lcd.cursor_pos = (1,9)
lcd.write_string('0.0')
lcd.cursor_pos = (2,0)
lcd.write_string('Humid:')
lcd.cursor_pos = (2,9)
lcd.write_string('0.0')
lcd.cursor_pos = (3,0)
lcd.write_string('TDS(ppm):')
lcd.cursor_pos = (3,9)
lcd.write_string('0.0')
lcd.cursor_pos = (2,15)
lcd.write_string('Flow')
lcd.cursor_pos = (3,15)
lcd.write_string('0.0')
lcd.clear()
# function to sync the data from virtual pins
@blynk.on("connected")
def blynk_connected():
    print("Hi, You have Connected to New Blynk2.0")
 #   print(".......................................................")
#    print("................... By SME Dehradun ...................")
    time.sleep(2);

@blynk.on("V5")   
def v5_write_handler(value):
    print('Current slider value: {}'.format(value[0]))
    offset = 1
    global phData
    #global phStatus
    phData = float(value[0])
    if phData < 1 or phData > 14:
        phData = 7
    print("phData:",phData)
    blynk.virtual_write(6,phData)
    time.sleep(0.01)
    if sdata.in_waiting > 0:
        mydata = sdata.readline().decode('utf-8').rstrip()
        
        pHStatus = mydata
        print('Temp:',mydata)
        blynk.virtual_write(0, mydata)
        lcd.cursor_pos = (0,0)
        lcd.write_string('Temp(C):')
        lcd.cursor_pos = (0,8)
        lcd.write_string(mydata)


        mydata = sdata.readline().decode('utf-8').rstrip()

        blynk.virtual_write(1, mydata)
        lcd.cursor_pos = (1,0)
        lcd.write_string('pH Level:')
        lcd.cursor_pos = (1,9)
        lcd.write_string(mydata)
        x = int(phData) + int(1.0)
        y = int(phData) - int(1.0) 
        print('pH Level:',mydata)
        phStatus = mydata
        print('x',x)
        if float(phStatus) > x:
            print("PUMP1 On")
            GPIO.output(outPin1,GPIO.HIGH)
            GPIO.output(outPin2,GPIO.LOW)
            time.sleep(1);
            GPIO.output(outPin1,GPIO.LOW)
        elif float(phStatus) < y:
            print("PUMP2 On")
            GPIO.output(outPin1,GPIO.LOW)
            GPIO.output(outPin2,GPIO.HIGH)
            time.sleep(1);
            GPIO.output(outPin2,GPIO.LOW)
        else:
            GPIO.output(outPin1,GPIO.LOW)
            GPIO.output(outPin2,GPIO.LOW)
        mydata = sdata.readline().decode('utf-8').rstrip()
        print('Humid:',mydata)
        blynk.virtual_write(2, mydata)
        lcd.cursor_pos = (2,0)
        lcd.write_string('Humid(%):')
        lcd.cursor_pos = (2,9)
        lcd.write_string(mydata)
        mydata = sdata.readline().decode('utf-8').rstrip()
        print('TDS:',mydata)
        blynk.virtual_write(3, mydata)
        lcd.cursor_pos = (3,0)
        lcd.write_string('TDS(ppm):')
        lcd.cursor_pos = (3,9)
        lcd.write_string(mydata)
        mydata = sdata.readline().decode('utf-8').rstrip()
        print('Flowrate:',mydata)
        lcd.cursor_pos = (2,15)
        lcd.write_string('Flow:')
        lcd.cursor_pos = (3,14)
        lcd.write_string(mydata)
        blynk.virtual_write(4, mydata)


@blynk.on("V6") 
def v1_write_handler(value):
    print('Current slider value: {}'.format(value[0]))
    global phData
    global phStatus
    phData = int(value[0])
    print("phData1:",phData)
    blynk.virtual_write(5,phData)             
# Functon for collect data from sensor & send it to Server
def myData():
            time.sleep(0.01)
            if sdata.in_waiting > 0:
                mydata = sdata.readline().decode('utf-8').rstrip()
                print('Temp:',mydata)
                blynk.virtual_write(0, mydata)
                lcd.cursor_pos = (0,0)
                lcd.write_string('Temp(C):')
                lcd.cursor_pos = (0,8)
                lcd.write_string(mydata)
                mydata = sdata.readline().decode('utf-8').rstrip()
                print('pH Level:',mydata)
                blynk.virtual_write(1, mydata)
                lcd.cursor_pos = (1,0)
                lcd.write_string('pH Level:')
                lcd.cursor_pos = (1,9)
                lcd.write_string(mydata)
                mydata = sdata.readline().decode('utf-8').rstrip()
                print('Humid:',mydata)
                blynk.virtual_write(2, mydata)
                lcd.cursor_pos = (2,0)
                lcd.write_string('Humid(%):')
                lcd.cursor_pos = (2,9)
                lcd.write_string(mydata)
                mydata = sdata.readline().decode('utf-8').rstrip()
                print('TDS:',mydata)
                blynk.virtual_write(3, mydata)
                lcd.cursor_pos = (3,0)
                lcd.write_string('TDS(ppm):')
                lcd.cursor_pos = (3,9)
                lcd.write_string(mydata)
                mydata = sdata.readline().decode('utf-8').rstrip()
                print('Flowrate:',mydata)
#                lcd.cursor_pos = (2,15)
#                lcd.write_string('Flow:')
#                lcd.cursor_pos = (3,14)
#                lcd.write_string(mydata)
                blynk.virtual_write(4, mydata)



#timer.set_interval(2, myData)

try:
    while True:
        blynk.run()
        timer.run()

 #       time.sleep(3)



        
except KeyboardInterrupt:
    GPIO.cleanup()
    print('GPIO Good to Go')
