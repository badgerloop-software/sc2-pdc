#include "canPDC.h"

CANPDC::CANPDC(CAN_TypeDef* canPort, CAN_PINS pins, int frequency) : CANManager(canPort, pins, frequency) {};

void CANPDC::readHandler(CAN_message_t msg) {
    
}

void CANPDC::sendPDCData() {
    readAnalog();

    readDigital();
    this->sendMessage(0x207, (void*)digital_data, sizeof(digital_data));
}