// #include <WSWire.h>
#include <Wire.h>

// Uncomment this for debugging
// #define Debug

const byte ADDRESS = 0x4E;
const byte FLAG_PWM = 0x80;

byte states[] = {OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT};

enum { 
  I2C_MSG_ARGS_MAX = 32,
  I2C_RESP_LEN_MAX = 32
};

int requestedCmd = -1;                   // which command was requested (if any)

byte i2cArgs[I2C_MSG_ARGS_MAX];         // array to store args received from master
int i2cArgsLen = 0;                     // how many args passed by master to given command

byte i2cResponse[I2C_RESP_LEN_MAX];     // array to store response
int i2cResponseLen = 0;                 // response length

extern const byte supportedI2Ccmd[] = { 
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2, A3, A6, A7, 3 | FLAG_PWM, 5 | FLAG_PWM, 6 | FLAG_PWM, 9 | FLAG_PWM, 10 | FLAG_PWM, 11 | FLAG_PWM
};

void setup() {
#ifdef Debug
  Serial.begin(9600);

  Serial.println("Starting!");
#endif // #ifdef Debug
  
  Wire.begin(ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  delay(1);

  if((requestedCmd == -1) || (i2cArgsLen == -1)) {
    return;
  } else if(requestedCmd == -2) {
    return;
  }

#ifdef Debug
  Serial.print("loop:\n\trequestedCmd: ");
  Serial.println(requestedCmd);
  Serial.print("\ti2cArgsLen: ");
  Serial.println(i2cArgsLen);
#endif // #ifdef Debug
  
  boolean pwm = requestedCmd & FLAG_PWM;
  requestedCmd &= ~FLAG_PWM;
  
  if (i2cArgsLen == 0) {
    if (states[requestedCmd] != INPUT)
      pinMode(requestedCmd, INPUT);
    
    if (requestedCmd < A0) {
      i2cResponse[0] = digitalRead(requestedCmd);
      i2cResponseLen = 1;
    } else {
      *(int*)(i2cResponse) = analogRead(requestedCmd);
      i2cResponseLen = 2;
    }

#ifdef Debug
    Serial.print("\ti2cResponse: ");
    Serial.println(*(int*)(i2cResponse));
    Serial.print("\ti2cResponseLen: ");
    Serial.println(i2cResponseLen);
#endif // #ifdef Debug
  } else if (i2cArgsLen == 1) {
    if (states[requestedCmd] != OUTPUT)
        pinMode(requestedCmd, OUTPUT);
    
    if (pwm)
      analogWrite(requestedCmd, i2cArgs[0]);
    else
      digitalWrite(requestedCmd, i2cArgs[0]);
  }
  
  requestedCmd = -1;
  i2cArgsLen = -1;
}

// function that executes when master sends data (begin-end transmission)
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
#ifdef Debug
  Serial.print("receiveEvent: ");
  Serial.println(howMany);
#endif // #ifdef Debug
  
  int argIndex = 0;
  int TMPrequestedCmd = -1;
  int TMPi2cArgsLen = -1; 

  if (Wire.available()){
    TMPrequestedCmd = Wire.read();         // receive first byte - command assumed

#ifdef Debug
    Serial.print("Cmd: ");
    Serial.println(TMPrequestedCmd);
#endif // #ifdef Debug
    
    while(Wire.available()) {              // receive rest of tramsmission from master assuming arguments to the command
      if (argIndex < I2C_MSG_ARGS_MAX) {
        i2cArgs[argIndex++] = Wire.read();
      } else {
        argIndex = -2;
        
        break;
      }
    }
    
    TMPi2cArgsLen = argIndex;  
  } else {
    requestedCmd = -2;
    i2cArgsLen = -1;
    
    return;
  }
  
  // validating command is supported by slave
  boolean commandFound = false;
  
  for (int i = 0; i < sizeof(supportedI2Ccmd); ++i) {
    if (supportedI2Ccmd[i] == TMPrequestedCmd) {
      commandFound = true;
      
      break;
    }
  }

  if (!commandFound) {
    requestedCmd = -2;
    i2cArgsLen = -1;
    
    return;
  }
  
  // now main loop code should pick up a command to execute and prepare required response when master waits before requesting response
  requestedCmd = TMPrequestedCmd;
  i2cArgsLen = TMPi2cArgsLen;
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  if(i2cResponseLen < 1) {
    return;
  }
  
  Wire.write(i2cResponse, i2cResponseLen);

#ifdef Debug
  Serial.print("requestEvent:\n\ti2cResponse: ");
  Serial.println(*(int*)(i2cResponse));
  Serial.print("\ti2cResponseLen: ");
  Serial.println(i2cResponseLen);
#endif // #ifdef Debug
  
  i2cResponseLen = 0;
  
  for(int i = 0; i < I2C_MSG_ARGS_MAX; ++i)
    i2cResponse[i] = 0xFF;
}
