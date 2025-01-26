#ifndef __ANALOG_H__
#define __ANALOG_H__

float acc_out;
float regen_brake;

float lv_12V_telem;
float lv_5V_telem;
float lv_5V_current;
float current_in_telem;
float brake_pressure_telem;

void readAnalog();

void initAnalog();


#endif