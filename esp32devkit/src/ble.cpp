#include "ble.h"
#include "main.h"
#include "serial.h"
#include "rtos_core.h"

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <sys/time.h>
#include <sstream>

#define BLE_NAME_MAX_20 "NUEDC-CQU-Undefined"

// See the following for generating UUIDs: https://www.uuidgenerator.net/
#define SERVICE_CAR_UUID "48680001-6EC3-4CED-BA37-CC2AACD4D821" // UART service UUID
#define CHARACTER_CAR_UUID_RX "48680002-6EC3-4CED-BA37-CC2AACD4D821"
#define CHARACTER_CAR_UUID_TX "48680003-6EC3-4CED-BA37-CC2AACD4D821"
#define SERVICE_DEBUG_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // Predefined service UUID, same as Nordic nRF
#define CHARACTER_DEBUG_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTER_DEBUG_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLESerial SerialBL, SerialDE;

void bleSetup()
{
    static char bleName[20] = BLE_NAME_MAX_20;
    // Create the BLE Device & Server
    if(!pref.isKey("bleName"))
    {
        pref.putBytes("bleName", BLE_NAME_MAX_20, 20);
    }
    else
    {
        pref.getBytes("bleName", bleName, 20);
    }
    BLESerial::deviceStart(bleName);
    SerialBL.begin(0, SERVICE_CAR_UUID, CHARACTER_CAR_UUID_RX, CHARACTER_CAR_UUID_TX);
    SerialDE.begin(0, SERVICE_DEBUG_UUID, CHARACTER_DEBUG_UUID_RX, CHARACTER_DEBUG_UUID_TX);
    // SerialDE.setTimeout(0); has no effect, see read()
}

void bleLoop()
{
    debugProcess();
    carProcess();
}


class BLESerialServerCallbacks: public BLEServerCallbacks {
    friend class BLESerial;
    BLESerial* bleSerial;
    
    void onConnect(BLEServer* pServer) {
        // xTaskCreateUniversal(this -> bleOnConnect, "BLEOnConnect", 1024, this -> bleSerial, 2, &NULLTaskHandle, ARDUINO_RUNNING_CORE);
        // delay(1000); // wait for connection to complete or messages can be lost
    };

    void bleOnConnect(void* bleSerialVoid)
    {
        BLESerial* bleSerial = (BLESerial*)bleSerialVoid;
        vTaskDelay(2000);
        bleSerial -> printf(
            "** Your config **\n%i %i %i\n",
            pref.getShort("servo", -1), pref.getShort("servoMin", -1), pref.getShort("servoMax", -1)
        );
    }

    void onDisconnect(BLEServer* pServer) {
        pServer->startAdvertising(); // restart advertising
        Serial.println("Started advertising");
    }
};

class BLESerialCharacteristicCallbacks: public BLECharacteristicCallbacks {
    friend class BLESerial; 
    BLESerial* bleSerial;
    
    void onWrite(BLECharacteristic *pCharacteristic) {
 
        // Serial.printf("recevie! %s\n", pCharacteristic -> getUUID().toString());
      bleSerial->receiveBuffer = bleSerial->receiveBuffer + pCharacteristic->getValue();
    }

};

bool BLESerial::deviceStart(const char* deviceName)
{
    BLEDevice::init(deviceName);
    return true;
}

// Constructor
BLESerial::BLESerial()
{
  // create instance  
  receiveBuffer = "";
}

// Destructor
BLESerial::~BLESerial(void)
{
    // clean up
}

// Begin bluetooth serial
/** @brief implement begin()
 *  @param mode 0 == Server, 1 == Client.
 */
bool BLESerial::begin(bool mode, const char* serviceUUID, const char* rxUUID, const char* txUUID)
{
    // Create the BLE Device if not init...
    BLEDevice::init("DEBUG");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    if (pServer == nullptr)
        return false;
    
    BLESerialServerCallbacks* bleSerialServerCallbacks =  new BLESerialServerCallbacks(); 
    bleSerialServerCallbacks->bleSerial = this;      
    pServer->setCallbacks(bleSerialServerCallbacks);

    // Create the BLE Service
    pService = pServer->createService(serviceUUID);
    if (pService == nullptr)
        return false;

    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(
                                            txUUID,
                                            BLECharacteristic::PROPERTY_NOTIFY
                                        );
    if (pTxCharacteristic == nullptr)
        return false;                    
    pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                                                rxUUID,
                                                BLECharacteristic::PROPERTY_WRITE
                                            );
    if (pRxCharacteristic == nullptr)
        return false; 

    BLESerialCharacteristicCallbacks* bleSerialCharacteristicCallbacks =  new BLESerialCharacteristicCallbacks(); 
    bleSerialCharacteristicCallbacks->bleSerial = this;  
    pRxCharacteristic->setCallbacks(bleSerialCharacteristicCallbacks);

    // Start the service
    pService->start();
    // Serial.println("starting service");

    // Start advertising
    pServer->getAdvertising()->addServiceUUID(pService->getUUID()); 
    pServer->getAdvertising()->start();
    Serial.printf("%s has been activated.\n", serviceUUID);
    return true;
}

int BLESerial::available(void)
{
    // reply with data available
    return receiveBuffer.length();
}

int BLESerial::peek(void)
{
    // return first character available
    // but don't remove it from the buffer
    if ((receiveBuffer.length() > 0)){
        uint8_t c = receiveBuffer[0];
        return c;
    }
    else
        return -1;
}

bool BLESerial::connected(void)
{
    return (pServer->getConnectedCount() > 0) ? true : false;
}

int BLESerial::read(void)
{
    // read a character
    if ((receiveBuffer.length() > 0)){
        uint8_t c = receiveBuffer[0];
        receiveBuffer.erase(0,1); // remove it from the buffer
        return c;
    }
    else
    {
        return -1;
    }
}

size_t BLESerial::write(uint8_t c)
{
    // write a character
    uint8_t _c = c;
    pTxCharacteristic->setValue(&_c, 1);
    pTxCharacteristic->notify();
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    // TODO: Timer! or task
    return 1;
}

size_t BLESerial::write(const uint8_t *buffer, size_t size)
{
    // write a buffer
    if(!this -> connected())
    {
        log_e("%s BTSerial ERROR: no client.\n", this->pService->getUUID().toString().c_str());
        return 0;
    }
    for(int i=0; i < size; i++){
        write(buffer[i]);
    }
    return size;
}

void BLESerial::flush()
{
    // remove buffered data
    receiveBuffer.clear();
}

void BLESerial::end()
{
    // close connection
}