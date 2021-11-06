#include "main.h"
#include "ble.h"
#include "timer.h"
#include "motor.h"
#include "serial.h"

Preferences pref;

void(* resetFunc) (void) = 0x0; //declare reset function at address 0, trigger watchdog.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pref.begin("default");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  motorSetup();
  bleSetup();
  timerSetup();
  serialSetup();
  log_i("setup complete! (Task: loopTask)");
}

void loop() {
  // put your main code here, to run repeatedly:
  bleLoop();
  timerLoop();
  motorLoop();
}

void serialEventRun() {
  openMVProcess();
  return;
}