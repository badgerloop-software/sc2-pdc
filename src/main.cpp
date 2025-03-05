#include <Arduino.h>

#include "canPDC.h"
#include "const.h"
#include "IOManagement.h"
#include "motor_control.h"
#include "adc.h"

//For random
#include <stdlib.h>
#include <time.h>

#define DEBUG_TECHNIQUE     1

int counter = 0;

//Counter for debug prints 100 ticks equates to 5 seconds

CANPDC canBus(CAN1, DEF);


#if DEBUG_TECHNIQUE == 1
//Define methods
void randomizeData();
void debugPrint();

void randomizeData() {
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
void debugPrint() {
  Serial.printf("acc_out: %f\n", acc_out);
  Serial.printf("regen_brake: %f\n", regen_brake);
  Serial.printf("lv_12V_telem: %f\n", lv_12V_telem);
  Serial.printf("lv_5V_telem: %f\n", lv_5V_telem);
  Serial.printf("lv_5V_current: %f\n", lv_5V_current);
  Serial.printf("current_in_telem: %f\n", current_in_telem);
  Serial.printf("brake_pressure_telem: %f\n", brake_pressure_telem);
  Serial.printf("brakeLED: %i\n", brakeLED);
  Serial.printf("digital_data.direction: %i\n", digital_data.direction);
  Serial.printf("digital_data.mc_speed_sig: %i\n", digital_data.mc_speed_sig);
  Serial.printf("digital_data.eco_mode: %i\n", digital_data.eco_mode);
  Serial.printf("digital_data.mcu_mc_on: %i\n", digital_data.mcu_mc_on);
  Serial.printf("digital_data.park_brake: %i\n", digital_data.park_brake);
}
#endif


void setup() {
  Serial.begin(115200);
  // initADC(ADC1); 
  // initIO();
  // initPDCState();

  //initialize random
  srand(time(NULL));
  randomizeData();
}

void loop() {
  // Display digital and analog values every second (for testing) 
  #if DEBUG_TECHNIQUE == 1
    if (counter >= COUNTER_EXP) {
      randomizeData();
      debugPrint();
      counter = 0;
      //Receiving
      Serial.printf("Received forwardANdReverse: %i\n", forwardAndReverse);
    }
    counter++;
  #endif
  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}