#include "serial.h"
#include "pref.h"
#include "main.h"
#include "motor.h"
#include "ble.h"
#include "timer.h"
#include "task/task_default.h"

#include <string>
#include <sstream>

#define LINE_BUF_SIZE 60

void carProcess()
{
    return;
}

void debugProcess()
{
    static int8_t charBuf = 0;
    static char lineBuf[LINE_BUF_SIZE] = {0}, tot = 0;
    static std :: stringstream streamBuf;
    short tempM, tempL, tempR;
    double tempP, tempI, tempD;
    while((charBuf = SerialDE.read()) != -1)
    {
        if(charBuf == '\n')
        {
            lineBuf[tot ++] = charBuf;
            tot = 0;
            streamBuf.clear();
            streamBuf << lineBuf;
            memset(lineBuf, 0, sizeof(lineBuf));
            streamBuf >> lineBuf;
            Serial.printf("Receive DEBUG BLE terminal command \'%s\'\n", lineBuf);
            if(!strcmp(lineBuf, "MODE"))
            {
                if(streamBuf >> tempL)
                {
                    pref.putShort("deviceMode", tempL);
                    taskDefaultMode((modeEnum)tempL);
                }
                SerialDE.printf("deviceMode: %i\n", EEPR_GS("deviceMode"));
            }
            else if(*lineBuf == 'M')
            {
                tempL = 100; tempR = 100;
                if(streamBuf >> tempL)
                {
                    pref.putShort("motorLeft", tempL);
                }
                if(streamBuf >> tempR)
                {
                    pref.putShort("motorRight", tempR);
                }
                SerialDE.printf("Motor: %i %i\n", EEPR_GS("motorLeft"), EEPR_GS("motorRight"));
            }
            else if(*lineBuf == 'S')
            {
                if(*(lineBuf + 1) == 'M')
                {
                    if(streamBuf >> tempM)
                    {
                        pref.putShort("servoMid", tempM);
                    }
                }
                else if(streamBuf >> tempM)
                {
                    pref.putShort("servo", tempM);
                }
                SerialDE.printf("Servo: %i at %i |can't change:<%i %i>\n", EEPR_GS("servo"), EEPR_GS("servoMid"), EEPR_GS("servoMin"), EEPR_GS("servoMax"));
            }
            else if(*lineBuf == 'R')
            {
                // pref.remove("deviceMode");
                pref.clear();
                SerialDE.println("Request done. Resetting!");
                resetFunc();
            }
            else if(*lineBuf == 'C')
            {
                if(streamBuf >> lineBuf)
                {
                    pref.putString("bleName", lineBuf);
                    SerialDE.printf("Your name already changed! Reset now.\n");
                }
                else
                {
                    pref.remove("bleName");
                    SerialDE.printf("Your name already reset to default! Reset now.\n");
                }
                resetFunc();
            }
            else if(*lineBuf == 'P')
            {
                if(streamBuf >> tempP)
                {
                    pref.putDouble("motorP", tempP);
                }
                if(streamBuf >> tempI)
                {
                    pref.putDouble("motorI", tempI);
                }
                if(streamBuf >> tempD)
                {
                    pref.putDouble("motorD", tempD);
                }
                SerialDE.printf("PID: %5.3f %5.3f %5.3f\n", EEPR_GD("motorP"), EEPR_GD("motorI"), EEPR_GD("motorD"));
            }
            else if(*lineBuf == 'B')
            {
                pref.putBool("bleMode", !pref.getBool("bleMode", 0));
                SerialDE.println("Request (change BLE mode) done. Resetting!");
                resetFunc();
            }
        }
        else
        {
            lineBuf[tot ++] = charBuf;
            if(tot >= 20) tot = 0;
        }
    }
    return;
}


void openMVLineProcess()
{
    static int8_t charBuf = 0;
    static char lineBuf[LINE_BUF_SIZE] = {0}, tot = 0;
    static std :: stringstream streamBuf;
    short temp, tempL;
    while((charBuf = Serial2.read()) != -1)
    {
        if(charBuf == '\n')
        {
            lineBuf[tot ++] = charBuf;
            tot = 0;
            streamBuf.clear();
            streamBuf << lineBuf;
            // SerialDE.printf("MV \'%s\'\n", lineBuf);
            memset(lineBuf, 0, sizeof(lineBuf));
            streamBuf >> lineBuf;
            if(toupper(*lineBuf) == 'L')
            {
                if(streamBuf >> commandServoDiff)
                {
                    ;
                }
            }
            else if(toupper(*lineBuf) == 'N')
            {
                if(streamBuf >> temp && temp && taskDefaultMode() == M_PID_LINE && ! commandTurnProtectFlag[0])
                {
                    taskDefaultStack(temp - 1);
                    commandTurnProtectFlag[0] = 1;
                    SerialDE.println("Got it! Prepare to turn...");
                    // libTimerTurn = libTimer.every(50, taskDefaultTriggerTurn);
                    // taskDefaultMode(M_TURN);
                }
            }
            else if(toupper(*lineBuf) == 'T')
            {
                if(commandTurnProtectFlag[0])
                {
                    libTimerTurn = libTimer.in(500, [](void*) -> bool{
                        commandTurnProtectFlag[0] = 0;
                        return taskDefaultMode(M_TURN), true;
                        });
                }
            }
            else if(toupper(*lineBuf) == 'S')
            {
                libTimer.in(200, [](void*) -> bool{return taskDefaultMode(M_IDLE_STOP), true;});
                libTimer.in(2000, [](void*) -> bool{return taskDefaultMode(M_AROUND), true;});
            }
        }
        else
        {
            lineBuf[tot ++] = charBuf;
            if(tot >= LINE_BUF_SIZE) tot = 0;
        }
    }
    return;
}

void serialSetup()
{
    Serial.begin(115200);
    Serial2.begin(19200); // Defined in hardware: RX2 == 16 TX2 == 17
    Serial2.setTimeout(100);
    Serial2.print("R\n"); // Which RESET the openmv.
    delay(100);
}

void (*controlFunction[])() = {};

void openMVProcess()
{
    static uint8_t lineBuf[LINE_BUF_SIZE] = {0};
    Serial2.readBytes(lineBuf, 1);
    Serial2.flush();
    switch(*lineBuf)
    {
    case '0':
        servoPutter.write(90);
        break;
    case '1':
        servoPutter.write(0);
        SerialDE.printf("Recive MV data 1!\n");
        break;
    case '2':
        servoPutter.write(180);
        SerialDE.printf("Recive MV data 2!\n");
        break;
    default:
        break;
    }
}
