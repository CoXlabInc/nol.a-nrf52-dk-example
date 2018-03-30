#include <cox.h>
#include <MMA8452Q.hpp>

MMA8452Q acc;
Timer timerPrintXYZ;

static void taskPrintXYZ(void *) {
  int16_t x, y, z;
  if (acc.isActive()) {
    acc.readXYZ(&x, &y, &z);
    printf("* X:%5d, Y:%5d, Z:%5d\n", x, y, z);
  } else {
    printf("* Gyro is turned off.\n");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.printf("\n*** [nRF52-DK] MMA8452Q 3-axis accelerometer ***\n");

  Wire.begin();
  acc.begin(Wire, 0x1D, -1, -1);

  printf("* Gyro ID: 0x%02X \n", acc.readSensorId());//, acc.isActive() ? "ON" : "OFF");
  printf("* Gyro ODR: 0x%X\n", acc.getODR());
  acc.setODR(MMA8452Q::ODR_100Hz);
  acc.setActive();

  timerPrintXYZ.onFired(taskPrintXYZ, NULL);
  timerPrintXYZ.startPeriodic(1000);
}
