#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "ble.h"

void debugProcess();
void carProcess();
void openMVProcess();
void openMVLineProcess();

void serialSetup();

extern BLESerial SerialBL, SerialDE;

#endif /* __SERIAL_H__ */