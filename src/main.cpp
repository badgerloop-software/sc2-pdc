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
  canBus.runQueue(1000);

  Serial.print("RPM: ");
  Serial.println(rpm);
  Serial.print("MPH: ");
  Serial.println(mph);

  for (int i = 0; i < 20; i++) {
    printf("%d |", previousPulses[i]);
  }

  printf("\n");
}
