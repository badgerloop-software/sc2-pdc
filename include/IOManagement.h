#ifndef __IO_MANAGER_H__
#define __IO_MANAGER_H__

#include <Arduino.h>
#include "STM32TimerInterrupt_Generic.h"
#include "adc.h"
#include "const.h"

// Outputs
#define MCU_DIR PB7
#define MCU_ECO PB1

// Inputs
#define MCU_MC_ON PA10
#define MCU_SPEED_SIG PA8
#define PRK_BRK_TELEM PB4
#define BRAKE_TELEM PA0

struct Digital_Data {
  bool direction : 1;    // output
  bool mc_speed_sig : 1; // input
  bool eco_mode : 1;     // output
  bool mcu_mc_on : 1;    // input (motor controller power signal)
  bool park_brake : 1;   // always false in production (sensor removed)
  bool brake_led : 1;    // output (pedal brake or regen >= threshold)
};

// Shared telem: IOManagement owns most but speed_calc writes rpm and mph
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

void initIO();
void readIO();
void set_direction(bool dir);
void set_eco_mode(bool eco);
void writeAccOut(float newAccOut);
void writeRegenBrake(float newRegenBrake);

#endif
