#include "serial.h"
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
    static char lineBuf[20] = {0}, tot = 0;
    short tempM, tempL, tempR;
    std :: stringstream streamBuf;
    while((charBuf = SerialDE.read()) != -1)
    {
        if(charBuf == '\n')
        {
            lineBuf[tot ++] = charBuf;
            tot = 0;
            Serial.println(lineBuf);
            streamBuf.clear();
            streamBuf << lineBuf;
            streamBuf >> lineBuf;
            Serial.println(lineBuf);
            if(*lineBuf == 'M')
            {
                tempL = 100; tempR = 100;
                if(streamBuf >> tempL >> tempR)
                {
                    pref.putShort("speedLeft", tempL);
                    pref.putShort("speedRight", tempR);
                }
                // Serial.printf("Motor: %d %d\n", (int32_t)tempL, (int32_t)tempR);
                SerialDE.printf("Motor: %i %i\n", eeprGS("speedLeft"), eeprGS("speedRight"));
            }
            else if(*lineBuf == 'S')
            {
                tempL = 90;
                if(streamBuf >> tempM)
                {
                    pref.putShort("servo", tempM);
                    // pref.putShort("servoMin", tempL);
                    // pref.putShort("servoMax", tempR);
                }
                // Serial.printf("Servo: %d | %d %d\n", (int32_t)tempM, (int32_t)tempL, (int32_t)tempR);
                SerialDE.printf("Servo: %i | %i %i\n", eeprGS("servo"), eeprGS("servoMin"), eeprGS("servoMax"));
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
                    SerialDE.printf("Your name already reset to default! Reset now.\n");
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

void serialSetup()
{
    Serial2.begin(19200); // RX2 == 16 TX2 == 17
    Serial2.setTimeout(10);
}

void (*controlFunction[])() = {};