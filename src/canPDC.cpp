#include "canPDC.h"

volatile bool brakeLED = false;
volatile bool forwardAndReverse = false;
volatile bool mc_on = false;

CANPDC::CANPDC(CAN_TypeDef* canPort, CAN_PINS pins, int frequency) : CANManager(canPort, pins, frequency) {};

void CANPDC::readHandler(CAN_message_t msg) {
    switch(msg.id){
        case FORWARD_AND_REVERSE_ID:
            forwardAndReverse = msg.buf[0];
            break;
        default:
            break;
    }    
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
    this->sendMessage(0x20A, (void*)&brakeLED, sizeof(bool));
    this->sendMessage(0x208, (void*)&mph, sizeof(float));
}