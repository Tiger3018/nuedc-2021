#include "main.h"
#include "ble.h"
#include "timer.h"
#include "motor.h"
// #include <Servo.h>
// #include <analogWrite.h>
// #include <BluetoothSerial.h>

Preferences pref;

void(* resetFunc) (void) = 0x0; //declare reset function at address 0, trigger watchdog.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  motorSetup();
  pref.begin("default");
  bleSetup();

  timerSetup();
  log_i("setup complete! (Task: loopTask)");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH);
  bleLoop();
  timerLoop();
  motorLoop();
}

void serialEventRun() {
  return;
}