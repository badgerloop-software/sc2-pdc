#ifndef __CAN_PDC_H__
#define __CAN_PDC_H__

#include "canmanager.h"
#include "analogPins.h"
#include "digitalPins.h"

//Macros for the CAN message IDs
#define FORWARD_AND_REVERSE_ID 0x300
#define MC_ON_ID 0x301

class CANPDC : public CANManager {
    public:
        CANPDC(CAN_TypeDef* canPort, CAN_PINS pins, int frequency = DEFAULT_CAN_FREQ);
        void readHandler(CAN_message_t msg);
        void sendPDCData();
};

extern volatile bool brakeLED;
extern volatile bool forwardAndReverse;
extern volatile bool mc_on;

#endif