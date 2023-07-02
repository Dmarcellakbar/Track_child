#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

static const int RXPin = 4, TXPin = 5;   // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS
static const uint32_t GPSBaud = 9600; //if Baud rate 9600 didn't work in your case then use 4800

TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget

SoftwareSerial ss(RXPin, TXPin);  // The serial connection to the GPS device

BlynkTimer timer;
WidgetRTC rtc;

String currentTime;
String currentDate;

double spd;       //Variable  to store the speed
double sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GPS

char auth[] = "JVoBDqTnvPMPHaYRdX57MVwE2Wr4-K8s";              //Your Project authentication key
char ssid[] = "o'mallays";                                       // Name of your network (HotSpot or Router name)
char pass[] = "0987654321";                                      // Corresponding Password

//unsigned int move_index;         // moving index, to be used later
unsigned int move_index = 1;       // fixed location for now
  

void setup()
{
  Serial.begin(115200);
  pinMode(D8,INPUT);
  pinMode(D7,INPUT);
  pinMode(D5, INPUT);
  Serial.println();
  ss.begin(GPSBaud);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, checkGPS); // every 5s check if GPS is connected, only really needs to be done once

  // Begin synchronizing time
  rtc.begin();
  
  // Display digital clock every 10 seconds
  timer.setInterval(1000L, clockDisplay);
}

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
      Blynk.virtualWrite(V4, "GPS ERROR");  // Value Display widget  on V4 if GPS not detected
  }
}

void loop()
{
    while (ss.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(ss.read()))
        displayInfo();
  }
  if(digitalRead(D8) == HIGH)
  {
  Serial.println("Send Notification to Blynk");
  Blynk.notify("Dalam Bahaya"); // This notification will be sent to Blynk App
  }
  if(digitalRead(D7) == HIGH)
  {
  Serial.println("Send Notification to Blynk");
  Blynk.notify("Waktunya Pulang"); // This notification will be sent to Blynk App
  }
  Blynk.run();
  timer.run();
  switchstate();
}

void displayInfo()
{ 
  if (gps.location.isValid() ) 
  {    
    double latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
    double longitude = (gps.location.lng()); 
    
    Serial.print("LAT:  ");
    Serial.println(latitude, 6);  // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude, 6);
    Blynk.virtualWrite(V1, String(latitude, 6));   
    Blynk.virtualWrite(V2, String(longitude, 6));  
    myMap.location(move_index, latitude, longitude, "GPS_Location");
    spd = gps.speed.kmph();               //get speed
       Blynk.virtualWrite(V3, spd);
       
       sats = gps.satellites.value();    //get number of satellites
       Blynk.virtualWrite(V4, sats);

       bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
       Blynk.virtualWrite(V5, bearing);                   
  }
  
 Serial.println();
}

void clockDisplay()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details
 
  currentTime = String(hour()) + ":" + minute() + ":" + second();
  currentDate = String(day()) + " " + month() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print("");
  Serial.print(currentDate);
  Serial.println();
 
  // Send time to the App
  Blynk.virtualWrite(V6, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V7, currentDate);
}

void switchstate(){

    if (digitalRead(D5)==HIGH)
          {
              Blynk.virtualWrite(V8, "TrackChild dilepas.");
          }
          else
         {
               Blynk.virtualWrite(V8, "TrackChild dipasang");
         }         
}
