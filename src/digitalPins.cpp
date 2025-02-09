#include "digitalPins.h"
#include "const.h"
#include <Arduino.h>

#include "STM32TimerInterrupt_Generic.h"

// Ticker to poll input readings at fixed rate
STM32TimerInterrupt digitalReader(TIM7);
volatile Digital_Data digital_data;

void init_digital(){
    //Other pin initialization
    pinMode(MCU_DIR, OUTPUT);
    pinMode(MCU_ECO, OUTPUT);
    pinMode(MCU_MC_ON, OUTPUT);

    pinMode(MCU_SPEED_SIG, INPUT);
    pinMode(PRK_BRK_TELEM, INPUT);

    // start the Timer to periodically read IO
    if(digitalReader.attachInterruptInterval(IO_UPDATE_PERIOD, read_digital)) { 
        printf("starting digitalReader timer\n");
    } else {
        printf("problem starting digitalReader timer\n");
    }
}

void read_digital(){
    // read all the digital inputs
    digital_data.mc_speed_sig = digitalRead(MCU_SPEED_SIG);
    digital_data.park_brake = digitalRead(PRK_BRK_TELEM);
}

void set_direction(bool dir){
    digitalWrite(MCU_DIR, dir);
    digital_data.direction = dir;
}

void set_eco_mode(bool eco){
    digitalWrite(MCU_ECO, eco);
    digital_data.eco_mode = eco;
}

void set_mcu_mc_on(bool mc_on){
    digital_data.mcu_mc_on = mc_on;
    digitalWrite(MCU_MC_ON, mc_on);
}