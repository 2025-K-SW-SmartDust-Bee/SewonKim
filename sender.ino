#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#ifdef WIRELESS_STICK_V3
static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_64_32, RST_OLED);
#else
static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
#endif

#define RF_FREQUENCY                                915000000 // Hz
#define TX_OUTPUT_POWER                             5         // dBm
#define LORA_BANDWIDTH                              0
#define LORA_SPREADING_FACTOR                       7
#define LORA_CODINGRATE                             1
#define LORA_PREAMBLE_LENGTH                        8
#define LORA_SYMBOL_TIMEOUT                         0
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
double txNumber;
bool lora_idle = true;

static RadioEvents_t RadioEvents;
void OnTxDone(void);
void OnTxTimeout(void);

// ✅ Vext 전원 ON 함수 (OLED 전원 제어용)
void VextON(void) {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);  // LOW = ON
}

void setup() {
    Serial.begin(115200);
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

    // ✅ OLED 전원 ON 및 초기화
    VextON();
    delay(100);
    display.init();
    display.clear();
    display.setContrast(255);
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "LoRa Sender Init");
    display.display();

    txNumber = 0;

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;

    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
}

void loop() {
    if (lora_idle == true) {
        delay(1000);
        txNumber += 0.01;
        sprintf(txpacket, "Hello world number %0.2f", txNumber);

        Serial.printf("\r\nsending packet \"%s\" , length %d\r\n", txpacket, strlen(txpacket));

        // ✅ OLED 출력
        display.clear();
        display.drawString(0, 0, "Sending:");
        display.drawString(0, 12, txpacket);
        display.display();

        Radio.Send((uint8_t *)txpacket, strlen(txpacket));
        lora_idle = false;
    }

    Radio.IrqProcess();
}

void OnTxDone(void) {
    Serial.println("TX done......");

    display.drawString(0, 24, "TX done");
    display.display();

    lora_idle = true;
}

void OnTxTimeout(void) {
    Radio.Sleep();
    Serial.println("TX Timeout......");

    display.drawString(0, 24, "TX Timeout");
    display.display();

    lora_idle = true;
}
