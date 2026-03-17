#include <Arduino.h>

#include "IOManagement.h"
#include "adc.h"
#include "canPDC.h"
#include "const.h"
#include "motor_control.h"
#include "speed_calc.h"

// For random
#include <stdlib.h>
#include <time.h>

// DEBUG_TECHNIQUE selects the operating mode:
//   0 = Production / normal operation (state machine + real I/O)
//   1 = Random CAN echo (no state machine, sends random data for CAN testing)
#define DEBUG_TECHNIQUE 0

int counter = 0;

CANPDC canBus(CAN1, DEF);

// DEBUG_TECHNIQUE == 1  —>  Random CAN echo mode
#if DEBUG_TECHNIQUE == 1
void randomizeData();
void debugPrint();

void randomizeData() {
  // Use realistic ranges for each signal
  acc_out = ((float)rand() / RAND_MAX);     // 0.0–1.0 (AnalogOut range)
  regen_brake = ((float)rand() / RAND_MAX); // 0.0–1.0
  lv_12V_telem =
      ((float)rand() / RAND_MAX) * 12.0; // 0–12V (after divider scaling)
  lv_5V_telem = ((float)rand() / RAND_MAX) * 5.0;          // 0–5V
  lv_5V_current = ((float)rand() / RAND_MAX) * 3.3;        // 0–3.3V
  current_in_telem = ((float)rand() / RAND_MAX) * 3.3;     // 0–3.3V
  brake_pressure_telem = ((float)rand() / RAND_MAX) * 3.3; // 0–3.3V

  // Random digital data
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
  Serial.printf("brakeLED: %i\n", digital_data.brake_led);
  Serial.printf("digital_data.direction: %i\n", digital_data.direction);
  Serial.printf("digital_data.mc_speed_sig: %i\n", digital_data.mc_speed_sig);
  Serial.printf("digital_data.eco_mode: %i\n", digital_data.eco_mode);
  Serial.printf("digital_data.mcu_mc_on: %i\n", digital_data.mcu_mc_on);
  Serial.printf("digital_data.park_brake: %i\n", digital_data.park_brake);
}
#endif

// setup
void setup() {
  Serial.begin(115200);
  Serial.printf("Starting up...\n");
  digitalWrite(PB6, HIGH);
  initIO();
  startSpeedCalculation();

#if DEBUG_TECHNIQUE == 0
  // Production mode: start the state machine
  initPDCState();
#elif DEBUG_TECHNIQUE == 1
  // Random echo mode: seed RNG, no state machine
  // Many embedded boards don't have a real-time clock, so time(NULL)
  // can return 0 every boot. Use millis() so the seed varies based on boot time.
  srand(millis());
  randomizeData();
#endif
}

// loop
void loop() {
  // Display digital and analog values every second (for testing)

  digitalWrite(PB6, HIGH);
  delay(500);
  digitalWrite(PB6, LOW);

  delay(500);

#if DEBUG_TECHNIQUE == 0
  // Production: state machine runs via its timer interrupt.
  // Just send data and process CAN queue.
#elif DEBUG_TECHNIQUE == 1
  // Random echo: periodically regenerate random values
  if (counter >= COUNTER_EXP) {
    randomizeData();
    debugPrint();
    counter = 0;
    // Show what we received from the steering wheel / test board
    Serial.printf("Received forwardAndReverse: %i\n", forwardAndReverse);
  }
  counter++;
#endif

  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}