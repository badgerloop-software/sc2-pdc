#include "speed_calc.h"

volatile uint8_t speedPulses = 0;
volatile float rpm = 0;
volatile float mph = 0;

volatile uint8_t previousPulses[ARRAY_SIZE];

STM32TimerInterrupt speedCalcTimer(TIM1);

void increment() {
    speedPulses++;
}

void calculateSpeed(){
    // Variable to track array update position
    static uint8_t calculationCounter = 0;
    static uint16_t runningSum = 0; 

    // Replaces oldest pulse in pulse counter array with a new one
    calculationCounter = (calculationCounter + 1) % ARRAY_SIZE;
    runningSum = runningSum - previousPulses[calculationCounter] + speedPulses;
    previousPulses[calculationCounter] = speedPulses;
    speedPulses = 0;

    // Calculate rpm and mph based on total pulses in previousPulses array
    float revolutions = runningSum / (float)PULSES_PER_REV;
    rpm = revolutions / (ARRAY_SIZE * SPEED_CALC_INTERVAL_US / 1000000.0 / 60.0 ); // revs / time to gather ARRAY_SIZE revs
    mph = rpm * WHEEL_CIRCUMFERENCE * 0.00094696; // (1 / 12 / 5280) * 60

}

void startSpeedCalculation() {
    // Increments speedPulses 1 for every pulse detected
    attachInterrupt(digitalPinToInterrupt(MCU_SPEED_SIG), increment, RISING); 

    // Calculates speed every 50 ms/50000 us
    speedCalcTimer.attachInterruptInterval(SPEED_CALC_INTERVAL_US, calculateSpeed);
}   

