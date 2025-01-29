#ifndef __ANALOG_H__
#define __ANALOG_H__

#include <Arduino.h>
#include "adc.h"
#include "const.h"
#include "STM32TimerInterrupt_Generic.h"

float acc_out;
float regen_brake;

float lv_12V_telem;
float lv_5V_telem;
float lv_5V_current;
float current_in_telem;
float brake_pressure_telem;

void initAnalog();

void readAnalog();

void writeAccOut(float newAccOut);

void writeRegenBrake(float newRegenBrake);

#endif