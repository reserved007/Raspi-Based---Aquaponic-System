/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
   More info: http://www.ardumotive.com/how-to-use-dht-22-sensor-en.html
   Dev: Michalis Vasilakis // Date: 1/7/2015 // www.ardumotive.com */

//Libraries
#include <DHT.h>;
#include "GravityTDS.h"
// Make sure to include Onewire and Dallas Temperature libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9);
//Constants
#define DHTPIN 3     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//#define TdsSensorPin A0
//GravityTDS gravityTds;

//float temperature = 25,tdsValue = 0;
//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

float calibration;
const int analogInPin = A1;
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10],temp1;
int pHlevel;
String cdata; // complete data, consisting of sensors values


float calibration_value = 15.4;
int phval = 0; 
//unsigned long int avgval; 
int buffer_arr[10];//,temp;
 
float ph_act;

float ph(float voltage){
  return 7+((2.5-voltage))/0.18;
}

#define TdsSensorPin A0
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;

double flow; //Water flow L/Min 
int flowsensor = 2; 
unsigned long currentTime;
unsigned long lastTime;
unsigned long pulse_freq;
 
float Celsius = 0;
float Fahrenheit = 0;
void pulse () // Interrupt function

{
   pulse_freq++;
}
extern volatile unsigned long timer0_millis;
void setup()
{
    Serial.begin(9600);
    mySerial.begin(9600);   // Setting the baud rate of GSM Module  
	  dht.begin();
//    gravityTds.setPin(TdsSensorPin);
//    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
//    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
//    gravityTds.begin();  //initialization
   pinMode(TdsSensorPin,INPUT);
   sensors.begin();
   pinMode(flowsensor, INPUT);
   attachInterrupt(0, pulse, RISING); // Setup Interrupt
   currentTime = millis();
   lastTime = currentTime;
   SendMessage();
}

void loop()
{




    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();

    //Print temp and humidity values to serial monitor
//    Serial.print("Humidity: ");
//    Serial.println(hum);
//    delay(1000);

    temp= dht.readTemperature();
 //   Serial.print(" %, Temp: ");
  //  Serial.println(temp);
  //  Serial.println(" Celsius");
  //  delay(2000); //Delay 2 sec.
 //   delay(1000);



  for(int i=0;i<10;i++){
    buf[i] = analogRead(analogInPin);
    delay(30);
  }
//  for(int i=0;i<9;i++){
//for(int j =i+1;j<10;j++)
//  {
 //   if(buf[i]>buf[j])
 //   {
 //   temp=buf[i];
 //   buf[i]=buf[j];
//     buf[j]=temp1;
 //   }
//  }
//  }
  float avgval=0;
 for(int i=2;i<8;i++)
// avgval+=buffer_arr[i];
 avgval+=buf[i];
//  float volt=(float)avgval*5.0/1023; 
//  ph_act = volt + calibration_value;
 float volt=(float)avgval*5.0/1024/10; 
 ph_act = -5.70 * volt + calibration_value;
 
// Serial.println("pH Val: ");
// Serial.print(ph_act);
// delay(1000);
 
 
 
 
 
 //    Serial.print("pHlevel:");
 //   Serial.println(pHlevel);
//    delay(1000);

    
    //temperature = readTemperature();  //add your temperature sensor and read it
 //   gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
 //   gravityTds.update();  //sample and calculate
 //   tdsValue = gravityTds.getTdsValue();  // then get the value
//    Serial.println(tdsValue,0);
//    Serial.println(tdsValue);
//    Serial.println("ppm");
 //   delay(1000);
   static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum1(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      //Serial.print("TDS Value:");
     // Serial.print(tdsValue,0);
      //Serial.println("ppm");
   }



    
//  cdata = cdata+ temp +","+pHlevel+","+ tdsValue+'\n'; // comma will be used a delimeter
//  cdata = cdata+hum +","+pHlevel+","+ tdsValue+'\n'; // comma will be used a delimeter
//  Serial.println(cdata);
//  cdata = "";
 sensors.requestTemperatures();
 Celsius = sensors.getTempCByIndex(0);
  Fahrenheit = sensors.toFahrenheit(Celsius); 
 // Serial.print(Celsius);
 // Serial.print(" C  ");
 // Serial.print(Fahrenheit);
 // Serial.println(" F");
  delay(1000);

   currentTime = millis();
   // Every second, calculate and print L/Min
   if(currentTime >= (lastTime + 1000))
   {
      lastTime = currentTime; 
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      flow = (pulse_freq / 7.5); 
      pulse_freq = 0; // Reset Counter
      //Serial.print(flow, DEC); 
      //Serial.println(" L/Min");
      
   }
int Seconds = ( millis()/1000 );
int Minutes = ( millis()/1000 ) / 60;
int Hours = ( ( millis()/1000 ) / 60 ) / 60;
//Serial.print("minutes:");
//Serial.print(Minutes);
//Serial.print(",Hours:");
//Serial.println(Hours);
if(Minutes == 2){
  //timer0_millis=0;
  //SendMessage();
}
if(Hours == 12){
  timer0_millis=0;
  SendMessage();
}
    Serial.println(Celsius);
    Serial.println(ph_act);
    Serial.println(hum);
    Serial.println(tdsValue,0);
    Serial.println(flow, 2); 
    delay(1000);
}



int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}


void SendMessage()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+639175199610\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.print("Temp:");// The SMS text you want to send
    delay(100);
    mySerial.println(Celsius);
    delay(100);
  mySerial.print("pH:");// The SMS text you want to send
  mySerial.println(ph_act);
    delay(100);
    mySerial.print("Humid:");// The SMS text you want to send
    mySerial.println(hum);
     delay(100);
    mySerial.print("TDS:");// The SMS text you want to send
    mySerial.println(tdsValue,0);
    delay(100);
    mySerial.print("Flow:");// The SMS text you want to send
    mySerial.println(flow, 4);   
    delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}   



int getMedianNum1(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}
