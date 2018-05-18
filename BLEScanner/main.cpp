#include <cox.h>

Timer timerHello;
Timer timerLEDOff;
BLEMac *ble;

static void taskHello(void *) {
  System.ledOn(0);
  timerLEDOff.startOneShot(10);
}

static void taskLEDOff(void *) {
  System.ledOff(0);
}

static void eventButtonPressed() {
  if (ble->isScanning()) {
    Serial.println("* Turn off scanner!");
    ble->endScan();
    timerHello.stop();
  } else {
    error_t err = ble->beginScan();
    if (err == ERROR_SUCCESS) {
      Serial.println("* Turn on scanner!");
      timerHello.startPeriodic(1000);
    } else {
      Serial.print("* Error during turning on scanner: ");
      Serial.print(err);
    }
  }
}

static void eventBLEAdvScanned(
  BLEMac::AdvPDU_t pduType,
  const BLEAddress *addr,
  const BLEAddress *directedAddr,
  const uint8_t *data,
  uint8_t len,
  int8_t rssi) {
  Serial.print("[SCANNED] PDU type:");
  Serial.print((uint8_t) pduType);
  Serial.print(", Source Address:");
  if (addr->isPeer) {
    Serial.print("(P)");
  }
  for (uint8_t i = 0; i < 6; i++) {
    Serial.printf(" %02X", addr->octet[i]);
  }
  if (directedAddr) {
    Serial.print(", Directed Address:");
    if (directedAddr->isPeer) {
      Serial.print("(P)");
    }
    for (uint8_t i = 0; i < 6; i++) {
      Serial.printf(" %02X", directedAddr->octet[i]);
    }
  }
  Serial.print(", Data:");
  for (uint8_t i = 0; i < len; i++) {
    Serial.printf(" %02X", data[i]);
  }
  Serial.print(", RSSI:");
  Serial.print(rssi);
  Serial.println(" dB");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("*** [nRF52-DK] BLE Scanner ***");

  timerHello.onFired(taskHello, NULL);
  timerLEDOff.onFired(taskLEDOff, NULL);

  System.onButtonPressed(0, eventButtonPressed);

  uint32_t ramReq;
  ble = System.initSoftDevice(0, 1, &ramReq);
  if (ble) {
    Serial.print("* BLE stack initialized successfully. The minimum required RAM base is 0x");
    Serial.print(ramReq, HEX);
    Serial.println(".");

    ble->onScanned(eventBLEAdvScanned);
    ble->setScanWindow(10000);
    ble->setScanInterval(10000);
    error_t err = ble->beginScan();
    Serial.print("* beginScan: "); Serial.println(err);
    if (err == ERROR_SUCCESS) {
      timerHello.startPeriodic(1000);
    }
  } else {
    Serial.print("* BLE stack: RAM base must be adjusted to 0x");
    Serial.print(ramReq, HEX);
    Serial.println(".");
  }
}
