#include "speed_calc.h"

#include "io_management.h"

// ------------- GLOBALS -------------

volatile uint8_t previousPulses[ARRAY_SIZE];

// ------------- LOCAL -------------

static volatile uint8_t speedPulses = 0;

static STM32TimerInterrupt speedCalcTimer(TIM1);

// ------------- LOCAL FUNCTIONS -------------

static void increment() {
    speedPulses++;
}

static void calculateSpeed() {
    static uint8_t calculationCounter = 0;
    static uint16_t runningSum = 0;

    calculationCounter = (calculationCounter + 1) % ARRAY_SIZE;
    runningSum = runningSum - previousPulses[calculationCounter] + speedPulses;
    previousPulses[calculationCounter] = speedPulses;
    speedPulses = 0;

    float revolutions = runningSum / (float)PULSES_PER_REV;
    rpm = revolutions /
          (ARRAY_SIZE * SPEED_CALC_INTERVAL_US / 1000000.0 / 60.0);
    mph = rpm * WHEEL_CIRCUMFERENCE * 0.00094696;
}

// ------------- PUBLIC FUNCTIONS -------------

void startSpeedCalculation() {
    attachInterrupt(digitalPinToInterrupt(MCU_SPEED_SIG), increment, RISING);
    speedCalcTimer.attachInterruptInterval(SPEED_CALC_INTERVAL_US, calculateSpeed);
}
