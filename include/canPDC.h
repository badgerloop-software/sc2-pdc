#ifndef __CAN_PDC_H__
#define __CAN_PDC_H__

#include "canmanager.h"
#include "analogPins.h"
#include "digitalPins.h"

class CANPDC : public CANManager {
    public:
        CANPDC(CAN_TypeDef* canPort, CAN_PINS pins, int frequency = DEFAULT_CAN_FREQ);
        void readHandler(CAN_message_t msg);
        void sendPDCData();
};

extern volatile bool brakeLED;
extern volatile bool forwardAndReverse;
extern volatile bool start_state_machine;

#endif __CAN_PDC_H__