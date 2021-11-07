#include "motor.h"
#include "pref.h"
#include "gpio.h"
#include "timer.h"
#include "serial.h"
#include "task/task_default.h"
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
    // motorLeft.setSpeed(pref.getShort("motorLeft", 100));
    // motorRight.setSpeed(pref.getShort("motorRight", 100));
    static modeEnum lastMode;
    if(taskDefaultMode() != lastMode)
    {
        lastMode = taskDefaultMode();
        switch (lastMode)
        {
        case M_IDLE:
            motorLeft.setRunMode(IDLE_RUN);
            motorRight.setRunMode(IDLE_RUN);
            break;

        case M_IDLE_STOP:
            motorLeft.setRunMode(IDLE_RUN);
            motorLeft.setMode(STOP);
            motorRight.setRunMode(IDLE_RUN);
            motorRight.setMode(STOP);
            break;

        case M_PID_SPEED:
        case M_PID_LINE:
        case M_BLE_SERIAL:
            motorLeft.setRunMode(PID_SPEED);
            motorRight.setRunMode(PID_SPEED);
            break;
        
        case M_TURN:
            motorLeft.setRunMode(PID_POS);
            motorRight.setRunMode(PID_POS);
            servoPutter.write(taskDefaultStackTop() ? 0 : 180);
            motorLeft.setTarget(motorLeft.getCurrent() + (taskDefaultStackTop() ? 200 : 1300));
            motorRight.setTarget(motorRight.getCurrent() - (taskDefaultStackTop() ? 1300 : 200));
            break;
        
        default:
            break;
        }
    }
    switch (taskDefaultMode())
    {
    case M_PID_LINE:
        servoPutter.write(pref.getShort("servoMid", 90) + commandServoDiff);
        motorLeft.setTargetSpeed(pref.getShort("motorLeft", 800));
        motorRight.setTargetSpeed(- pref.getShort("motorRight", 800));
        break;
    
    case M_TURN:
        break;

    case M_BLE_SERIAL:
        servoPutter.write(pref.getShort("servo", 90));
        motorLeft.setTargetSpeed(pref.getShort("motorLeft", 800));
        motorRight.setTargetSpeed(- pref.getShort("motorRight", 800));
        break;
    
    case M_AROUND:
        break;

    default:
        servoPutter.write(pref.getShort("servo", 90));
        break;
    }
    if(taskDefaultMode() != M_TURN)
    {
        motorLeft.kp = motorRight.kp = pref.getDouble("motorP", 0.09);
        motorLeft.ki = motorRight.ki = pref.getDouble("motorI", 0.300);
        motorLeft.kd = motorRight.kd = pref.getDouble("motorD", 0.001);
    }
    else
    {
        motorLeft.kp = motorRight.kp = 0;
        motorLeft.ki = motorRight.ki = 15;
        motorLeft.kd = motorRight.kd = 0.4;
    }
    motorLeft.tick(encCounter[0]);
    motorRight.tick(encCounter[1]);
    // servoPutter.detach();
    // servoPutter.attach(SERVO_PIN, 1000, 2500);
}