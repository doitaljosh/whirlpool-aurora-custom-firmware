#define led1_ena 1
#define led2_ena 2
#define dcOutput_ena 15
#define dcOutput1_ena 22
#define dcOutput2_ena 20
#define led1_pwm 23
#define led2_pwm 24
#define dcOutput1_pwm 7
#define dcOutput2_pwm 9

typedef struct {
  
  uint8_t pinNum;
  bool val;
  
} gpioInit_t;

gpioInit_t outputPins[] = {
  
  {led1_ena, HIGH},
  {led2_ena, HIGH},
  {dcOutput_ena, LOW},
  {dcOutput1_ena, LOW},
  {dcOutput2_ena, LOW},
  {led1_pwm, LOW},
  {led2_pwm, LOW},
  {dcOutput1_pwm, LOW},
  {dcOutput2_pwm, LOW}
  
};

const uint8_t stx = 0xDD;
const uint8_t addr = 0x01;
const uint8_t masterAddr = 0x00;
const uint8_t len = 8;
const uint8_t cmdResponseLen = 4;

/*  Frame format:
 *  [0xDD][ADDR][CMD][CH][CHSTATE][PWM1][PWM2][CRC]
 */ 

uint8_t msgBuf[8];
uint8_t sendBuf[4];
uint8_t numBytesRead;
uint8_t stxReceived;
uint8_t firstTimeStxReceived;

void setupGpio() {
  
  for (int i=0; i < sizeof(outputPins)/sizeof(gpioInit_t); i++) {
    
     pinMode(outputPins[i].pinNum, OUTPUT);
     digitalWrite(outputPins[i].pinNum, outputPins[i].val);
     
  }
  
}

uint8_t setPwm(uint8_t channel, uint8_t val) {
  
  if (val <= 255) {
    
    switch(channel) {
      
      case 0x00:
        analogWrite(led1_pwm, val);
        return 1;
        break;
      case 0x01:
        analogWrite(led2_pwm, val);
        return 1;
        break;
      case 0x02:
        analogWrite(led1_pwm, val);
        analogWrite(led2_pwm, val);
        return 1;
        break;
      case 0x03:
        analogWrite(dcOutput1_pwm, val);
        return 1;
        break;
      case 0x04:
        analogWrite(dcOutput2_pwm, val);
        return 1;
        break;
      case 0x05:
        analogWrite(dcOutput1_pwm, val);
        analogWrite(dcOutput2_pwm, val);
        return 1;
        break;
      case 0x06:
        analogWrite(led1_pwm, val);
        analogWrite(led2_pwm, val);
        analogWrite(dcOutput1_pwm, val);
        analogWrite(dcOutput2_pwm, val);
        return 1;
        break;
      default:
        return 0;
        break;
        
    }
    
  } else {
    
    return 0;
    
  }
  
}

uint8_t ledPowerCtrl(uint8_t channel, uint8_t val) {
  switch(channel) {
    case 0x00:
      digitalWrite(led1_ena, !val);
      return 1;
      break;
    case 0x01:
      digitalWrite(led2_ena, !val);
      return 1;
      break;
    case 0x02:
      digitalWrite(led1_ena, !val);
      digitalWrite(led2_ena, !val);
      return 1;
      break;
    default:
      return 0;
      break;
  }
}

uint8_t dcOutputPowerCtrl(uint8_t channelId, uint8_t val) {
  if (val == 1) {
    digitalWrite(dcOutput_ena, HIGH);
  } else {
    digitalWrite(dcOutput_ena, LOW);
  }
  
  switch(channelId) {
    case 0x00:
      digitalWrite(dcOutput1_ena, val);
      return 1;
      break;
    case 0x01:
      digitalWrite(dcOutput2_ena, val);
      return 1;
      break;
    case 0x02:
      digitalWrite(dcOutput1_ena, val);
      digitalWrite(dcOutput2_ena, val);
      return 1;
      break;
    default:
      return 0;
      break;
  }
}

void flashLeds(uint8_t channel, uint8_t iterations, uint8_t onPeriod, uint8_t offPeriod) {
  
  ledPowerCtrl(channel, 1);
  
  for (int i=0; i<=(iterations-1); i++) {
    
    setPwm(channel, 255);
    delay(onPeriod);
    setPwm(channel, 0);
    delay(offPeriod);
    
  }
  
  ledPowerCtrl(channel, 0);
  
}

uint8_t verifyCrc(uint8_t origCrc) {
  
  uint8_t calculatedCrc = 0;
  uint16_t sum = 0;
  
  for(uint8_t i = 0; i < (len - 1); i++) {
    
    sum += msgBuf[i];
    
  }
  
  calculatedCrc = sum & 0xFF;
  
  if(origCrc == calculatedCrc) {
    
    return 1;
    
  } else {
    
    return 0;
    
  }
  
}

uint8_t calculateCrc() {
  uint8_t calculatedCrc = 0;
  uint16_t sum = 0;

  for(uint8_t i = 0; i < (cmdResponseLen - 1); i++) {
    sum += sendBuf[i];
  }
  calculatedCrc = sum & 0xFF;

  return calculatedCrc;
}

void cmdResponse(uint8_t cmd) {
  
  sendBuf[0] = stx;
  sendBuf[1] = masterAddr;
  sendBuf[2] = cmd;
  sendBuf[3] = calculateCrc();

  for (int c=0; c<=cmdResponseLen; c++) {
    Serial_write(sendBuf[c]);
  }
  
}

void setup() {
  
  Serial_begin(19200);
  
  msgBuf[0] = 0xDD;
  
  numBytesRead = 0;
  stxReceived = 0;
  firstTimeStxReceived = 0;
  
  setupGpio();
  flashLeds(2, 4, 500, 500);
  
}

void loop() {
  
  if(Serial_available() > 0) {
    
    uint8_t b = Serial_read();
    
    if(b == stx) {
      
      if(!firstTimeStxReceived) {
        
        stxReceived = 1;
        numBytesRead = 0;
        firstTimeStxReceived = 1;
        
      }
      
    }
    
    msgBuf[numBytesRead] = b;
    numBytesRead++;
    
    if(numBytesRead >= len) {
      
      numBytesRead = 0;
      
      if(stxReceived) {

        uint8_t cmd = msgBuf[2];
        uint8_t crc = msgBuf[7];
        
        if(msgBuf[1] == addr && verifyCrc(crc)) {

          switch(cmd) {
            
            case 0x00:
              ledPowerCtrl(msgBuf[3], msgBuf[4]);
              dcOutputPowerCtrl(msgBuf[3], msgBuf[4]);
              setPwm(msgBuf[5], msgBuf[6]);
              break;
            case 0x01:
              ledPowerCtrl(msgBuf[3], msgBuf[4]);
              setPwm(0, msgBuf[5]);
              setPwm(1, msgBuf[6]);
              break;
            case 0x02:
              dcOutputPowerCtrl(msgBuf[3], msgBuf[4]);
              setPwm(3, msgBuf[5]);
              setPwm(4, msgBuf[6]);
              break;
            default:
              break;
              
          }
          
        }
        
        stxReceived = 0;
        firstTimeStxReceived = 0;

        cmdResponse(cmd);
        
      }
      
    }
    
  }
  
}
