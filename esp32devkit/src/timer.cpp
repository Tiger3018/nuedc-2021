#include "gpio.h"
#include "timer.h"
#include <Arduino.h>
#include <AccelMotor.h>

volatile long encCounter[2] = {0};

static unsigned long regTimer;

void encoderCallbackL() {
    if(digitalRead(MOTOR_LIN1_PIN) != digitalRead(MOTOR_LIN2_PIN))
    {
        ++ encCounter[0];
    }
    else
    {
        -- encCounter[0];
    }
}

void encoderCallbackR() {
    if(digitalRead(MOTOR_RIN1_PIN) != digitalRead(MOTOR_RIN2_PIN))
    {
        ++ encCounter[1];
    }
    else
    {
        -- encCounter[1];
    }
}

void timerSetup()
{
    regTimer = millis();
}

void timerLoop()
{
    if(millis() - regTimer > 1000)
    {
        regTimer = millis();
        Serial.printf("Counter: %ld %ld\n", encCounter[0], encCounter[1]);
    }
}