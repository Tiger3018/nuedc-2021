#include "main.h"
#include "timer.h"
#include <AccelMotor.h>

static long encCounter[2] = {0};

long encTick(byte pin) {
  static bool lastState;
  static long encCounter = 0;
  bool curState = digitalRead(pin);       // опрос
  if (lastState != curState) {            // словили изменение
    lastState = curState;
    if (curState) {                       // по фронту
    //   encCounter += motor.getState();     // запомнили поворот
    }
  }
  return encCounter;
}

void encoderRead() {
    motorLeft.tick(encCounter[0]);
    motorRight.tick(encCounter[1]);
}

void encoderCallback() {
    Serial.printf("got it!");
}