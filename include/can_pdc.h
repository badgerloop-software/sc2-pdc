#ifndef __CAN_PDC_H__
#define __CAN_PDC_H__

#include "canmanager.h"
#include "io_management.h"

// ------------- CLASS -------------

// IDs / DLCs from embedded-pio/can_ids.h (CAN_*)
class CanPdc : public CANManager {
   public:
    CanPdc(CAN_TypeDef* canPort, CAN_PINS pins, int frequency = DEFAULT_CAN_FREQ);
    void readHandler(CAN_message_t msg) override;
    void sendPDCData();
};

extern volatile bool forwardAndReverse;

#endif  // __CAN_PDC_H__
