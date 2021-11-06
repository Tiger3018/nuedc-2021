#ifndef __TIMER_H__
#define __TIMER_H__

/** @brief for Left motor Channel A rising edge, if Channel B is HIGH, encCount--; \n
 *         for Left motor Channel A trailing edge, if Channel B is HIGH, encCount ++;
 */
void encCallBackLA();
void encCallBackLB();
void encCallBackRA();
void encCallBackRB();
void encoderInit(int a, int b, int c, int d);
inline void encoderUp(char pos, volatile long* encCount);
inline void encoderDown(char pos, volatile long* encCount);

void timerSetup();
void timerLoop();

extern volatile long encCounter[2];

#endif /* __TIMER_H__ */