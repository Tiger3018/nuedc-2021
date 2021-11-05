#include "motor.h"
#include "main.h"
#include "gpio.h"
#include "timer.h"
#include <AccelMotor.h>
#include <ESP32Servo.h>

// AccelMotor motorBase(DRIVER3WIRE);
AccelMotor motorLeft(DRIVER3WIRE, MOTOR_L1_PIN, MOTOR_L2_PIN, MOTOR_LPWM_PIN, HIGH);
AccelMotor motorRight(DRIVER3WIRE, MOTOR_R1_PIN, MOTOR_R2_PIN, MOTOR_RPWM_PIN, HIGH);

// ESP32PWM pwm;
Servo servoPutter;

static int64_t _;

bool motorSetup()
{
    // For ESP32Servo and ESP32PWM in lib ESP32Servo. See ESP32PWM::allocatenext
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    // ESP32PWM::allocateTimer(2);
    // ESP32PWM::allocateTimer(3);

    pinMode(MOTOR_LIN1_PIN, INPUT);
    pinMode(MOTOR_LIN2_PIN, INPUT);
    pinMode(MOTOR_RIN1_PIN, INPUT);
    pinMode(MOTOR_RIN2_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(MOTOR_LIN1_PIN), encoderCallbackL, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOTOR_LIN2_PIN), encoderCallbackL, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOTOR_LIN1_PIN), encoderCallbackR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOTOR_LIN2_PIN), encoderCallbackR, CHANGE);
    motorLeft.setRatio(30 * 70); /* speed reduction ratio = 1:30, */
    motorRight.setRatio(30 * 70); /* speed reduction ratio = 1:30, */
    // motorBase.setResolution(); /* 8 is default */
    motorLeft.setRunMode(IDLE_RUN);
    // motorLeft.run(FORWARD, 2000); /* private */
    motorLeft.setRunMode(PID_SPEED);
    motorLeft.setMode(AUTO);
    motorRight.setMode(AUTO);

    servoPutter.setPeriodHertz(300);
    if(_ = !servoPutter.attach(SERVO_PIN, 1000, 2500))
    {
        log_e("Fail to allocate timer for servo.");
    }
    else
    {
        log_i("servoPutter is attached to %lld", _);
    }
    servoPutter.write(90);
    return true;
}

void motorLoop()
{
    motorLeft.setSpeed(pref.getShort("speedLeft", 100));
    // motorLeft.tick();
    motorRight.setSpeed(pref.getShort("speedRight", 100));
    // servoPutter.detach();
    // delay(100);
    // servoPutter.attach(SERVO_PIN, pref.getShort("servoMin", 1000), pref.getShort("servoMax", 2500));
    // delay(100);
    servoPutter.write(pref.getShort("servo", 90));
}