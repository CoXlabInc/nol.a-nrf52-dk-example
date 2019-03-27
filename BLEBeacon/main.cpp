#include <cox.h>
#include <BLEDevice.hpp>
#include <BLEBeacon.hpp>

BLEAdvertising *pAdvertising = nullptr;
bool isAdvertising = false;

Timer timerHello;
Timer timerLEDOff;

#define BEACON_UUID "8ec76ea3-6668-48da-9866-75be8bc86f4d" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)

static void taskHello(void *) {
  digitalWrite(D6, LOW);
  timerLEDOff.startOneShot(10);
}

static void taskLEDOff(void *) {
  digitalWrite(D6, HIGH);
}

static void eventButtonPressed() {
  if (!pAdvertising) {
    Serial.printf("* BLE Advertiser is not initialized.\n");
    return;
  }

  if (isAdvertising) {
    Serial.printf("* Turn off beacon!\n");
    pAdvertising->stop();
    timerHello.stop();
    isAdvertising = false;
  } else {
    pAdvertising->start();
    Serial.printf("* Turn on beacon!\n");
    timerHello.startPeriodic(1000);
    isAdvertising = true;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("*** [nRF52-DK] BLE Beacon ***");

  System.ledOn(0);
  pinMode(D6, OUTPUT);
  digitalWrite(D6, HIGH);
  timerHello.onFired(taskHello, NULL);
  timerLEDOff.onFired(taskLEDOff, NULL);

  System.onButtonPressed(0, eventButtonPressed);

  BLEDevice::init("");

  BLEServer *pServer = BLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  oBeacon.setMajor(0x0102);
  oBeacon.setMinor(0x0304);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04

  std::string strServiceData = "";

  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData();
  oAdvertisementData.addData(strServiceData);

  pAdvertising->setAdvertisementData(oAdvertisementData);

  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  oScanResponseData.setName("CoXlab");
  pAdvertising->setScanResponseData(oScanResponseData);

  timerHello.startPeriodic(1000);

  pAdvertising->start();
  isAdvertising = true;
  Serial.println("* Advertising started...");
}
