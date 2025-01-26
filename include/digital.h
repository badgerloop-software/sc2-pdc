#ifndef __DIGITAL_H__
#define __DIGITAL_H__

struct Digital_Data {
    bool direction : 1;         // output
    bool mc_speed_sig : 1;      // input
    bool eco_mode : 1;          // output
    bool mc_on : 1;             // output
    bool park_brake : 1;        // input
};

// read all the digital inputs
void readDigital();

// initialize all the digital pins
void initDigital();

#endif