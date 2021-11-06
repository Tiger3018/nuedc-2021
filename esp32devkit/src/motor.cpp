#include "motor.h"
#include "pref.h"
#include "gpio.h"
#include "timer.h"
#include <AccelMotor.h>
#include <ESP32Servo.h>

// AccelMotor motorBase(DRIVER3WIRE);
AccelMotor motorLeft(DRIVER3WIRE, MOTOR_L1_PIN, MOTOR_L2_PIN, MOTOR_LPWM_PIN, HIGH); // speed = speed
AccelMotor motorRight(DRIVER3WIRE, MOTOR_R1_PIN, MOTOR_R2_PIN, MOTOR_RPWM_PIN, HIGH); // speed = -speed

// ESP32PWM pwm;
Servo servoPutter;

static int64_t _;

bool motorSetup()
{
    // For ESP32Servo and ESP32PWM in lib ESP32Servo. See ESP32PWM::allocatenext
    // ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    // ESP32PWM::allocateTimer(3);
    // pref.putShort("servoMin", 1000); pref.putShort("servoMax", 2500);
    if(_ = !servoPutter.attach(SERVO_PIN, 1000, 2500))
    {
        log_e("Fail to allocate timer for servo.");
    }
    else
    {
        log_i("servoPutter is attached to %lld", _);
    }
    servoPutter.setPeriodHertz(300);
    servoPutter.detach();
    servoPutter.attach(SERVO_PIN, 1000, 2500);
    // servoPutter.write(90);

    pinMode(MOTOR_LIN1_PIN, INPUT_PULLUP);
    pinMode(MOTOR_LIN2_PIN, INPUT_PULLUP);
    pinMode(MOTOR_RIN1_PIN, INPUT_PULLUP);
    pinMode(MOTOR_RIN2_PIN, INPUT_PULLUP);
    encoderInit(
        digitalRead(MOTOR_LIN1_PIN), digitalRead(MOTOR_LIN2_PIN),
        digitalRead(MOTOR_RIN1_PIN), digitalRead(MOTOR_RIN2_PIN)
        );
    attachInterrupt(digitalPinToInterrupt(MOTOR_LIN1_PIN), encCallBackLA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOTOR_LIN2_PIN), encCallBackLB, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOTOR_RIN1_PIN), encCallBackRA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MOTOR_RIN2_PIN), encCallBackRB, CHANGE);

    motorLeft.setRatio(30 * 70); /* speed reduction ratio = 1:30, */
    motorRight.setRatio(30 * 70); /* speed reduction ratio = 1:30, */
    // motorBase.setResolution(); /* 8 is default */
    motorLeft.setRunMode(IDLE_RUN);
    // motorLeft.run(FORWARD, 2000); /* private */
    motorLeft.setRunMode(PID_SPEED);
    motorRight.setRunMode(PID_SPEED);
    // motorLeft.setMode(AUTO);
    // motorRight.setMode(AUTO);

    return true;
}

void motorLoop()
{
    // motorLeft.setSpeed(pref.getShort("speedLeft", 100));
    // motorRight.setSpeed(pref.getShort("speedRight", 100));
    servoPutter.write(pref.getShort("servo", 90));
    motorLeft.setTargetSpeed(pref.getShort("speedLeft", 100));
    motorRight.setTargetSpeed(pref.getShort("speedRight", 100));
    motorLeft.kp = motorRight.kp = pref.getDouble("motorP", 0.06);
    motorLeft.ki = motorRight.ki = pref.getDouble("motorI", 0.055);
    motorLeft.kd = motorRight.kd = pref.getDouble("motorD", 0.01);
    motorLeft.tick(encCounter[0]);
    motorRight.tick(encCounter[1]);
    // servoPutter.detach();
    // servoPutter.attach(SERVO_PIN, 1000, 2500);
}