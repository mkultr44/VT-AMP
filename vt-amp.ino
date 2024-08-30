#define LED_POWER 19
#define LED_TX 18
#define LED_RX 17
#define LED_BC 16
#define CTRL 26
#define CTRL2 25
#define VM 27
#define LEN 28
#define DET 29

volatile bool beacon = false;
long lastbeacon = 0;
bool latch = false;
float voltageprint;
uint32_t beacons = 0;

void rxtxctrl() {
  beacon = digitalReadFast(DET);
  digitalWriteFast(CTRL, beacon);
  digitalWriteFast(CTRL2, beacon);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(VM, INPUT);
  pinMode(DET, INPUT);
  pinMode(LEN, OUTPUT);
  pinMode(CTRL, OUTPUT);
  pinMode(CTRL2, OUTPUT);
  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);
  pinMode(LED_BC, OUTPUT);
  digitalWriteFast(LEN, latch);
  digitalWriteFast(LED_POWER, HIGH);
  digitalWriteFast(LED_BC, LOW);
  rxtxctrl();
  led();
  attachInterrupt(digitalPinToInterrupt(DET), rxtxctrl, CHANGE);
}

void serialout() {
  int lastbeaconminutes = (millis() - lastbeacon) / 60000;
  if (beacon) {
    Serial.printf("<TX> Last Beacon: %3i m Beacons: %5i VM: %4.1f mV", lastbeaconminutes, beacons, voltageprint);
    Serial.println();
  } else {
    Serial.printf("<RX> Last Beacon: %3i m Beacons: %5i VM: %4.1f mV", lastbeaconminutes, beacons, voltageprint);
    Serial.println();
  }
}

void led() {
if (beacons > 5) digitalWriteFast(LED_BC, HIGH);
digitalWriteFast(LED_TX, beacon);
digitalWriteFast(LED_RX, !beacon);
}

void loop() {
  if (beacon) {
    if (millis() - lastbeacon > 5000) {
      beacons = beacons + 1;
      beacon = false;
      lastbeacon = millis();
    }
  }
  int voltage = analogRead(VM);
  voltageprint = voltage * 3300 / 4096;
  serialout();
  led();
}
