#include "digital.h"
#include <Arduino.h>

#include "STM32TimerInterrupt_Generic.h"
// #include "const.h"

// Ticker to poll input readings at fixed rate
STM32TimerInterrupt digitalReader(TIM7);

void initDigital(){
    pinMode(MCU_DIR, OUTPUT);
    pinMode(MCU_ECO, OUTPUT);
    pinMode(MCU_MC_ON, OUTPUT);

    pinMode(MCU_SPEED_SIG, INPUT);
    pinMode(PRK_BRK_TELEM, INPUT);

    // start the Timer to periodically read IO
    if(digitalReader.attachInterruptInterval(IO_UPDATE_PERIOD, readDigital)) { 
        printf("starting digitalReader timer\n");
    } else {
        printf("problem starting digitalReader timer\n");
    }
}

void readDigital(){
    // read all the digital inputs
    digital_data.mc_speed_sig = digitalRead(MCU_SPEED_SIG);
    digital_data.park_brake = digitalRead(PRK_BRK_TELEM);
}

void setDirection(bool dir){
    digitalWrite(MCU_DIR, dir);
    digital_data.direction = dir;
}

void setEcoMode(bool eco){
    digitalWrite(MCU_ECO, eco);
    digital_data.eco_mode = eco;
}

void setMCOn(bool mc_on){
    digitalWrite(MCU_MC_ON, mc_on);
    digital_data.mc_on = mc_on;
}