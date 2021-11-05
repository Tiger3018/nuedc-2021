#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <Arduino.h>
#include <AccelMotor.h>
#include <ESP32Servo.h>

extern AccelMotor motorLeft, motorRight;
extern Servo servoPutter;

// ESP32PWM pwm;
bool motorSetup(void);
void motorLoop(void);

#endif /* __MOTOR_H__ */