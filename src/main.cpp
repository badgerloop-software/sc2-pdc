// sc2-pdc main: Power Distribution and Controls on Nucleo-L432KC
// Boot order: Serial, IO, speed calc, state machine - Loop sends CAN telem
// The state machine runs on a timer interrupt, not in loop()
#include <Arduino.h>

#include "IOManagement.h"
#include "canPDC.h"
#include "const.h"
#include "motor_control.h"
#include "speed_calc.h"

// DEBUG_TECHNIQUE selects the operating mode:
// 0 = production (state machine and real IO)
// 1 = random CAN echo (no state machine, for bus tests)
#define DEBUG_TECHNIQUE 0

#if DEBUG_TECHNIQUE == 1
#include <stdlib.h>
int counter = 0;
#endif

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
  acc_out = ((float)rand() / RAND_MAX);     // 0.0 to 1.0
  regen_brake = ((float)rand() / RAND_MAX); // 0.0 to 1.0
  lv_12V_telem =
      ((float)rand() / RAND_MAX) * 12.0; // 0 to 12 V after divider scaling
  lv_5V_telem = ((float)rand() / RAND_MAX) * 5.0;          // 0 to 5 V
  lv_5V_current = ((float)rand() / RAND_MAX) * 3.3;        // 0 to 3.3 V
  current_in_telem = ((float)rand() / RAND_MAX) * 3.3;     // 0 to 3.3 V
  brake_pressed = rand() % 2;
  digital_data.brake_led = brake_pressed;

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

void setup() {
  Serial.begin(115200);
#ifdef DEBUG_PRINTS
  Serial.printf("Starting up...\n");
#endif
  digitalWrite(PB6, HIGH);
  initIO();
  startSpeedCalculation();

#if DEBUG_TECHNIQUE == 0
  // Start PARK/IDLE/FORWARD/REVERSE on TIM2
  initPDCState();
#elif DEBUG_TECHNIQUE == 1
  // Seed with millis(), no real time clock so time(NULL) is always 0
  srand(millis());
  randomizeData();
#endif
}

void loop() {
#if DEBUG_TECHNIQUE == 0
  // State machine runs on its timer, loop() only sends CAN and reads RX
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
  if (counter >= COUNTER_EXP) {
    randomizeData();
    debugPrint();
    counter = 0;
#ifdef DEBUG_PRINTS
    Serial.printf("Received forwardAndReverse: %i\n", forwardAndReverse);
#endif
  }
  counter++;
#endif

  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}
