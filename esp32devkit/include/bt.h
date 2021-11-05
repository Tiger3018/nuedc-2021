#ifndef __BT_H__
#define __BT_H__

#include <BluetoothSerial.h>
#include <Arduino.h>

extern BluetoothSerial SerialBT;

void BTConfirmRequestCallback(uint32_t numVal);
void BTAuthCompleteCallback(boolean success);

#endif /* __BT_H__ */