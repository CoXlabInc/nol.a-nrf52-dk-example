#include <cox.h>
#include <BLEDevice.hpp>

#define SCAN_DURATION 10

BLEScan* pBLEScan = nullptr;
bool isScanning = false;

class MyAdvertisedDeviceListener: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    Serial.printf("[%lu.%06lu|SCANNED] %s\n", (uint32_t) tv.tv_sec, tv.tv_usec, advertisedDevice.toString().c_str());
  }
};


MyAdvertisedDeviceListener listener;


static void eventScanDone(BLEScanResults results) {
  isScanning = false;
  Serial.printf("* Scanning done. %lu devices found.\n", results.getCount());
  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice advertisedDevice = results.getDevice(i);
    BLEAddress addr = advertisedDevice.getAddress();

    Serial.printf("- [%u] %s", i, addr.toString().c_str());

    if (advertisedDevice.haveName()) {
      Serial.printf(", Name: %s", advertisedDevice.getName().c_str());
    }

    if (advertisedDevice.haveAppearance()) {
      Serial.printf(", Appearance: 0x%04X", advertisedDevice.getAppearance());
    }

    if (advertisedDevice.haveManufacturerData()) {
      Serial.print(", Manufacturer data:");
      std::string manufacturerData = advertisedDevice.getManufacturerData();
      for (uint8_t i = 0; i < manufacturerData.length(); i++) {
        Serial.printf(" %02X", (uint8_t) manufacturerData.data()[i]);
      }
    }

    if (advertisedDevice.haveServiceUUID()) {
      BLEUUID serviceUUID = advertisedDevice.getServiceUUID();
      Serial.printf(", service UUID: %s", serviceUUID.toString().c_str());
    }

    if (advertisedDevice.haveTXPower()) {
      int8_t txPower = advertisedDevice.getTXPower();
      Serial.printf(", TX power:%d dBm", txPower);
    }

    if (advertisedDevice.haveRSSI()) {
      int rssi = advertisedDevice.getRSSI();
      Serial.printf(", RSSI: %d dB", rssi);
    }

    Serial.printf(", Raw [");
    for (uint8_t i = 0; i < advertisedDevice.getPayloadLength(); i++) {
      Serial.printf(" %02X", (uint8_t) advertisedDevice.getPayload()[i]);
    }
    Serial.printf(" ]\n");
  }

  isScanning = pBLEScan->start(SCAN_DURATION, eventScanDone, false);
}

static void eventButtonPressed() {
  if (isScanning) {
    Serial.println("* Turn off scanner!");
    pBLEScan->stop();
    isScanning = false;
  } else {
    isScanning = pBLEScan->start(SCAN_DURATION, eventScanDone, false);
    if (isScanning) {
      printf("* Turn on scanner!\n");
    } else {
      printf("* Scan failed.\n");
    }
  }
}

void setup() {
  Serial.begin(115200);
  printf("\n*** [nRF52-DK] BLE Scanner ***\n");
  printf("* Reset by 0x%08lx\n", System.getResetReason());
  if (System.getResetReason() & (System.RESET_REASON_DOG | System.RESET_REASON_SREQ)) {
    const McuNRF51::StackDump *last = System.getLastStackDump();
    printf(" - R0: 0x%08lx, R1: 0x%08lx, R2: 0x%08lx, R3: 0x%08lx\n", last->r0, last->r1, last->r2, last->r3);
    printf(" - R12: 0x%08lx, LR: 0x%08lx, PC: 0x%08lx, PSR: 0x%08lx\n", last->r12, last->lr, last->pc, last->psr);
  }

  System.onButtonPressed(0, eventButtonPressed);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(&listener);
  pBLEScan->setActiveScan(true);

  isScanning = pBLEScan->start(SCAN_DURATION, eventScanDone, false);
  if (isScanning) {
    printf("* Now scanning...\n");
  } else {
    printf("* Scan failed.\n");
  }
}
