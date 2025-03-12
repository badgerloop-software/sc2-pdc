#include <Arduino.h>
#include "IOManagement.h"
#include "const.h"
#include "canPDC.h"
#include "speed_calc.h"

CANPDC canBus(CAN1, DEF);

uint8_t counter = 0;

void setup() {
  Serial.begin(115200);
  initIO();
  startSpeedCalculation();
}

void loop() {
  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
  counter++;
  if (counter >= 10) {
    counter = 0;

    // clear the screen
    printf("\e[1;1H\e[2J");

    // digital inputs 
    printf("mc_on: %d\n", digital_data.mc_on);
    printf("park_brake: %d\n", digital_data.park_brake);
    
    // analog inputs
    printf("acc_in: %f\n", acc_in);
    printf("12V: %f\n", lv_12V_telem);
    printf("5V: %f\n", lv_5V_telem);
    printf("5V current: %f\n", lv_5V_current);
    printf("current in: %f\n", current_in_telem);
    printf("brake pressure: %f\n", brake_pressure_telem);
  }
}
