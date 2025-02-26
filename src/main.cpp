#include <Arduino.h>
#include "IOManagement.h"
#include "const.h"
#include "canPDC.h"

CANPDC canBus(CAN1, DEF);

void setup() {
  Serial.begin(115200);
  initIO();
}

void loop() {
  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}