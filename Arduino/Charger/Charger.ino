#include <LcdBarGraph.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 8, 3, 7, 4, 6, 5);
LcdBarGraph lbg(&lcd, 4,4,1);

const int chargeEnd = 552;
const int chargeBegin = 440;
const int chargeEndMin = chargeEnd - 2;

byte leer[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};

byte laden[8] = {
  B00001,
  B00010,
  B00100,
  B00010,
  B10100,
  B11000,
  B11100,
};

byte check[8] = {
  B00001,
  B00001,
  B00010,
  B00010,
  B00100,
  B10100,
  B01000,
};

const int pinUp      = 12;
const int pinDown    = 11;
const int pinRelais  = A2;
const int pinStatus  = 13;
const int pinVoltage = A0;

boolean charge = true;
int Voltage = 0;
int Percentage = 0;
int displayState = 0;

unsigned long chargeTime = -1;
unsigned long lastTime;

void printVoltage() {
  int voltageTmp = Voltage;
  
  short digit[3];
  
  for(int i = 0; i < 3; i++) {
    digit[i] = voltageTmp % 10;
    voltageTmp /= 10;
  }
  
  lcd.setCursor(0, 0);
  lcd.write(byte((charge)? ((chargeTime / 500) % 2) : 2));
  lcd.print(digit[2]);
  lcd.print(digit[1]);
  lcd.print(',');
  lcd.print(digit[0]);
  lcd.print(" V ");
}

void printPercentage() {
  lcd.setCursor(0, 1);
  
  if (Percentage < 10)
    lcd.print("  ");
  else if (Percentage < 100)
    lcd.print(' ');
  
  lcd.print(Percentage);
  lcd.print('%');
  
  lbg.drawValue(Percentage, 100);
}

void printTime() {
  lcd.setCursor(0, 0);
  
  if (chargeTime == -1) {
    lcd.print("Charged!");
  } else {  
    unsigned short hours, minutes;
    unsigned long tmpChargeTime = chargeTime / 1000;
    const int divisor[] = {10, 10, -1, 6, 10, -1, 6, 10};
    char seperator = ((chargeTime / 500) % 2) ? ' ' : ':';
    
    for (int i = 7; i >= 0; i--) {
      lcd.setCursor(i, 0);
      
      if (divisor[i] == -1) {
        lcd.print(seperator);
      } else {
        lcd.print(tmpChargeTime % divisor[i]);
        tmpChargeTime /= divisor[i];
      }
    }
  }
}

void clearLine(int line) {
  lcd.setCursor(0, line);
  
  lcd.print("        ");
}

void setRelais(bool loading) {
  digitalWrite(pinRelais, !loading);
  digitalWrite(pinStatus, loading);
}

void setup() {
  lcd.createChar(0, leer);
  lcd.createChar(1, laden);
  lcd.createChar(2, check);
  
  lcd.begin(8, 2);
  pinMode(pinRelais, OUTPUT);
  pinMode(pinStatus, OUTPUT);
  setRelais(true);
  
  for(int i = 0; i < 10; i++) {
    analogRead(pinVoltage);
    delay(10);
  }
  
  setRelais(false);
  delay(100);
  setRelais(true);
}

void loop() {
  Voltage = map(analogRead(pinVoltage), 0, 1023, 0, 655);
  Percentage = constrain(map(Voltage, chargeBegin, chargeEnd, 0, 100), 0, 100);
  
  if ((displayState < 1) && !digitalRead(pinDown)) {
    displayState++;
    
    clearLine(0);
  } else if ((displayState > 0) && !digitalRead(pinUp)) {
    displayState--;
    
    clearLine(0);
  }
  
  if (displayState == 0) {
    printVoltage();
    printPercentage();
  } else if (displayState == 1) {
    printTime();
    printPercentage();
  }

  if (!charge && (Voltage <= chargeEndMin)) {
    charge = true;
    
    chargeTime = 0;
  } else if (charge && (Voltage >= chargeEnd)) {
    charge = false;
  }
    
  if (charge) {
    setRelais(true);
    
    chargeTime += millis() - lastTime;
  } else {
    setRelais(false);
  }
  
  lastTime = millis();
  
  /*lcd.setCursor(4,1);
  lcd.print(chargeTime);*/
  
  //delay(100);
}



