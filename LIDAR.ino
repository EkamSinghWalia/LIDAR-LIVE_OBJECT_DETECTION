
#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>

SoftwareSerial sim808(2,3);
Servo mServo;
VL53L0X sensor;

char phone_no[] = "+919873378570"; // replace with your phone no.
String data[5];
#define DEBUG true
String state,timegps,latitude="0.00",longitude="0.00";
long int tUpdate=millis()/1000;

int distThresh=30;

long int dist;


void SEND(const char *ptr)
{
  unsigned char i;
  i=0;
  while(ptr[i]!='\0')
    {
      sim808.write(ptr[i]);
      i++;
      delay(50);
    }
}

int angle=10;
int DIR=1;

void setup() {

pinMode(A0,INPUT);
pinMode(A1,OUTPUT);  
pinMode(A2,OUTPUT);
pinMode(A3,OUTPUT);

pinMode(4,OUTPUT);
pinMode(5,OUTPUT);
pinMode(6,OUTPUT);
pinMode(7,OUTPUT);
pinMode(8,OUTPUT);
pinMode(10,OUTPUT);
pinMode(11,OUTPUT);
pinMode(12,OUTPUT);
pinMode(13,OUTPUT);

digitalWrite(4,HIGH);
digitalWrite(5,HIGH);
digitalWrite(6,HIGH);
digitalWrite(7,HIGH);
digitalWrite(8,HIGH);
digitalWrite(10,HIGH);
digitalWrite(11,HIGH);
digitalWrite(12,HIGH);
digitalWrite(13,HIGH);

digitalWrite(A1,HIGH);
digitalWrite(A2,HIGH);
digitalWrite(A3,HIGH);

sim808.begin(9600);
Serial.begin(9600);
mServo.attach(9);

delay(50);

  digitalWrite(A3,LOW);
  SEND("AT+CMGF=1"); sim808.write(0x0d);
  delay(1000);
  SEND("ATE0"); sim808.write(0x0d);
  delay(1000);
  SEND("AT+CSMP=17,167,0,0"); sim808.write('\n'); sim808.write(0x0d);
  delay(1000);
  sendData("AT+CGNSPWR=1",1000,DEBUG);
  delay(50);
  sendData("AT+CGNSSEQ=RMC",1000,DEBUG);
  delay(150);

  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init())
    Serial.println("Sensor isnt working       ");
  
  sensor.startContinuous();
  digitalWrite(A3,HIGH);
}


void msg()
{
digitalWrite(A3,LOW);
    SEND("AT+CMGS=\"+918527341274\"");
    sim808.write(0x0d);
    delay(1000);
  
                SEND("EMERGENCY!!!!\nPlease Help!!\n");
                
                SEND("\nhttp://maps.google.com/maps?q=loc:");
                sim808.print(latitude);
                sim808.print(",");
                sim808.print (longitude);
                delay(200);
                sim808.write(0x0d);
                sim808.write(0x1a);
                delay(200);
                delay(5000);
                //while( sim808.available()>0)
               //   { Serial.write((char)sim808.read()); delay(5); }
                sim808.flush();

digitalWrite(A3,HIGH);
}

char inChar='C';
int flag=0;
long int TIME=millis()/1000;
int AUTO=1;
boolean metalData=false;
boolean lidarDetect=false;
boolean traced=false;
boolean currentDet=false;
int lastAngle=10;
int objectNo=1;
int finalTrace=0;

void sendMSG()
{
       digitalWrite(4,HIGH); digitalWrite(5,HIGH); digitalWrite(6,HIGH); digitalWrite(7,HIGH); 
     digitalWrite(10,HIGH); digitalWrite(11,HIGH); digitalWrite(12,HIGH); digitalWrite(13,HIGH);
     inChar='C';
     
    digitalWrite(A2,LOW);
    digitalWrite(A1,LOW);
    sendTabData("AT+CGNSINF",1000,DEBUG);
    /*if (state !=0) {
      Serial.println("State  :"+state);
      Serial.println("Time  :"+timegps);
      Serial.println("Latitude  :"+latitude);
      Serial.println("Longitude  :"+longitude);
      if(latitude=="0.00")
          {
            latitude="28.7654";
            longitude="77.5464";
          }
    } else {
    Serial.println("GPS Initializing…");*/
    latitude="28.76"+(millis()%100);
    longitude="77.54"+(millis()%100);
    //}
    digitalWrite(A1,HIGH);
    msg();
    delay(5000);
    digitalWrite(A2,HIGH);
}

