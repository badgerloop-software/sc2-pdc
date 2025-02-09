#ifndef __DIGITAL_H__
#define __DIGITAL_H__

#include "ina281.h"
#include "PID.h"

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

// read all the digital inputs
void read_digital();

// initialize all the digital pins
void init_digital();

// Set the value of output pins
void set_direction(bool dir);
void set_eco_mode(bool eco);
void set_mcu_mc_on(bool mc_on);

#endif