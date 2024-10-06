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
constexpr int BLINKTIMER = 100;
constexpr float COUPLING = 8;
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

double convertoRFtreshold(double switchingVoltage) {
  const std::vector<std::pair<double, double>> data = {
    { -28, 0 }, { -24, 20 }, { -20, 80 }, { -16, 180 }, { -12, 360 }, { -8, 600 }, { -4, 950 }, { 0, 1400 }, { 4, 2000 }, { 8, 2600 }
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
    int step = map(dbm, -16, 14, 0, LEDBAR_COUNT - 1);
    if (step != laststep){
    for (int i = 0; i < LEDBAR_COUNT; i++) {
      if(step >= i) digitalWriteFast(ledbar[i], HIGH);
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
  attachInterrupt(digitalPinToInterrupt(DET), rxtxctrl, CHANGE);
}

void loop() {
  if (beacon && (millis() - lastbeacon > BEACONCD)) {
    beacons++;
    lastbeacon = millis();
    beacon = false;
  }
  voltage = analogRead(VM) * VCC / pow(2, BITDEPTH);
  dbm = convertoRFtreshold(voltage) + COUPLING;
  serialout();
  updateLED();
}
