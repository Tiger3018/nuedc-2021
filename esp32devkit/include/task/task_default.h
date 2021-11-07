#ifndef __TASK_DEFAULT_H__
#define __TASK_DEFAULT_H__

#include <stdint.h>

// ProtectFlag[0] prevent other turn commands, ProtectFlag[1] prevent first handle process.
extern short commandServoDiff, commandNumber, commandTurnProtectFlag[2];

enum modeEnum{
    M_IDLE = 0,
    M_IDLE_STOP,
    M_PID_SPEED,
    M_PID_LINE,
    M_TURN,
    M_BLE_SERIAL,
    M_FORMAL_WAIT,
    M_AROUND
};

class modeClass
{
    public:
    modeEnum mode = M_IDLE;
    bool turn[3];
    int8_t tot = 0;
};

void taskDefaultLoop();
void taskDefaultStack(bool turnVal);
bool taskDefaultStack();
bool taskDefaultStackTop();
modeEnum taskDefaultMode();
modeEnum taskDefaultMode(modeEnum inputMode);
bool taskDefaultTriggerTurn(void*);

#endif /* __TASK_DEFAULT_H__ */