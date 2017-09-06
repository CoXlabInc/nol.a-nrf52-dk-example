#include <cox.h>

Timer tPrint;

static void printTask(void *) {
  System.ledToggle(0);
  printf("[%lu usec] Hi!\n", micros());
}

static void keyboard(SerialPort&) {
  System.ledToggle(0);
  printf("[%lu usec] Keyboard input\n", micros());
}

static void eventButtonPressed(uint8_t n) {
  System.ledToggle(n);
  printf("[%lu usec] User button [%u] pressed\n", micros(), n);
}

static void eventButton1Pressed() {
  eventButtonPressed(0);
}

static void eventButton2Pressed() {
  eventButtonPressed(1);
}

static void eventButton3Pressed() {
  eventButtonPressed(2);
}

static void eventButton4Pressed() {
  eventButtonPressed(3);
}

void setup() {
  System.ledOn(0);
  Serial.begin(115200);
  printf("\n*** [nRF52-DK] Basic Functions ***\n");

  tPrint.onFired(printTask, NULL);
  tPrint.startPeriodicMicros(1000000);

  Serial.listen();
  Serial.onReceive(keyboard);

  System.onButtonPressed(0, eventButton1Pressed);
  System.onButtonPressed(1, eventButton2Pressed);
  System.onButtonPressed(2, eventButton3Pressed);
  System.onButtonPressed(3, eventButton4Pressed);
}
