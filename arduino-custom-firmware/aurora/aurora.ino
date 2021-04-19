#define led1_ena 1
#define led2_ena 2
#define led1_pwm 23
#define led2_pwm 24

const uint8_t stx = 0xDD;
const uint8_t addr = 0x01;
const uint8_t len = 7;

/*  Frame format:
 *  [0xDD][ADDR][PWR][PWRVAL][LED1BRT][LED2BRT][CRC]
 */ 

uint8_t msgBuf[len];
uint8_t numBytesRead;
uint8_t stxReceived;
uint8_t firstTimeStxReceived;

void setupGpio() {
  pinMode(led1_ena, OUTPUT);
  pinMode(led2_ena, OUTPUT);
  pinMode(led1_pwm, OUTPUT);
  pinMode(led2_pwm, OUTPUT);  
}

uint8_t setBrightness(uint8_t led1, uint8_t led2) {
  if (led1 <= 255 && led2 <= 255) {
    analogWrite(led1_pwm, led1);
    analogWrite(led2_pwm, led2);
    return 1;
  } else {
    return 0;
  }
}

uint8_t enableLedPower(uint8_t ledId, uint8_t val) {
  switch(ledId) {
    case 0x00:
      digitalWrite(led1_ena, val);
      return 1;
      break;
    case 0x01:
      digitalWrite(led2_ena, val);
      return 1;
      break;
    case 0x02:
      digitalWrite(led1_ena, val);
      digitalWrite(led2_ena, val);
      return 1;
      break;
    default:
      return 0;
      break;
  }
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

void setup() {
  while(!Serial);
  Serial_begin(19200);
  
  msgBuf[0] = 0xDD;
  
  numBytesRead = 0;
  stxReceived = 0;
  firstTimeStxReceived = 0;
  
  setupGpio();
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
        uint8_t crc = msgBuf[6];
        
        if(msgBuf[1] == addr && verifyCrc(crc)) {
          enableLedPower(msgBuf[2], msgBuf[3]);
          setBrightness(msgBuf[4], msgBuf[5]);
        }
        
        stxReceived = 0;
        firstTimeStxReceived = 0;
      }
    }
  }
}
