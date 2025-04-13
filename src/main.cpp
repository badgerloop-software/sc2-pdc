#include <Arduino.h>
#include "IOManagement.h"
#include "dac.h"
#include "const.h"
#include "canPDC.h"
#include "speed_calc.h"
#include "motor_control.h"

CANPDC canBus(CAN1, DEF);

uint8_t counter = 0;
bool digital_output = false;

void setup() {
  Serial.begin(115200);
  initIO();
  initPDCState();
  startSpeedCalculation();
}

void loop() {
  canBus.sendPDCData();
  canBus.runQueue(50);
  counter++;
  if (counter >= 20) {
    counter = 0;
    // clear the screen
    printf("\e[1;1H\e[2J");

    // state
    printf("State: ");
    switch (pdcState) {
      case PDCStates::OFF:
        printf("OFF\n");
        break;
      case PDCStates::PARK:
        printf("PARK\n");
        break;
      case PDCStates::IDLE:
        printf("IDLE\n");
        break;
      case PDCStates::FORWARD:
        printf("FORWARD\n");
        break;
      case PDCStates::REVERSE:
        printf("REVERSE\n");
        break;
      case PDCStates::CRUISE_POWER:
        printf("CRUISE_POWER\n");
        break;
      case PDCStates::CRUISE_SPEED:
        printf("CRUISE_SPEED\n");
        break;
      default:
        printf("unknown\n");
        break;
    }

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
    printf("speed: %f\n", rpm);

  }
}
