#include <Arduino.h>
#include "analogPins.h"
#include "digitalPins.h"
#include "const.h"
#include "canPDC.h"

// put function declarations here:
CANPDC canBus(CAN1, DEF);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initAnalog();
  initADC(ADC1);
}

void loop() {
  // put your main code here, to run repeatedly:
  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}

// put function definitions here:

