#ifndef __CAN_PDC_H__
#define __CAN_PDC_H__

#include "IOManagement.h"
#include "zephyrcan.h"

// Macros for the CAN message IDs
#define FORWARD_AND_REVERSE_ID 0x300
#define REGEN_BRAKE_INPUT_ID 0x301
#define THROTTLE_INPUT_ID 0x302
#define DRIVE_MODE_INPUT_ID 0x303


const uint8_t ARR_IDS [] =  {FORWARD_AND_REVERSE_ID, REGEN_BRAKE_INPUT_ID, THROTTLE_INPUT_ID, DRIVE_MODE_INPUT_ID};  

class CANPDC : public ZephyrCAN {
public:
  CANPDC(CAN_TypeDef *canPort, CAN_PINS pins, int frequency = DEFAULT_CAN_FREQ);
  void readHandler(struct can_frame * msg);
  void sendPDCData();
};

extern volatile bool forwardAndReverse;

#endif
