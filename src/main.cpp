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

static const char *pdcStateToString(PDCStates state) {
  switch (state) {
  case PDCStates::OFF:
    return "OFF";
  case PDCStates::PARK:
    return "PARK";
  case PDCStates::IDLE:
    return "IDLE";
  case PDCStates::FORWARD:
    return "FORWARD";
  case PDCStates::REVERSE:
    return "REVERSE";
  default:
    return "UNKNOWN";
  }
}

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
  brake_pressed = rand() % 2;
  digital_data.brake_led = brake_pressed;

  // Random digital data
  digital_data.direction = rand() % 2;
  digital_data.mc_speed_sig = rand() % 2;
  digital_data.eco_mode = rand() % 2;
  digital_data.mcu_mc_on = rand() % 2;
  digital_data.park_brake = rand() % 2;
}

void debugPrint() {
#ifdef DEBUG_PRINTS
  Serial.printf("acc_in: %f\n", acc_in);
  Serial.printf("acc_out: %f\n", acc_out);
  Serial.printf("regen_brake: %f\n", regen_brake);
  Serial.printf("lv_12V_telem: %f\n", lv_12V_telem);
  Serial.printf("lv_5V_telem: %f\n", lv_5V_telem);
  Serial.printf("lv_5V_current: %f\n", lv_5V_current);
  Serial.printf("current_in_telem: %f\n", current_in_telem);
  Serial.printf("brake_pressed: %i\n", brake_pressed);
  Serial.printf("brakeLED: %i\n", digital_data.brake_led);
  Serial.printf("digital_data.direction: %i\n", digital_data.direction);
  Serial.printf("digital_data.mc_speed_sig: %i\n", digital_data.mc_speed_sig);
  Serial.printf("digital_data.eco_mode: %i\n", digital_data.eco_mode);
  Serial.printf("digital_data.mcu_mc_on: %i\n", digital_data.mcu_mc_on);
  Serial.printf("digital_data.park_brake: %i\n", digital_data.park_brake);
#endif
}
#endif

// setup
void setup() {
  Serial.begin(115200);
#ifdef DEBUG_PRINTS
  Serial.printf("Starting up...\n");
#endif
  digitalWrite(PB6, HIGH);
  initIO();
  startSpeedCalculation();

#if DEBUG_TECHNIQUE == 0
  // Production mode: start the state machine
  initPDCState();
#elif DEBUG_TECHNIQUE == 1
  // Random echo mode: seed RNG, no state machine
  // Many embedded boards don't have a real-time clock, so time(NULL)
  // can return 0 every boot. Use millis() so the seed varies based on boot
  // time.
  srand(millis());
  randomizeData();
#endif
}

// loop
void loop() {
#if DEBUG_TECHNIQUE == 0
  // Production: state machine runs via its timer interrupt.
  // Just send data and process CAN queue.
#ifdef DEBUG_PRINTS
  {
    static uint32_t lastDebugMs = 0;
    uint32_t nowMs = millis();
    if (nowMs - lastDebugMs >= 1000) {
      lastDebugMs = nowMs;
      Serial.printf(
          "state=%s can_dir=%s fr=%u acc_in_raw=%u "
          "acc_in=%.3f acc_out=%.3f regen=%.3f rpm=%.1f mph=%.1f "
          "brake=%u brake_v=%.3f park=%u mcu_dir=%u eco=%u brake_led=%u\n",
          pdcStateToString(get_state()),
          forwardAndReverse == FORWARD_VALUE ? "Fwd" : "Rev", forwardAndReverse,
          acc_in_raw, acc_in, acc_out, regen_brake, rpm, mph,
          brake_pressed, brake_pressure_telem, digital_data.park_brake,
          digital_data.direction, digital_data.eco_mode, digital_data.brake_led);
    }
  }
#endif
#elif DEBUG_TECHNIQUE == 1
  // Random echo: periodically regenerate random values
  if (counter >= COUNTER_EXP) {
    randomizeData();
    debugPrint();
    counter = 0;
    // Show what we received from the steering wheel / test board
#ifdef DEBUG_PRINTS
    Serial.printf("Received forwardAndReverse: %i\n", forwardAndReverse);
#endif
  }
  counter++;
#endif

  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}