#include "main.h"
#include "bt.h"
#include "timer.h"
#include "motor.h"
// #include <Servo.h>
// #include <analogWrite.h>
#include <BluetoothSerial.h>


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  motorSetup();


  SerialBT.enableSSP();
  SerialBT.onConfirmRequest(BTConfirmRequestCallback);
  SerialBT.onAuthComplete(BTAuthCompleteCallback);
  // SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  // motorRight.setRunMode(IDLE_RUN);
  log_i("setup complete! (Task: loopTask)");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH);
  // delay(1000);//millis();
  // motorLeft.tick(encTick(MOTOR_LIN1_PIN));
  // Serial.printf("now statistics: %ld %d\n", motorLeft.getCurrent(), motorLeft.getSpeed());
  // digitalWrite(LED_PIN, LOW);
  // delay(1000);//millis();
  // delay(1000);
  // Serial.println(motorLeft.getCurrent(), motorLeft.getSpeed());
  // motorLeft.tick()
}

void serialEventRun() {
  return;
}