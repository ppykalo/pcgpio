#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "protocol.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

Protocol protocol;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      protocol.tryProcessReceivedData((const uint8_t*)rxValue.c_str(), rxValue.length(),
        [=](uint8_t* response, uint8_t responseLength) {
            pCharacteristic->setValue(response, responseLength);
        }
      );
    }
};

void setup() {
    protocol.addFunction(0x01, 0x01, [](const Protocol::OperationContext& context) {
        uint8_t writeValue = context.argument(0) ? 0x01 : 0x00;
        //Serial.printf("Writing %d to GPIO %d\n", writeValue, gpio);
        digitalWrite(context.device(), writeValue);
    });
    protocol.addFunction(0x01, 0x02, [](const Protocol::OperationContext& context) {
        //Serial.printf("Writing %d to GPIO %d\n", writeValue, gpio);
        uint8_t response[1];
        response[0] = digitalRead(context.device());
        context.writeResponse(response, 1);
    });

    protocol.addFunction(0x01, 0x00, [](const Protocol::OperationContext& context) {
        uint8_t modeValue = context.argument(0);
        //Serial.printf("Setting mode %d to GPIO %d\n", modeValue, gpio);
        pinMode(context.device(), modeValue);
    });

    // put your setup code here, to run once:
    //pinMode(1, OUTPUT);

    // BLE initialization
    //Serial.begin(9600);
    // Serial.println("Starting BLE work!");

    BLEDevice::init("BLE IO");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_WRITE
                                        );
    pCharacteristic->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("Hello World says Neil");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    // Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
    // put your main code here, to run repeatedly:
    //Serial.print(".");
    delay(1000);
}
