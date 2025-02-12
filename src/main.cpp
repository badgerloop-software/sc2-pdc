#include <Arduino.h>
#include "analogPins.h"
#include "digitalPins.h"
#include "const.h"
#include "canPDC.h"

CANPDC canBus(CAN1, DEF);

void setup() {
  Serial.begin(115200);
  initAnalog();
}

void loop() {
  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}