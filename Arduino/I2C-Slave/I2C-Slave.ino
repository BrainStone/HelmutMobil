#include <WSWire.h>

// Uncomment this for debugging
// #define Debug

const byte ADDRESS = 0x4E;
const byte FLAG_PWM = 0x80;

byte states[] = {OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT, OUTPUT};

enum { 
  I2C_MSG_ARGS_MAX = 32,
  I2C_RESP_LEN_MAX = 32
};

int requestedCmd = 0;                   // which command was requested (if any)

byte i2cArgs[I2C_MSG_ARGS_MAX];         // array to store args received from master
int i2cArgsLen = 0;                     // how many args passed by master to given command

byte i2cResponse[I2C_RESP_LEN_MAX];     // array to store response
int i2cResponseLen = 0;                 // response length

extern const byte supportedI2Ccmd[] = { 
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2, A3, A6, A7, 3 | FLAG_PWM, 5 | FLAG_PWM, 6 | FLAG_PWM, 9 | FLAG_PWM, 10 | FLAG_PWM, 11 | FLAG_PWM
};

void setup() {
  Wire.begin(ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  delay(1);
  
  if((requestedCmd == -1) || (i2cArgsLen == -1)) {
    return;
  } else if(requestedCmd == -2) {
#ifdef Debug
    for(int i = 0; i < 5; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      
      digitalWrite(13, LOW);
      delay(100);
    }
#endif // #ifdef Debug
    
    return;
  }
  
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
  digitalWrite(13, HIGH);
#endif // #ifdef Debug
  
  int argIndex = 0;
  int TMPrequestedCmd = -1;
  int TMPi2cArgsLen = -1; 

  if (Wire.available()){
    TMPrequestedCmd = Wire.read();         // receive first byte - command assumed
    
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
  
#ifdef Debug
  digitalWrite(13, LOW);
#endif // #ifdef Debug
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(i2cResponse, i2cResponseLen);
  
  i2cResponseLen = 0;
  
  for(int i = 0; i < I2C_MSG_ARGS_MAX; ++i)
    i2cResponse[i] = 0xFF;
}
