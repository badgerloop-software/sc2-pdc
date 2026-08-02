#ifndef __IO_MANAGEMENT_H__
#define __IO_MANAGEMENT_H__

#include <Arduino.h>
#include <HardwareTimer.h>
#ifndef MICROSEC_FORMAT
#define MICROSEC_FORMAT TimerFormat_t::MICROSEC_FORMAT
#endif
#include "STM32TimerInterrupt_Generic.h"
#include "adc.h"
#include "board_config.h"

// ------------- TYPES -------------

struct Digital_Data {
    bool direction : 1;
    bool mc_speed_sig : 1;
    bool eco_mode : 1;
    bool mcu_mc_on : 1;
    bool park_brake : 1;
    bool brake_led : 1;
};

// ------------- GLOBALS -------------

extern volatile Digital_Data digital_data;

extern volatile uint16_t acc_in_raw;
extern volatile float acc_in;
extern volatile float acc_out;
extern volatile float regen_in;
extern volatile float regen_brake;
extern volatile float lv_12V_telem;
extern volatile float lv_5V_telem;
extern volatile float lv_5V_current;
extern volatile float current_in_telem;
extern volatile bool brake_pressed;
extern volatile float brake_pressure_telem;
extern volatile float mph;
extern volatile float rpm;

// ------------- FUNCTIONS -------------

void initIO();
void initSpeedCounter();
void readIO();
void set_direction(bool dir);
void set_eco_mode(bool eco);
void writeAccOut(float newAccOut);
void writeRegenBrake(float newRegenBrake);

#endif  // __IO_MANAGEMENT_H__
