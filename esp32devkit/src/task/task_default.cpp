#include "task/task_default.h"
#include "pref.h"
#include "motor.h"
#include "timer.h"
#include "gpio.h"
#include "serial.h"

#include <Arduino.h>

static modeClass modeObject;
short commandServoDiff = 0, commandNumber, commandTurnProtectFlag[2] = {0};

void taskDefaultLoop()
{
    switch (taskDefaultMode())
    {
    case M_TURN:
        if(!commandTurnProtectFlag[1])
        {
            commandTurnProtectFlag[1] = 1;
        }
        else if(abs(motorLeft.getCurrent() - motorLeft.getTarget()) < 20 &&
           abs(motorRight.getCurrent() - motorRight.getTarget()) < 20)
        {
            delay(100);
            taskDefaultMode(M_IDLE_STOP);
            commandTurnProtectFlag[1] = 0;
            libTimer.in(1000, [](void*) -> bool{commandServoDiff = 0; return taskDefaultMode(M_PID_LINE), true;});
        }
        // if(!regTimer[1])
        //     regTimer[1] = millis();
        // else if(millis() - regTimer[1] > 1000);
        break;

    case M_AROUND:
        break;

    default:
        break;
    }
}

void taskDefaultStack(bool turnVal)
{
    modeObject.turn[modeObject.tot ++] = turnVal;
}
bool taskDefaultStack()
{
    if(!modeObject.tot) {log_e("error!"); return false;}
    return modeObject.turn[-- modeObject.tot];
}
bool taskDefaultStackTop()
{
    return modeObject.turn[modeObject.tot - 1];
}

modeEnum taskDefaultMode()
{
    return modeObject.mode;
}
modeEnum taskDefaultMode(modeEnum inputMode)
{
    return modeObject.mode = inputMode;
}

bool taskDefaultTriggerTurn(void*)
{
    if((digitalRead(PNP1_PIN) & digitalRead(PNP2_PIN) & digitalRead(PNP3_PIN) &
    digitalRead(PNP4_PIN)))
    {
        SerialDE.printf("Trigger!\n");
        taskDefaultMode(M_TURN);
        commandTurnProtectFlag[0] = 0;
        libTimer.cancel(libTimerTurn);
    }
    return true;
}
