#include "motor.h"
#include "gpio.h"
#include "timer.h"
#include <AccelMotor.h>
#include <ESP32Servo.h>

// AccelMotor motorBase(DRIVER3WIRE);
AccelMotor motorLeft(DRIVER3WIRE, MOTOR_L1_PIN, MOTOR_L2_PIN, MOTOR_LPWM_PIN, HIGH);
AccelMotor motorRight(DRIVER3WIRE, MOTOR_R1_PIN, MOTOR_R2_PIN, MOTOR_RPWM_PIN, HIGH);

bool motorSetup()
{
  pinMode(MOTOR_LIN1_PIN, INPUT);
  pinMode(MOTOR_LIN2_PIN, INPUT);
  pinMode(MOTOR_RIN1_PIN, INPUT);
  pinMode(MOTOR_RIN2_PIN, INPUT);
  attachInterrupt(MOTOR_LIN1_PIN, encoderCallback, CHANGE);
  attachInterrupt(MOTOR_LIN2_PIN, encoderCallback, CHANGE);
  motorLeft.setRatio(30 * 70); /* speed reduction ratio = 1:30, */
  motorRight.setRatio(30 * 70); /* speed reduction ratio = 1:30, */
  // motorBase.setResolution(); /* 8 is default */
  motorLeft.setRunMode(IDLE_RUN);
  // motorLeft.run(FORWARD, 2000); /* private */
  motorLeft.setRunMode(PID_SPEED);
  motorLeft.setSpeed(300);
  return true;
}
