#include <vector>
#include <algorithm>

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
constexpr uint8_t LEDBAR_COUNT = 10;
constexpr uint8_t ledbar[LEDBAR_COUNT] = { LEDBAR1, LEDBAR2, LEDBAR3, LEDBAR4, LEDBAR5, LEDBAR6, LEDBAR7, LEDBAR8, LEDBAR9, LEDBAR10 };
volatile bool tx = false;
volatile bool rx = true;
volatile bool beacon = false;
unsigned long lastbeacon = 0;
unsigned long bartimeout = 0;
unsigned long beacons = 0;
double dbm_last = 0;
double dbm = 0;
bool latch = false;
int voltage = 0;

double interpolate(double x0, double y0, double x1, double y1, double y) {
  return x0 + (y - y0) * (x1 - x0) / (y1 - y0);
}

double convertoRFtreshold(double switchingVoltage) {
  const std::vector<std::pair<double, double>> data = {
    { -28, 0 }, { -24, 20 }, { -20, 80 }, { -16, 180 }, { -12, 360 },
    { -8, 600 }, { -4, 950 }, { 0, 1400 }, { 4, 2000 }, { 8, 2600 }
  };
  if (switchingVoltage <= data.front().second) return data.front().first;
  if (switchingVoltage >= data.back().second) return data.back().first;
  auto it = std::lower_bound(data.begin(), data.end(), std::make_pair(0.0, switchingVoltage),
                             [](const std::pair<double, double>& a, const std::pair<double, double>& b) {
                               return a.second < b.second;
                             });
  return interpolate((it - 1)->first, (it - 1)->second, it->first, it->second, switchingVoltage);
}

void ampctrl(bool txState) {
  digitalWriteFast(RXAMP, txState);
  digitalWriteFast(TXAMP, !txState);
}

void rxtxctrl() {
  tx = digitalReadFast(DET);
  rx = !tx;
  ampctrl(tx);
  digitalWriteFast(CTRL1, tx);
  digitalWriteFast(CTRL2, tx);
  beacon = tx;
}

void updateLEDBar() {
  if (dbm > dbm_last + 0.5 || dbm < dbm_last - 0.5) {
    bartimeout = millis() + 5000;
  }
  if (millis() < bartimeout) {
    int step = map(dbm, -14, 22, 0, LEDBAR_COUNT - 1);
    for (int i = 0; i < LEDBAR_COUNT; i++) {
      digitalWriteFast(ledbar[i], i <= step ? HIGH : LOW);
    }
  } else {
    for (int i = 0; i < LEDBAR_COUNT; ++i) {
      digitalWriteFast(ledbar[i], LOW);
    }
  }
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
    delay(50);
    digitalWriteFast(ledbar[i], LOW);
  }
  digitalWriteFast(LEDRX, LOW);
  digitalWriteFast(LEDTX, LOW);
  digitalWriteFast(LEDBC, LOW);
}


void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
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
  attachInterrupt(digitalPinToInterrupt(DET), rxtxctrl, CHANGE);
}

void loop() {
  if (beacon && (millis() - lastbeacon > 5000)) {
    beacons++;
    lastbeacon = millis();
    beacon = false;
  }
  voltage = analogRead(VM) * 3300 / 4096;
  dbm = convertoRFtreshold(voltage) + 10.5;
  serialout();
  updateLEDBar();
  dbm_last = dbm;
}
