#include "digitalPins.h"
#include <Arduino.h>

#include "STM32TimerInterrupt_Generic.h"
// #include "const.h"

// Ticker to poll input readings at fixed rate
STM32TimerInterrupt digitalReader(TIM7);

//for PWM
// struct PWM_pin{
//     PinName pwmPin;             
//     uint32_t channel;               // channel to use for PWM
//     HardwareTimer *pwmTimer;        // timer to set PWM output 
// };

// PWM_pin arrayPWM[NUM_ARRAYS] = {
//     {
//         MCU_MC_DRIVER,
//     }
// };

void initDigital(){
    //PWM initialization
    // for (int i = 0; i < NUM_ARRAYS; i++){
    //     TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(arrayPWM[i].pwmPin, PinMap_PWM);
    //     arrayPWM[i].pwmTimer = new HardwareTimer(Instance);
    //     arrayPWM[i].channel = STM_PIN_CHANNEL(pinmap_function(arrayPWM[i].pwmPin, PinMap_PWM));
    //     arrayPWM[i].pwmTimer->setPWM(arrayPWM[i].channel, arrayPWM[i].pwmPin, PWM_FREQ, 0);
    // }


    //Other pin initialization
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