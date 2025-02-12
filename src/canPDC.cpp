#include "canPDC.h"

CANPDC::CANPDC(CAN_TypeDef* canPort, CAN_PINS pins, int frequency) : CANManager(canPort, pins, frequency) {};

void CANPDC::readHandler(CAN_message_t msg) {
    
}

void CANPDC::sendPDCData() {
    this->sendMessage(0x200, (void*)&acc_out, sizeof(float));
    this->sendMessage(0x201, (void*)&regen_brake, sizeof(float));
    this->sendMessage(0x202, (void*)&lv_12V_telem, sizeof(float));
    this->sendMessage(0x203, (void*)&lv_5V_telem, sizeof(float));
    this->sendMessage(0x204, (void*)&lv_5V_current, sizeof(float));
    this->sendMessage(0x205, (void*)&current_in_telem, sizeof(float));
    this->sendMessage(0x206, (void*)&brake_pressure_telem, sizeof(float));
    this->sendMessage(0x207, (void*)&digital_data, sizeof(digital_data));
}