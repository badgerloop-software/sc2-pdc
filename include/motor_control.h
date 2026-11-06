#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#include "const.h"
#include "IOManagement.h"
#include "canPDC.h"

void initPDCState();
void transition();
PDCStates get_state();

#endif