void loop() {

  dist=sensor.readRangeContinuousMillimeters()/10;
  if (sensor.timeoutOccurred()) { Serial.println("TIMEOUT                   "); }

  if(dist<distThresh && lidarDetect==false && abs(lastAngle-angle)>5 )
    {
      if(objectNo==1)
        Serial.println("CLEAR                     ");

      currentDet=true;
      lastAngle=angle;
      lidarDetect=true;
      traced=true;
      finalTrace=1;
      Serial.print("Object");
      Serial.print(objectNo);
      Serial.print(" start ");
      Serial.print(dist/10);
      Serial.print(dist%10);
      Serial.print("cm(");
      Serial.print(angle/100);
      Serial.print((angle/10)%10);
      Serial.print(angle%10);
      Serial.println("deg)");
    }
    else if(dist<distThresh && AUTO==0 &&  (millis()/1000 -tUpdate)>0 )
      {
        tUpdate=millis()/1000;
        currentDet=false;
      Serial.println("CLEAR                     ");
      delay(500);
      Serial.print("Object");
      Serial.print(objectNo);
      Serial.print(" start ");
      Serial.print(dist/10);
      Serial.print(dist%10);
      Serial.print("cm(");
      Serial.print(angle/100);
      Serial.print((angle/10)%10);
      Serial.print(angle%10);
      Serial.println("deg)");
      }
    else if(dist>50)
      {
        lidarDetect=false;
        if(currentDet==true)
        {
              Serial.print("Object");
              Serial.print(objectNo);
              Serial.print(" end ");
              Serial.print(angle/100);
              Serial.print((angle/10)%10);
              Serial.print(angle%10);
              Serial.println("deg)       ");
              objectNo++;
              currentDet=false;
      } 
     }
  
if(Serial.available()>0)
    inChar=(char)Serial.read();

if(analogRead(A0)>=700 && flag==0 && (millis()/1000)-TIME > 0)
    {
      digitalWrite(A2,LOW);
     digitalWrite(4,HIGH); digitalWrite(5,HIGH); digitalWrite(6,HIGH); digitalWrite(7,HIGH); 
     digitalWrite(10,HIGH); digitalWrite(11,HIGH); digitalWrite(12,HIGH); digitalWrite(13,HIGH);
      if(metalData==false)
        {
          metalData=true;
          Serial.println("Metal Detected            ");
        }
      delay(1500);
      inChar='C';
      flag=1;
    }
else if(analogRead(A0)>=700 && flag==1)
      {
        digitalWrite(A2,!digitalRead(A2));
        delay(300);
      }
else if(analogRead(A0)<=400 && flag==1)
    {
      flag=0;
      digitalWrite(A2,HIGH);
      TIME=millis()/1000;
      metalData=false;
    }

if(inChar=='U')
   { digitalWrite(4,LOW); digitalWrite(5,HIGH); digitalWrite(6,LOW); digitalWrite(7,HIGH); 
     digitalWrite(10,LOW); digitalWrite(11,HIGH); digitalWrite(12,LOW); digitalWrite(13,HIGH);
   }
else if(inChar=='D')
   { digitalWrite(4,HIGH); digitalWrite(5,LOW); digitalWrite(6,HIGH); digitalWrite(7,LOW); 
     digitalWrite(10,HIGH); digitalWrite(11,LOW); digitalWrite(12,HIGH); digitalWrite(13,LOW);
   }
else if(inChar=='L')
   { digitalWrite(4,HIGH); digitalWrite(5,LOW); digitalWrite(6,LOW); digitalWrite(7,HIGH); 
     digitalWrite(10,HIGH); digitalWrite(11,LOW); digitalWrite(12,LOW); digitalWrite(13,HIGH);
   }
else if(inChar=='R')
   { digitalWrite(4,LOW); digitalWrite(5,HIGH); digitalWrite(6,HIGH); digitalWrite(7,LOW); 
     digitalWrite(10,LOW); digitalWrite(11,HIGH); digitalWrite(12,HIGH); digitalWrite(13,LOW);
   }
else if(inChar=='C')
   { digitalWrite(4,HIGH); digitalWrite(5,HIGH); digitalWrite(6,HIGH); digitalWrite(7,HIGH); 
     digitalWrite(10,HIGH); digitalWrite(11,HIGH); digitalWrite(12,HIGH); digitalWrite(13,HIGH);
   }
else if(inChar=='b')
   { digitalWrite(4,LOW); digitalWrite(5,HIGH); digitalWrite(6,HIGH); digitalWrite(7,LOW); 
     digitalWrite(10,HIGH); digitalWrite(11,LOW); digitalWrite(12,LOW); digitalWrite(13,HIGH);
   }
else if(inChar=='d')
   { digitalWrite(4,HIGH); digitalWrite(5,LOW); digitalWrite(6,LOW); digitalWrite(7,HIGH); 
     digitalWrite(10,LOW); digitalWrite(11,HIGH); digitalWrite(12,HIGH); digitalWrite(13,LOW);
   }
else if(inChar=='a')
   AUTO=1;
else if(inChar=='c')
   AUTO=0;    
else if(inChar=='e')
   sendMSG();

mServo.write(angle);
if(AUTO==1)
  {
        angle=angle+1;
        if(angle>170)
          {
            if(traced==false && objectNo==1 && finalTrace==0)
              {
                Serial.println("No Intrusion              ");
              }
            angle=10; 
            finalTrace=0;
            traced=false;
            currentDet=false;
            objectNo=1;     
          }
  }
  
delay(1);
}
void sendTabData(String command , const int timeout , boolean debug){

sim808.println(command);
long int time = millis();
int i = 0;

while((time+timeout) > millis()){
while(sim808.available()){
char c = sim808.read();
if (c != ',') {
data[i] +=c;
delay(100);
} else {
i++;
}
if (i == 5) {
delay(100);
goto exitL;
}
}
}exitL:
if (debug) {
state = data[1];
timegps = data[2];
latitude = data[3];
longitude =data[4];
}
}
String sendData (String command , const int timeout ,boolean debug){
String response = "";
sim808.println(command);
long int time = millis();
int i = 0;

while ( (time+timeout ) > millis()){
while (sim808.available()){
char c = sim808.read();
response +=c;
}
