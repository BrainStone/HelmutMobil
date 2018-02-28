#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

const size_t boolean_size = sizeof(boolean);
const size_t int_size     = sizeof(int);
const size_t double_size   = sizeof(double);
const size_t int16_t_size = sizeof(int16_t);

const byte POTI1     = 0x00;
const byte POTI2     = 0x01;
const byte POTI3     = 0x02;
const byte ONB_TEMP  = 0x03;
const byte GYRO_AcX  = 0x04;
const byte GYRO_AcY  = 0x05;
const byte GYRO_AcZ  = 0x06;
const byte GYRO_TEMP = 0x07;
const byte GYRO_GyX  = 0x08;
const byte GYRO_GyY  = 0x09;
const byte GYRO_GyZ  = 0x0A;
const byte DROSSEL   = 0x0B;
const byte POWEROFF  = 0x0C;

const byte START_ECHO_MODE = 0x0E;
const byte END_ECHO_MODE   = 0x00;
boolean echoMode = false;

const int ONE_WIRE_BUS = 12;
const int pinCom = 10;
const int pinRelais0 = 2;
const int pinEchoMode = 13;
const int pinFirstSensor = A6;
const int pinSecondSensor = A3;
const int pinThirdSensor = A2;
const int pinDrossel = 11;
const int pinPowerOff = A0;
const int MPU = 0x68;                        //MPU 6050 i2C adress
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;   //gyro variabs
double temperatureONB;
double temperatureGYRO;

int firstSensor  = 0;   // Steer_Pot
int secondSensor = 0;   // Brake_Pot
int thirdSensor  = 0;   // DNC at this moment (probably voltage divided Battery power)
boolean drossel  = false;

int inByte = 0;

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);  //initialize all the stuff softly

char* tmp; // Needed for fast response

void setup(){
  /*Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);     
  Wire.endTransmission(true);*/
  sensors.begin();
  Serial.begin(115200);                 //start actual transfer
  
  initRelais();
  
  tmp = new char[2];
  
  pinMode(pinEchoMode, OUTPUT);
  pinMode(pinFirstSensor, INPUT);
  pinMode(pinSecondSensor, INPUT);
  pinMode(pinThirdSensor, INPUT);
  pinMode(pinDrossel, INPUT);
  pinMode(pinPowerOff, OUTPUT);
}

void loop() {

  while (Serial.available() > 0) {       //wait for serial load
    inByte = Serial.read();

    if(echoMode) {
      tmp[0] = inByte;
      tmp[1] = inByte ^ 0xFF;
      Serial.write(tmp, 2);
      
      if(inByte == END_ECHO_MODE) {
        echoMode = false;
        
        digitalWrite(pinEchoMode, LOW);
      }
    } else {
      switch (inByte) {
      case START_ECHO_MODE:
        echoMode = true;
        
        digitalWrite(pinEchoMode, HIGH);
        break;
      case POTI1:    
        Serial.write((char*)&firstSensor, int_size);
        break;
      case POTI2:
        Serial.write((char*)&secondSensor, int_size);    
        break;
      case POTI3:
        Serial.write((char*)&thirdSensor, int_size);    
        break;
      case ONB_TEMP:
        TempONB();
        Serial.write((char*)&temperatureONB, double_size);
        break;
      case GYRO_AcX:
        //Gyro_Update(); 
        Serial.write((char*)&AcX, int16_t_size);
        break;
      case GYRO_AcY: 
        //Gyro_Update();   
        Serial.write((char*)&AcY, int16_t_size);
        break;
      case GYRO_AcZ:
        //Gyro_Update();    
        Serial.write((char*)&AcZ, int16_t_size);
        break;
      case GYRO_TEMP:
        //Gyro_Update();
        Serial.write((char*)&temperatureGYRO, double_size);
        break;
      case GYRO_GyX:
        //Gyro_Update();    
        Serial.write((char*)&GyX, int16_t_size);
        break;
      case GYRO_GyY:
        //Gyro_Update();    
        Serial.write((char*)&GyY, int16_t_size);
        break;
      case GYRO_GyZ:
        //Gyro_Update();    
        Serial.write((char*)&GyZ, int16_t_size);
        break;
      case DROSSEL:
        Serial.write((char*)&drossel, boolean_size);
      case POWEROFF:
        delay(20000);
        digitalWrite(pinRelais0 + 7, LOW);
        delay(60000);
        
      default:
        if ((inByte & 0x0F) == 0x0F) {
          digitalWrite((inByte >> 5) + pinRelais0, (inByte >> 4) & 0x01);
          
          Serial.write((inByte >> 4) | 0xF0);
        } else {
          // Send simple response
          
          tmp[0] = inByte;
          tmp[1] = inByte ^ 0xFF;
          Serial.write(tmp, 2);
        }
      }
    }
  } 
  
  if(!echoMode) {
    firstSensor  = analogRead(pinFirstSensor);
    secondSensor = analogRead(pinSecondSensor);
    thirdSensor  = analogRead(pinThirdSensor);
    drossel = digitalRead(pinDrossel);
  }
}

void initRelais() {
  const int pinRelais7 = pinRelais0 + 7;
  
  for(int i = pinRelais0; i <= pinRelais7; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  
  pinMode(pinCom, OUTPUT);
  digitalWrite(pinCom, HIGH);
}

void Gyro_Update() {                 //get i2C values from Gyro
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);
  
  AcX = (Wire.read() << 8) | Wire.read();  
  AcY = (Wire.read() << 8) | Wire.read();  
  AcZ = (Wire.read() << 8) | Wire.read();  
  Tmp = (Wire.read() << 8) | Wire.read();
  GyX = (Wire.read() << 8) | Wire.read();  
  GyY = (Wire.read() << 8) | Wire.read();  
  GyZ = (Wire.read() << 8) | Wire.read();
 
 temperatureGYRO = (Tmp / 340.00) + 36.53; 
}

void TempONB() {
  sensors.requestTemperatures();
  temperatureONB = sensors.getTempCByIndex(0);
}
