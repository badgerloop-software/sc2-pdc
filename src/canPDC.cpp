#include "canPDC.h"

volatile bool forwardAndReverse = false;
volatile bool cruiseEnabled = false;
volatile bool cruiseSetPulse = false;
volatile bool cruiseResetPulse = false;

static bool lastCruiseSet = false;
static bool lastCruiseReset = false;
static uint8_t lastDriveMode = 0xFF;

static constexpr uint16_t THROTTLE_REST_COUNTS = 869U;
static constexpr uint16_t THROTTLE_FULL_COUNTS = 3228U;

CANPDC::CANPDC(CAN_TypeDef *canPort, CAN_PINS pins, int frequency)
    : CANManager(canPort, pins, frequency) {};

void CANPDC::readHandler(CAN_message_t msg) {
  switch (msg.id) {
  case FORWARD_AND_REVERSE_ID: // 0x300
    // Byte 0 bit layout per CAN spec:
    //   bit 0: headlight, bit 1: left_blink, bit 2: right_blink,
    //   bit 3: direction_switch, bit 4: horn, bit 5: crz_mode_a,
    //   bit 6: crz_set, bit 7: crz_reset
    forwardAndReverse = (msg.buf[0] >> 3) & 1; // bit 3 = direction_switch
    cruiseEnabled = (msg.buf[0] >> 5) & 1;

    bool cruiseSetNow = ((msg.buf[0] >> 6) & 1) != 0;
    bool cruiseResetNow = ((msg.buf[0] >> 7) & 1) != 0;
    if (cruiseSetNow && !lastCruiseSet) {
      cruiseSetPulse = true;
    }
    if (cruiseResetNow && !lastCruiseReset) {
      cruiseResetPulse = true;
    }
    lastCruiseSet = cruiseSetNow;
    lastCruiseReset = cruiseResetNow;

    // Byte 1 (testing extension — allows test board to drive these over CAN):
    //   bit 0: park_brake, bit 1: mcu_mc_on
    digital_data.park_brake = msg.buf[1] & 1;
    // mcu_mc_on is also readable from the GPIO (PA10). The CAN value from the
    // test board overrides the GPIO when the test board is active.
    digital_data.mcu_mc_on = (msg.buf[1] >> 1) & 1;
    break;

  case REGEN_BRAKE_INPUT_ID: { // 0x301 regen brake from steering wheel (normalized 0.0–1.0)
    if (msg.len >= sizeof(float)) {
      float regen_val = 0.0f;
      memcpy((void *)&regen_val, msg.buf, sizeof(float));
      if (regen_val < 0.0f)
        regen_val = 0.0f;
      else if (regen_val > 1.0f)
        regen_val = 1.0f;
      regen_in = regen_val;
    }
    break;
  }

#ifndef TEST_MODE
  case THROTTLE_INPUT_ID: { // 0x302 uint16 throttle command in production.
    uint16_t throttle_raw = 0;
    memcpy((void *)&throttle_raw, msg.buf, sizeof(uint16_t));
    acc_in_raw = throttle_raw;

    // Normalize using calibrated pedal range and clamp to [0, 1].
    float normalized =
        ((float)throttle_raw - (float)THROTTLE_REST_COUNTS) /
        ((float)THROTTLE_FULL_COUNTS - (float)THROTTLE_REST_COUNTS);
    if (normalized < 0.0f) {
      normalized = 0.0f;
    } else if (normalized > 1.0f) {
      normalized = 1.0f;
    }
    acc_in = normalized;
    break;
  }

  case DRIVE_MODE_INPUT_ID: { // 0x303 eco/pwr from steering wheel (0=Eco, 1=Pwr)
    if (msg.len >= 1 && msg.buf[0] != lastDriveMode) {
      lastDriveMode = msg.buf[0];
      set_eco_mode(lastDriveMode == 0);
    }
    break;
  }
#endif

#ifdef TEST_MODE
  case 0x209: // acc_in — sent by test board simulating the pedal.
              // Only used in TEST_MODE; in production 0x302 is used.
    memcpy((void *)&acc_in, msg.buf, sizeof(float));
    break;
#endif

  default:
    break;
  }
}

void CANPDC::sendPDCData() {
  this->sendMessage(0x200, (void *)&acc_out, sizeof(float));
  this->sendMessage(0x201, (void *)&regen_brake, sizeof(float));
  this->sendMessage(0x202, (void *)&lv_12V_telem, sizeof(float));
  this->sendMessage(0x203, (void *)&lv_5V_telem, sizeof(float));
  this->sendMessage(0x204, (void *)&lv_5V_current, sizeof(float));
  this->sendMessage(0x205, (void *)&current_in_telem, sizeof(float));
  this->sendMessage(0x206, (void *)&brake_pressure_telem, sizeof(float));
  this->sendMessage(0x207, (void *)&digital_data, sizeof(digital_data));
  this->sendMessage(0x208, (void *)&mph, sizeof(float));
  // 0x209/0x302 are INPUTs received from the pedal/test board — do not
  // re-broadcast.
}