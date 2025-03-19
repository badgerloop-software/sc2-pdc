#include <Arduino.h>
#include "IOManagement.h"
#include "const.h"
#include "canPDC.h"
#include "speed_calc.h"

CANPDC canBus(CAN1, DEF);

void setup() {
  Serial.begin(115200);
  initIO();
  startSpeedCalculation();
}

void loop() {
  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}
