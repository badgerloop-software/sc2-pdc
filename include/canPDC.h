#ifndef __CAN_PDC_H__
#define __CAN_PDC_H__

#include "IOManagement.h"
#include "canmanager.h"

// Macros for the CAN message IDs
#define FORWARD_AND_REVERSE_ID 0x300

class CANPDC : public CANManager {
public:
  CANPDC(CAN_TypeDef *canPort, CAN_PINS pins, int frequency = DEFAULT_CAN_FREQ);
  void readHandler(CAN_message_t msg);
  void sendPDCData();
};

extern volatile bool forwardAndReverse;

#endif