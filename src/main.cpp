#include <Arduino.h>

#define PWM_PIN PA_8
#define STEP_SIZE 10

HardwareTimer *tim;
uint32_t channel;

uint32_t dutyCycle;
int simRPM;
char buf[1];

void setup(){
    //Serial related
    Serial.begin(115200);

    // Automatically retrieve TIM instance and channel associated to pin
    // This is used to be compatible with all STM32 series automatically.
    TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(PWM_PIN, PinMap_PWM);
    tim = new HardwareTimer(Instance);

    channel = STM_PIN_CHANNEL(pinmap_function(PWM_PIN, PinMap_PWM));
    // Fixed 10% duty cycle, the speed depends on frequency
    dutyCycle = 10;

    simRPM = 0;
    Serial.write("Starting\n");
}

void loop()
{

    delay(1000);
    // printf("\e[1;1H\e[2J");
    Serial.printf("Simulating RPM: %i    \n", simRPM);
    Serial.printf("Press i to increment, d to decrement\n");

    if (Serial.available() > 0) {
        buf[0] = Serial.read();
        if (buf[0] == 'i') {
            simRPM += STEP_SIZE;
        } else if (buf[0] == 'd') {
            simRPM -= STEP_SIZE;
        }

        if (simRPM < 0) simRPM = 0;
    }

    // Divide by 60 to get rps, multiply by 48 to get ticks per second
    // Period in ms is 1000 / ticks per second
    // Roughly equivalent to 1333.33333 / rpm

    float periodMS;
    if (simRPM > 0) {
        periodMS = 1333.333333 / simRPM;
        tim->setPWM(channel, PWM_PIN, 1000/periodMS, dutyCycle);
    } else {
        tim->setPWM(channel, PWM_PIN, 1, 0);
    }
    
    Serial.printf("Current frequency: %f Hz\n", 1000/periodMS);
}