#include <cox.h>
#include <BLEDevice.hpp>


#define SERVICE_UUID        "f64f0000-7fdf-4b2c-ad31-e65ca15bef6b"
#define CHARACTERISTIC_UUID "f64f0100-7fdf-4b2c-ad31-e65ca15bef6b"

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) {
    std::string value = pChar->getValue();

    if (value.length() > 0) {
      printf("*******\n");
      printf("%s\n", value.c_str());
      printf("*******\n");
    }
  };
} myCallbacks;

void setup() {
  Serial.begin(115200);
  printf("\n### [nRF52-DK] BLE GATT Server ###\n");

  BLEDevice::init("My nRF52-DK");
  BLEServer *server = BLEDevice::createServer();
  printf("- Server created: %p\n", server);

  BLEService *service = server->createService(SERVICE_UUID);
  printf("- Service created\n");

  BLECharacteristic *characteristic = service->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  characteristic->setCallbacks(&myCallbacks);
  printf("- Characteristic created\n");

  service->start();
  printf("- Characteristic added\n");

  BLEAdvertising *adv = server->getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->setMinIntervalMicros(1000000);
  adv->start();
  printf("- Advertising started...\n");
}
