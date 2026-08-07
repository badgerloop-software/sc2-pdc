// speed_calc: MCU_SPEED_SIG pulses -> rpm and mph
// Rising-edge ISR counts pulses - Timer TIM1 runs calculateSpeed every 50 ms
// Rolling window of ARRAY_SIZE samples smooths rpm
#include "speed_calc.h"
#include "IOManagement.h"

volatile uint8_t speedPulses = 0;
static volatile uint8_t previousPulses[ARRAY_SIZE];

STM32TimerInterrupt speedCalcTimer(TIM1);

void increment() {
    speedPulses++;
}

void calculateSpeed(){
    // Sum pulses over ARRAY_SIZE intervals, convert to rpm, then to mph
    static uint8_t calculationCounter = 0;
    static uint16_t runningSum = 0;

    calculationCounter = (calculationCounter + 1) % ARRAY_SIZE;
    runningSum = runningSum - previousPulses[calculationCounter] + speedPulses;
    previousPulses[calculationCounter] = speedPulses;
    speedPulses = 0;

    float revolutions = runningSum / (float)PULSES_PER_REV;
    rpm = revolutions / (ARRAY_SIZE * SPEED_CALC_INTERVAL_US / 1000000.0 / 60.0 );
    // inches per rev * rpm * (60 min/h) / (12 in/ft) / (5280 ft/mi)
    mph = rpm * WHEEL_CIRCUMFERENCE * 0.00094696;
}

void startSpeedCalculation() {
    attachInterrupt(digitalPinToInterrupt(MCU_SPEED_SIG), increment, RISING);
    speedCalcTimer.attachInterruptInterval(SPEED_CALC_INTERVAL_US, calculateSpeed);
}
