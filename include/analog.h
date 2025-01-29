#ifndef __ANALOG_H__
#define __ANALOG_H__

#include "adc.h"
#include "const.h"


void initAnalog();

void readAnalog();

void writeAccOut(float newAccOut);

void writeRegenBrake(float newRegenBrake);

#endif