#include <vector>
#include <algorithm>

constexpr int VCC = 3300;
constexpr int BITDEPTH = 12;
constexpr int BEACONCD = 5000;
constexpr int CTRL1 = 3;
constexpr int CTRL2 = 4;
constexpr int VM = 27;
constexpr int LEN = 2;
constexpr int DET = 29;
constexpr int RXAMP = 8;
constexpr int TXAMP = 12;
constexpr int LEDTX = 24;
constexpr int LEDRX = 23;
constexpr int LEDBC = 21;
constexpr int LEDMCU = 22;
constexpr int LEDBAR1 = 28;
constexpr int LEDBAR2 = 26;
constexpr int LEDBAR3 = 25;
constexpr int LEDBAR4 = 20;
constexpr int LEDBAR5 = 9;
constexpr int LEDBAR6 = 10;
constexpr int LEDBAR7 = 19;
constexpr int LEDBAR8 = 18;
constexpr int LEDBAR9 = 13;
constexpr int LEDBAR10 = 14;
constexpr int BLINKTIMER = 1000;
constexpr uint8_t LEDBAR_COUNT = 10;
constexpr uint8_t ledbar[LEDBAR_COUNT] = { LEDBAR1, LEDBAR2, LEDBAR3, LEDBAR4, LEDBAR5, LEDBAR6, LEDBAR7, LEDBAR8, LEDBAR9, LEDBAR10 };
volatile bool tx = false;
volatile bool rx = true;
volatile bool beacon = false;
unsigned long lastbeacon = 0;
unsigned long blink = 0;
unsigned long bartimeout = 0;
unsigned long beacons = 0;
int laststep = 0;
double dbm = 0;
bool latch = false;
int voltage = 0;
uint8_t beaconindicator = 0;

double interpolate(double x0, double y0, double x1, double y1, double y) {
  return x0 + (y - y0) * (x1 - x0) / (y1 - y0);
}

double convertoRFthreshold(double switchingVoltage) {
  const std::vector<std::pair<double, double>> data = {
    { -15.8, 114.4 },
    { -14.4, 116.1 },
    { -13.1, 120.5 },
    { -11.7, 123.3 },
    { -10.3, 125.2 },
    { -9.0, 132.6 },
    { -7.6, 139.3 },
    { -6.3, 145.3 },
    { -4.9, 159.5 },
    { -3.6, 168.8 },
    { -2.2, 189.3 },
    { -0.8, 207.9 },
    { 0.5, 239.4 },
    { 1.9, 271.1 },
    { 3.3, 311.7 },
    { 4.5, 345.1 },
    { 6.0, 393.8 },
    { 8.6, 525.1 },
    { 9.9, 598.4 },
    { 13.8, 931.2 },
    { 14.7, 1007.3 },
    { 15.6, 1150.5 },
    { 16.7, 1264.2 },
    { 17.6, 1404.3 },
    { 18.5, 1511.3 },
    { 19.2, 1658.2 },
    { 19.6, 1776.4 },
    { 20.3, 1929.3 },
    { 21.0, 2058.4 },
    { 21.1, 2380.6 },
    { 21.4, 2185.9 },
    { 21.7, 2281.6 },
    { 21.9, 2570.8 }
  };
  if (switchingVoltage <= data.front().second) return data.front().first;
  if (switchingVoltage >= data.back().second) return data.back().first;

  auto it = std::lower_bound(data.begin(), data.end(), std::make_pair(0.0, switchingVoltage),
                             [](const std::pair<double, double>& a, const std::pair<double, double>& b) {
                               return a.second < b.second;
                             });

  return interpolate((it - 1)->first, (it - 1)->second, it->first, it->second, switchingVoltage);
}

void rxtxctrl() {
  tx = digitalReadFast(DET);
  digitalWriteFast(CTRL1, tx);
  digitalWriteFast(CTRL2, tx);
  digitalWriteFast(RXAMP, tx);
  digitalWriteFast(TXAMP, !tx);
  rx = !tx;
  beacon = tx;
}

void blinkBC() {
  if (blink + BLINKTIMER < millis()) {
    digitalWriteFast(LEDBC, HIGH);
  }
  if (blink + 2 * BLINKTIMER < millis()) {
    digitalWriteFast(LEDBC, LOW);
    blink = millis();
  }
}

void updateLED() {
  if (beacons > 0 && beacons < 5) blinkBC();
  if (beacons >= 5) digitalWriteFast(LEDBC, HIGH);
  int step = map(dbm, -16, 22, 0, LEDBAR_COUNT - 1);
  if (step != laststep) {
    for (int i = 0; i < LEDBAR_COUNT; i++) {
      if (step >= i) digitalWriteFast(ledbar[i], HIGH);
      else digitalWriteFast(ledbar[i], LOW);
    }
    bartimeout = millis() + 5000;
  } else if (millis() > bartimeout) {
    for (int i = 0; i < LEDBAR_COUNT; i++) {
      digitalWriteFast(ledbar[i], LOW);
    }
  }
  laststep = step;
  digitalWriteFast(LEDTX, tx);
  digitalWriteFast(LEDRX, rx);
  digitalWriteFast(LEDBC, beacons >= 5 ? HIGH : LOW);
}

void serialout() {
  int lastbeaconminutes = (millis() - lastbeacon) / 60000;
  Serial.printf("%s Last Beacon: %4i m Beacons: %5lu TX-Threshold: %3.1f dBm\n",
                tx ? "<TX>" : "<RX>", lastbeaconminutes, beacons, dbm);
}

void ledboot() {
  digitalWriteFast(LEDRX, HIGH);
  digitalWriteFast(LEDTX, HIGH);
  digitalWriteFast(LEDBC, HIGH);
  digitalWriteFast(LEDMCU, HIGH);
  delay(250);
  for (int i = 0; i < LEDBAR_COUNT; i++) {
    digitalWriteFast(ledbar[i], HIGH);
    delay(33);
    digitalWriteFast(ledbar[i], LOW);
  }
  digitalWriteFast(LEDRX, LOW);
  digitalWriteFast(LEDTX, LOW);
  digitalWriteFast(LEDBC, LOW);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(BITDEPTH);
  pinMode(VM, INPUT);
  pinMode(DET, INPUT);
  pinMode(LEN, OUTPUT);
  pinMode(CTRL1, OUTPUT);
  pinMode(CTRL2, OUTPUT);
  pinMode(TXAMP, OUTPUT);
  pinMode(RXAMP, OUTPUT);
  pinMode(LEDTX, OUTPUT);
  pinMode(LEDRX, OUTPUT);
  pinMode(LEDBC, OUTPUT);
  pinMode(LEDMCU, OUTPUT);
  for (int i = 0; i < LEDBAR_COUNT; i++) {
    pinMode(ledbar[i], OUTPUT);
  }
  digitalWriteFast(LEN, latch);
  ledboot();
  rxtxctrl();
  voltage = analogRead(VM) * VCC / pow(2, BITDEPTH);
  dbm = convertoRFthreshold(voltage);
  int step = map(dbm, -16, 22, 0, LEDBAR_COUNT - 1);
  laststep = step;
  attachInterrupt(digitalPinToInterrupt(DET), rxtxctrl, CHANGE);
}

void loop() {
  if (beacon && (millis() - lastbeacon > BEACONCD)) {
    beacons++;
    lastbeacon = millis();
    beacon = false;
  }
  voltage = analogRead(VM) * VCC / pow(2, BITDEPTH);
  dbm = convertoRFthreshold(voltage);
  serialout();
  updateLED();
}
