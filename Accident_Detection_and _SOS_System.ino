
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial gsm(9,8);
SoftwareSerial uno2(6,7);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
const float ACCIDENT_THRESHOLD = 2.0;
float latitude,longitude;
float acceleration,v,dv,v_old=0;



void setup() {
  Serial.begin(115200);
  uno2.begin(9600);
  gsm.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);
  gsm.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  ss.begin(GPSBaud);
    if(!accel.begin())
   {
      Serial.println("No valid sensor found");
      while(1);
   }
}

void loop() {
sensors_event_t event; 
accel.getEvent(&event);
//delay(500);
Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print(" ");
Serial.println("m/s^2 ");
delay(200);

  acceleration = event.acceleration.x;
  dv = acceleration * 0.01;
  v = v_old + dv;
  v_old=v;
  /*
  Serial.print("Velocity x: ");
  Serial.print(v*3.6);
  Serial.println(" km/h");
  uno2.println(v*3.6);
  */
//delay(100);


if(event.acceleration.x > ACCIDENT_THRESHOLD || event.acceleration.y > ACCIDENT_THRESHOLD ){
   Serial.println("ACCIDENT DETETCED");
   delay(1000);
   GPS();
  }
}

void GPS(){
  Serial.println("GPS");
  
  while(!ss.available()){
    Serial.println("NOT AVAILABLE");
  }
  while(ss.available() > 0){
    Serial.println("AVAILABLE");
    while(!gps.encode(ss.read())){
        Serial.println("NOT ENCODED");
      }
    Serial.println("ENCODED");
    delay(200);
    if(!gps.location.isUpdated()){
      Serial.println("NOT UPDATED");
      //gsm_send_error();
      
    } 
    latitude=gps.location.lat();
    longitude=gps.location.lng();
    Serial.print("Latitude= "); 
    Serial.print(latitude,6);
    Serial.print(" Longitude= "); 
    Serial.println(longitude,6);
    gsm_send();
    Serial.println("SEND FUNCTION CALLED");
    break;
  }
}

void gsm_send_error(){
  Serial.println("NEW FUNCTION CALLED");
  gsm.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  gsm.println("AT+CMGS=\"+917306845112\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  gsm.println("GPS NOT AVAILABLE");
  updateSerial();
  gsm.write(26);
  delay(500);
}

void gsm_send(){
  Serial.println("FUNCTION START");
  gsm.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  gsm.println("AT+CMGS=\"+917306845112\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  gsm.println("Accident of SKEVA occcured at location:");
  updateSerial();
  gsm.print(latitude,6);
  gsm.print(", ");
  gsm.print(longitude,6);
  updateSerial(); 
  gsm.write(26);
  delay(500);
  Serial.println("FUCNTION END");
}
void updateSerial()
{

  while (Serial.available()) 
  {
    gsm.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(gsm.available()) 
  {
    Serial.write(gsm.read());//Forward what Software Serial received to Serial Port
  }
}