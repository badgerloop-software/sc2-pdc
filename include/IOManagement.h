// combined header file of digitalPin.cpp and analogPins.cpp 
// combined variables / functions given the name hybrid

#ifndef __IO_MANAGER_H__
#define __IO_MANAGER_H__

// from digitalPins.h
#include "ina281.h" 
#include "PID.h" 

// from analogPins.h
#include "adc.h"
#include "const.h"
#include "STM32TimerInterrupt_Generic.h"

// from digitalPins.h
//Outputs
#define MCU_DIR                PA_6
#define MCU_ECO                PB_1
#define MCU_MC_ON              PA_2

//Inputs
#define MCU_SPEED_SIG          PA_8
#define PRK_BRK_TELEM          PB_4

struct Digital_Data {
    bool direction : 1;         // output
    bool mc_speed_sig : 1;      // input
    bool eco_mode : 1;          // output
    bool mcu_mc_on : 1;         // output
    bool park_brake : 1;        // input
};

extern volatile Digital_Data digital_data;

// from analogPins.h
extern volatile float acc_out;
extern volatile float regen_brake;

extern volatile float lv_12V_telem;
extern volatile float lv_5V_telem;
extern volatile float lv_5V_current;
extern volatile float current_in_telem;
extern volatile float brake_pressure_telem;

// initialize digital and analog pins
void initHybrid();

// read digital and analog inputs
void readHybrid();

// from digitalPins.h
// Set the value of output pins
void set_direction(bool dir);
void set_eco_mode(bool eco);
void set_mcu_mc_on(bool mc_on);

// from analogPins.h
void writeAccOut(float newAccOut);

void writeRegenBrake(float newRegenBrake);

#endif

