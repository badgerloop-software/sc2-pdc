#include <Arduino.h>

#include "canPDC.h"
#include "const.h"
#include "IOManagement.h"
#include "motor_control.h"
#include "adc.h"

//For random
#include <stdlib.h>
#include <time.h>

int counter;
CANPDC canBus(CAN1, DEF);


#if DEBUG_TECHNIQUE == 1
void initData() {
  // Generate a random float between 0 and 100
  acc_out = ((float)rand() / RAND_MAX) * 100;
  regen_brake = ((float)rand() / RAND_MAX) * 100;
  lv_12V_telem = ((float)rand() / RAND_MAX) * 100;
  lv_5V_telem = ((float)rand() / RAND_MAX) * 100;
  lv_5V_current = ((float)rand() / RAND_MAX) * 100;
  current_in_telem = ((float)rand() / RAND_MAX) * 100;
  brake_pressure_telem = ((float)rand() / RAND_MAX) * 100;
  
  //Random boolean
  brakeLED = rand() % 2;

  //Random digital data
  digital_data.direction = rand() % 2;
  digital_data.mc_speed_sig = rand() % 2;
  digital_data.eco_mode = rand() % 2;
  digital_data.mcu_mc_on = rand() % 2;
  digital_data.park_brake = rand() % 2;
}
#endif


void setup() {
  #if DEBUG_TECNIQUE
    int counter = 0;
  #endif

  Serial.begin(115200);
  initADC(ADC1); 
  initIO();
  initPDCState();

  //initialize random
  srand(time(NULL));
}

void loop() {
  initData();
  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}