#ifndef __ANALOG_H__
#define __ANALOG_H__

#include "adc.h"
#include "const.h"
#include "STM32TimerInterrupt_Generic.h"

extern volatile float acc_out;
extern volatile float regen_brake;

extern volatile float lv_12V_telem;
extern volatile float lv_5V_telem;
extern volatile float lv_5V_current;
extern volatile float current_in_telem;
extern volatile float brake_pressure_telem;

void initAnalog();

void readAnalog();

void writeAccOut(float newAccOut);

void writeRegenBrake(float newRegenBrake);

#endif
