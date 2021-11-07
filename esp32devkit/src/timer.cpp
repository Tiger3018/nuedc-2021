#include "gpio.h"
#include "timer.h"
#include "motor.h"
#include <Arduino.h>
#include <AccelMotor.h>

#include <bitset>

using std :: bitset;

volatile long encCounter[2] = {0};
volatile char pin = 0;

unsigned long regTimer[10] = {0};
Timer<10> libTimer;
uintptr_t libTimerTurn, libTimerStartUp;

void timerSetup()
{
    regTimer[0] = millis();
}

void timerLoop()
{
    libTimer.tick();
    if(millis() - regTimer[0] > 1000)
    {
        regTimer[0] = millis();
        // Serial.printf("Counter: %ld %ld\n", encCounter[0], encCounter[1]);
        // Serial.printf("Speed: %d %ld\n", motorLeft.getSpeed(), motorRight.getSpeed());
    }
}

void encCallBackLAR() { pin |= 0x1; encoderDown(0x2, &encCounter[0]);}
void encCallBackLAT() { pin &= 0x0; encoderUp(0x2, &encCounter[0]);}
void encCallBackLBR() { pin |= 0x2; encoderUp(0x1, &encCounter[0]);}
void encCallBackLBT() { pin &= 0x0; encoderDown(0x1, &encCounter[0]);}
void encCallBackRAR() { pin |= 0x4; encoderDown(0x8, &encCounter[1]);}
void encCallBackRAT() { pin &= 0x0; encoderUp(0x8, &encCounter[1]);}
void encCallBackRBR() { pin |= 0x8; encoderUp(0x4, &encCounter[1]);}
void encCallBackRBT() { pin &= 0x0; encoderDown(0x4, &encCounter[1]);}
void encCallBackLA() { (digitalRead(MOTOR_LIN1_PIN)) ? encCallBackLAR() : encCallBackLAT(); }
void encCallBackLB() { (digitalRead(MOTOR_LIN2_PIN)) ? encCallBackLBR() : encCallBackLBT(); }
void encCallBackRA() { (digitalRead(MOTOR_RIN1_PIN)) ? encCallBackRAR() : encCallBackRAT(); }
void encCallBackRB() { (digitalRead(MOTOR_RIN2_PIN)) ? encCallBackRBR() : encCallBackRBT(); }

void encoderInit(int a, int b, int c, int d)
{
    pin = (a << 0) | (b << 1) | (c << 2) | (d << 3);
}
inline void encoderUp(char pos, volatile long* encCount)
{
    *encCount  = (pin & pos) ? -(~(*encCount)) : ~(-(*encCount));
}
inline void encoderDown(char pos, volatile long* encCount)
{
    *encCount += (pin & pos) ? -1 : 1;
}