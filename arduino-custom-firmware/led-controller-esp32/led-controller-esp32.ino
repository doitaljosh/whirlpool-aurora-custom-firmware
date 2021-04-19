const uint8_t len = 8;
const uint8_t stx = 0xDD;
const uint8_t ledAddr = 0x01;

uint8_t msgBuf[8];

uint8_t ledId = 0;
uint8_t ledState = 0;
uint8_t led1Brightness = 0;
uint8_t led2Brightness = 0;

uint8_t calculateCrc() {
  uint8_t calculatedCrc = 0;
  uint16_t sum = 0;

  for(uint8_t i = 0; i < (len - 1); i++) {
    sum += msgBuf[i];
  }
  calculatedCrc = sum & 0xFF;

  return calculatedCrc;
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(19200);

  msgBuf[0] = stx;
}

void loop() {

  ledId = 2;
  ledState = 1;

  if(led1Brightness == 255) {
    led1Brightness = 0;
  } else {
    led1Brightness++;
  }

  msgBuf[1] = ledAddr;
  msgBuf[2] = 0x01;
  msgBuf[3] = ledId;
  msgBuf[4] = ledState;
  msgBuf[5] = led1Brightness;
  msgBuf[6] = led1Brightness;
  msgBuf[7] = calculateCrc();

  Serial.print("Sending message: ");
  for(int i=0; i<=len; i++) {
    Serial.print(msgBuf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  Serial2.write(msgBuf, len);

  delay(40);
}
