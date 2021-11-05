#ifndef __BLE_H__
#define __BLE_H__

#include <Arduino.h>
#include <BLEDevice.h>

void bleSetup();
void bleLoop();

class BLESerial: public Stream
{
    public:

        static bool deviceStart(const char* deviceName);

        BLESerial(void);
        ~BLESerial(void);

        bool begin(bool mode, const char* serviceUUID, const char* rxUUID, const char* txUUID);
        int available(void);
        int peek(void);
        bool connected(void);
        int read(void);
        size_t write(uint8_t c);
        size_t write(const uint8_t *buffer, size_t size);
        void flush();
        void end(void);

    private:
        String local_name;
        BLEServer *pServer = NULL;
        BLEService *pService;
        BLECharacteristic * pTxCharacteristic;
        bool deviceConnected = false;
        uint8_t txValue = 0;
        
        std::string receiveBuffer;

        friend class BLESerialServerCallbacks;
        friend class BLESerialCharacteristicCallbacks;

};

#endif /* __BLE_H__ */