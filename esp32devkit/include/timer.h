#ifndef __TIMER_H__
#define __TIMER_H__

void encoderCallbackL();
void encoderCallbackR();
void timerSetup();
void timerLoop();

extern volatile long encCounter[2];

#endif /* __TIMER_H__ */