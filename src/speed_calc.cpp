#include "speed_calc.h"

static volatile int speedPulses = 0;
volatile float rpm = 0;
volatile float mph = 0;

static unsigned int previousPulses[ARRAY_SIZE];

STM32TimerInterrupt intervalTimer(TIM1); 

void increment() {
    speedPulses++;
}

void calculateSpeed(){
    // Variable to track array update position
    static int calculationCounter = 0;
    static unsigned int runningSum = 0; 

    // Replaces oldest pulse in pulse counter vec with a new one
    calculationCounter = (calculationCounter + 1) % ARRAY_SIZE;
    runningSum = runningSum - previousPulses[calculationCounter] + speedPulses;
    previousPulses[calculationCounter] = speedPulses;
    speedPulses = 0;

    // Calculate rpm and mph based on total pulses in previousPulses vector
    float revolutions = runningSum / (float)PULSES_PER_REV;
    rpm = revolutions / (INTERVAL_MS * ARRAY_SIZE / 60000.0);
    mph = rpm * WHEEL_CIRCUMFERENCE * 0.00094696;
}

void startSpeedCalculations() {
    // Increments speedPulses 1 for every pulse detected
    attachInterrupt(digitalPinToInterrupt(MCU_SPEED_SIG), increment, RISING); 

    // Calculates speed every 50 ms/50000 us
    intervalTimer.attachInterruptInterval(INTERVAL_MS * 1000, calculateSpeed);
}

