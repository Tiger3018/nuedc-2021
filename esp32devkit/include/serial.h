#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "ble.h"

void debugProcess();
void carProcess();
void openMVProcess();

void serialSetup();

extern BLESerial SerialBL, SerialDE;

#endif /* __SERIAL_H__ */