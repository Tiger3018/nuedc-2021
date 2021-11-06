#ifndef __MAIN_H__
#define __MAIN_H__

#include <Arduino.h>
#include <ESP32Servo.h>
#include "gpio.h"

extern void(* resetFunc) (void); //declare reset function at address 0, trigger watchdog.

#endif /* __MAIN__H__ */