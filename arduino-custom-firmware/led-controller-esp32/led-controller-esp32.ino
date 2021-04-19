const uint8_t len = 8;
const uint8_t stx = 0xDD;
const uint8_t masterAddr = 0x00;
const uint8_t ledAddr = 0x01;

uint8_t sendBufLen = 8;
uint8_t rcvBufLen = 4;

uint8_t sendBuf[8];
uint8_t rcvBuf[4];
uint8_t numBytesRead;
uint8_t stxReceived;
uint8_t firstTimeStxReceived;

uint8_t cmdToSend = 0x01;
uint8_t ledId = 0;
uint8_t ledState = 0;
uint8_t led1Brightness = 0;
uint8_t led2Brightness = 0;

uint8_t calculateCrc() {
  
  uint8_t calculatedCrc = 0;
  uint16_t sum = 0;

  for(uint8_t i = 0; i < (sendBufLen - 1); i++) {
    
    sum += sendBuf[i];
    
  }
  
  calculatedCrc = sum & 0xFF;

  return calculatedCrc;
  
}

uint8_t verifyCrc(uint8_t origCrc) {
  
  uint8_t calculatedCrc = 0;
  uint16_t sum = 0;
  
  for(uint8_t i = 0; i < (rcvBufLen - 1); i++) {
    
    sum += rcvBuf[i];
    
  }
  
  calculatedCrc = sum & 0xFF;
  
  if(origCrc == calculatedCrc) {
    
    return 1;
    
  } else {
    
    return 0;
    
  }
  
}

uint8_t verifyCmdResponse(uint8_t cmdExpected) {

  if(Serial2.available() > 0) {
    
    uint8_t b = Serial2.read();

    if(b == stx) {

      if(!firstTimeStxReceived) {

        stxReceived = 1;
        numBytesRead = 0;
        firstTimeStxReceived = 1;

      }
      
    }

    rcvBuf[numBytesRead] = b;
    numBytesRead++;

    if(numBytesRead >= rcvBufLen) {

      numBytesRead = 0;

      if(stxReceived) {

        uint8_t cmdReceived = rcvBuf[2];
        uint8_t crcReceived = rcvBuf[3];

        if(rcvBuf[1] == masterAddr && cmdReceived == cmdExpected && verifyCrc(crcReceived)) {
          return 1;
        } else {
          return 0;
        }
        
        stxReceived = 0;
        firstTimeStxReceived = 0;
        
      }
      
    }
    
  } else {
    
    return 0;
    
  }
  
}

void setup() {
  
  Serial.begin(115200);
  Serial2.begin(19200);

  sendBuf[0] = stx;
  
}

void loop() {

  ledId = 2;
  ledState = 1;

  if(led1Brightness == 255) {
    
    led1Brightness = 0;
    
  } else {
    
    led1Brightness++;
    
  }

  sendBuf[1] = ledAddr;
  sendBuf[2] = cmdToSend;
  sendBuf[3] = ledId;
  sendBuf[4] = ledState;
  sendBuf[5] = led1Brightness;
  sendBuf[6] = led1Brightness;
  sendBuf[7] = calculateCrc();

  Serial.print("Sending message: ");
  
  for(int i=0; i<=sendBufLen; i++) {
    
    Serial.print(sendBuf[i], HEX);
    Serial.print(" ");
    
  }
  
  Serial.println();

  Serial2.write(sendBuf, len);

  if(verifyCmdResponse(cmdToSend) == 1) {
    
    Serial.println("Received OK response.");
    
  } else {
    
    Serial.println("LED controller not responding correctly!");
    
  }

  delay(40);
  
}
