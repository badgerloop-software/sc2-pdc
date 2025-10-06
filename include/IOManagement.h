#ifndef __IO_MANAGER_H__
#define __IO_MANAGER_H__

#include "PID.h" 
#include "dac.h"
#include "adc.h"
#include "const.h"
#include "STM32TimerInterrupt_Generic.h"
#include <Arduino.h>

//Outputs
#define MCU_DIR                PB7
#define MCU_ECO                PB1

//Inputs
#define MC_ON                  PA10
#define PRK_BRK_TELEM          PB4

struct Digital_Data {
    bool direction : 1;         // output
    bool eco_mode : 1;          // output
    bool mc_on : 1;             // input
    bool park_brake : 1;        // input
    bool brakeLED : 1;          // derived by state machine
};

extern volatile Digital_Data digital_data;

extern volatile float acc_in;
extern volatile float acc_out;
extern volatile float regen_brake;
extern volatile float lv_12V_telem;
extern volatile float lv_5V_telem;
extern volatile float lv_5V_current;
extern volatile float current_in_telem;
extern volatile float brake_pressure_telem;

// initialize digital and analog pins
void initIO();

// read digital and analog inputs
void readIO();

// Set the value of output pins
void set_direction(bool dir);
void set_eco_mode(bool eco);
void writeAccOut(float newAccOut);
void writeRegenBrake(float newRegenBrake);

#endif

