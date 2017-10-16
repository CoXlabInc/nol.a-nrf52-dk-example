#include <cox.h>

Timer tPrint;

static void printTask(void *) {
  System.ledToggle(0);
  Serial.printf("[%lu usec] Hi!\n", micros());
  Serial.printf("* Random number:%lu\n", random());
  Serial.printf("* A0: %ld mV\n", map(analogRead(3), 0, 4095, 0, 3300));
}

static void keyboard(SerialPort &p) {
  System.ledToggle(0);
  Serial.printf("[%lu usec] Keyboard input:", micros());
  while (p.available() > 0) {
    char c = p.read();
    Serial.print(c);
  }
  Serial.println();
}

static void eventButtonPressed(uint8_t n) {
  System.ledToggle(n);
  Serial.printf("[%lu usec] User button [%u] pressed\n", micros(), n);
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
  Serial.printf("\n*** [nRF52-DK] Basic Functions ***\n");

  tPrint.onFired(printTask, NULL);
  tPrint.startPeriodic(1000);

  // Serial.listen();
  Serial.onReceive(keyboard);

  System.onButtonPressed(0, eventButton1Pressed);
  System.onButtonPressed(1, eventButton2Pressed);
  System.onButtonPressed(2, eventButton3Pressed);
  System.onButtonPressed(3, eventButton4Pressed);

  pinMode(3, INPUT);
}
