#include "canPDC.h"

volatile bool forwardAndReverse = false;

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
    // Byte 1 (testing extension — allows test board to drive these over CAN):
    //   bit 0: park_brake, bit 1: mcu_mc_on
    digital_data.park_brake = msg.buf[1] & 1;
    // mcu_mc_on is also readable from the GPIO (PA10). The CAN value from the
    // test board overrides the GPIO when the test board is active.
    digital_data.mcu_mc_on = (msg.buf[1] >> 1) & 1;
    break;

  case 0x209: // acc_in — sent by the pedal board (or test board simulating the
              // pedal)
    memcpy((void *)&acc_in, msg.buf, sizeof(float));
    break;

#ifdef TEST_MODE
  case 0x20A: // rpm — sent by test board (no physical PWM wire during bench
              // testing)
    memcpy((void *)&rpm, msg.buf, sizeof(float));
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
  // 0x209 (acc_in) is an INPUT received from the pedal board — do not
  // re-broadcast.
}