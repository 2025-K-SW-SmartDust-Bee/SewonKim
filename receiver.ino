#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#ifdef WIRELESS_STICK_V3
static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_64_32, RST_OLED);
#else
static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
#endif

#define RF_FREQUENCY                                915000000
#define LORA_BANDWIDTH                              0
#define LORA_SPREADING_FACTOR                       7
#define LORA_CODINGRATE                             1
#define LORA_PREAMBLE_LENGTH                        8
#define LORA_SYMBOL_TIMEOUT                         0
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define RX_TIMEOUT_VALUE                            0
#define BUFFER_SIZE                                 256

char rxpacket[BUFFER_SIZE];
char bestPacket[BUFFER_SIZE];
int bestRSSI = -999;
bool newBestAvailable = false;

unsigned long lastDisplayTime = 0;
const unsigned long displayInterval = 5000;  // 5초마다 OLED 갱신

static RadioEvents_t RadioEvents;

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
void OnRxTimeout(void);
void OnRxError(void);

void VextON(void) {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

  // OLED 초기화
  VextON();
  delay(100);
  display.init();
  display.clear();
  display.setContrast(255);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "LoRa RSSI Filter");
  display.display();

  // LoRa 수신 초기화
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);
  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  Radio.Rx(RX_TIMEOUT_VALUE);
  lastDisplayTime = millis();
}

void loop() {
  Radio.IrqProcess();

  // 5초마다 OLED 갱신
  if (millis() - lastDisplayTime > displayInterval) {
    lastDisplayTime = millis();

    if (newBestAvailable) {
      Serial.printf("💡 Best packet: \"%s\" (RSSI: %d)\n", bestPacket, bestRSSI);
      display.clear();
      display.drawString(0, 0, "Best RX:");
      display.drawString(0, 12, bestPacket);
      display.drawString(0, 24, "RSSI: " + String(bestRSSI));
      display.display();
      newBestAvailable = false;
    } else {
      display.clear();
      display.drawString(0, 0, "No data in last 5s");
      display.display();
    }

    // 다음 수집을 위해 초기화
    bestRSSI = -999;
  }
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
  memcpy(rxpacket, payload, size);
  rxpacket[size] = '\0';

  Serial.printf("📶 RX: \"%s\" (RSSI: %d)\n", rxpacket, rssi);

  if (rssi > bestRSSI) {
    bestRSSI = rssi;
    memcpy(bestPacket, rxpacket, size + 1);
    newBestAvailable = true;
  }

  Radio.Rx(RX_TIMEOUT_VALUE);
}

void OnRxTimeout(void) {
  Radio.Rx(RX_TIMEOUT_VALUE);
}

void OnRxError(void) {
  Radio.Rx(RX_TIMEOUT_VALUE);
}
