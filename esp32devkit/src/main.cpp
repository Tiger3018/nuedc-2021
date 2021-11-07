#include "main.h"
#include "ble.h"
#include "timer.h"
#include "motor.h"
#include "serial.h"
#include "pref.h"
#include "task/task_default.h"

void(* resetFunc) (void) = 0x0; //declare reset function at address 0, trigger watchdog.

void setup() {
  // put your setup code here, to run once:
  pref.begin("default");
  serialSetup();
  taskDefaultMode((modeEnum)pref.getShort("deviceMode", 0));

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  motorSetup();
  bleSetup();
  timerSetup();

  pinMode(THING_PIN, INPUT);
  libTimerStartUp = libTimer.every(100, [](void*) -> bool{
    if(digitalRead(THING_PIN) == HIGH)
    {
      libTimer.cancel(libTimerStartUp);
      taskDefaultMode(M_PID_LINE);
      SerialDE.println("now start!");
    }
    return true;
  });

  log_i("setup complete! (Task: loopTask)");
}

void loop() {
  // put your main code here, to run repeatedly:
  motorLoop();
  taskDefaultLoop(); /* To get diff value */
}

void serialEventRun() {
  openMVLineProcess();
  bleLoop();
  timerLoop();
  return;
}