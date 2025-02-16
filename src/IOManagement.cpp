// combined source file of digitalPin.cpp and analogPins.cpp 
// combined variables / functions given the name hybrid

#include "IOManagement.h"

// from digitalPins.cpp
#include "const.h"
#include <Arduino.h>
#include "STM32TimerInterrupt_Generic.h"

volatile Digital_Data digital_data;

// from analogPins.cpp
volatile float acc_out = 0;
volatile float regen_brake = 0;
volatile float lv_12V_telem = 0;
volatile float lv_5V_telem = 0;
volatile float lv_5V_current = 0;
volatile float current_in_telem = 0;
volatile float brake_pressure_telem = 0;

// combined timer of digitalReader() and dataUpdater()
// Ticker to poll input readings at fixed rate
STM32TimerInterrupt hybridTimer(TIM7); // used TIM7 from digital, analog used TIM2

// combined function of init_digital() and initAnalog()
void initHybrid() {
    // from init_digital()
    // other pin initialization
    pinMode(MCU_DIR, OUTPUT);
    pinMode(MCU_ECO, OUTPUT);
    pinMode(MCU_MC_ON, OUTPUT);
    pinMode(MCU_SPEED_SIG, INPUT);
    pinMode(PRK_BRK_TELEM, INPUT);

    // from initAnalog() 
    initADC(ADC1);

    if(hybridTimer.attachInterruptInterval(IO_UPDATE_PERIOD, readHybrid))
    {
        printf("starting hybrid timer\n");
    }
    else 
    {
        printf("problem starting hybrid timer\n");
    }
}

// combined function of read_digital() and readAnalog()
void readHybrid() 
{
    // from read_digital()
    // read all the digital inputs
    digital_data.mc_speed_sig = digitalRead(MCU_SPEED_SIG);
    digital_data.park_brake = digitalRead(PRK_BRK_TELEM);

    // from readAnalog()
    lv_12V_telem = readADC(ADC_CHANNEL_6) * 3.3 *  35.1/5.1; // PA_1
    lv_5V_telem = readADC(ADC_CHANNEL_12) * 3.3 * 15.1/5.1; // PA_7
    lv_5V_current = readADC(ADC_CHANNEL_15) * INA180_CURRENT_MULTIPLIER; // PB_0
    current_in_telem = readADC(ADC_CHANNEL_8) * INA180_CURRENT_MULTIPLIER; // PA_3
    brake_pressure_telem = readADC(ADC_CHANNEL_5); // PA_0
}

// from digitalPins.cpp
void set_direction(bool dir){
    digitalWrite(MCU_DIR, dir);
    digital_data.direction = dir;
}

// from digitalPins.cpp
void set_eco_mode(bool eco){
    digitalWrite(MCU_ECO, eco);
    digital_data.eco_mode = eco;
}

// from digitalPins.cpp
void set_mcu_mc_on(bool mc_on){
    digital_data.mcu_mc_on = mc_on;
    digitalWrite(MCU_MC_ON, mc_on);
}

// from analogPins.cpp
void writeAccOut(float newAccOut) {
    acc_out = newAccOut;
    analogWrite(PA_5, acc_out);
}

// from analogPins.cpp 
void writeRegenBrake(float newRegenBrake) {
    regen_brake = newRegenBrake;
    analogWrite(PA_4, regen_brake);
}
