#include "serial.h"
#include "pref.h"
#include "main.h"
#include "motor.h"
#include "ble.h"

#include <string>
#include <sstream>

void carProcess()
{
    return;
}

void debugProcess()
{
    static int8_t charBuf = 0;
    static char lineBuf[40] = {0}, tot = 0;
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
            if(*lineBuf == 'M')
            {
                tempL = 100; tempR = 100;
                if(streamBuf >> tempL)
                {
                    pref.putShort("speedLeft", tempL);
                }
                if(streamBuf >> tempR)
                {
                    pref.putShort("speedRight", tempR);
                }
                SerialDE.printf("Motor: %i %i\n", EEPR_GS("speedLeft"), EEPR_GS("speedRight"));
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
                resetFunc();
            }
            else if(*lineBuf == 'C')
            {
                if(streamBuf >> lineBuf)
                {
                    SerialDE.printf("Your name already changed! Reset now.\n");
                }
                else
                {
                    pref.remove("bleName");
                    SerialDE.printf("Your name already reset to default! Reset now.\n");
                }
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
                SerialDE.printf("Motor: %f %f %f\n", EEPR_GD("motorP"), EEPR_GD("motorI"), EEPR_GD("motorD"));
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

void openMVProcess()
{
    static uint8_t lineBuf[20] = {0};
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

void openMVLineProcess()
{
    static int8_t charBuf = 0;
    static char lineBuf[40] = {0}, tot = 0;
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
            memset(lineBuf, 0, sizeof(lineBuf));
            if(streamBuf >> temp)
            {
                pref.putShort("servo", temp + 
                    pref.getShort("servoMid", 90)
                );
                if(temp != tempL)
                {
                    tempL = temp;
                    // SerialDE.printf("S - %d\n", (int)(temp));
                }
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

void serialSetup()
{
    Serial2.begin(19200); // Defined in hardware: RX2 == 16 TX2 == 17
    Serial2.setTimeout(100);
    Serial2.print("R\n"); // Which RESET the openmv.
    delay(100);
}

void (*controlFunction[])() = {};