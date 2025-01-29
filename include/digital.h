#ifndef __DIGITAL_H__
#define __DIGITAL_H__

#include "STM32TimerInterrupt_Generic.h"

//Define macros for the digital pins

//Outputs
#define MCU_DIR             PA_6
#define MCU_ECO             PB_1
#define MCU_MC_ON           PA_2

//Inputs
#define MCU_SPEED_SIG       PA_8
#define PRK_BRK_TELEM       PB_4

#define IO_UPDATE_PERIOD    1000000 // 1 second

struct Digital_Data {
    bool direction : 1;         // output
    bool mc_speed_sig : 1;      // input
    bool eco_mode : 1;          // output
    bool mc_on : 1;             // output
    bool park_brake : 1;        // input
};

extern volatile Digital_Data digital_data;

// read all the digital inputs
void readDigital();

// initialize all the digital pins
void initDigital();

// Set the value of output pins
void setDirection(bool dir);
void setEcoMode(bool eco);
void setMCOn(bool mc_on);

#endif