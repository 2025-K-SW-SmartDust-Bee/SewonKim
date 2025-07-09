

# 📡 LoRa 송수신기 프로젝트 (RSSI 기반 필터링)

이 프로젝트는 Heltec WiFi LoRa 32 (V3) 보드를 이용해 LoRa 송수신 통신을 구현하고, 수신 측에서 가장 가까운 송신기의 메시지(RSSI 기준으로 가장 강한 신호)를 선택해 OLED에 표시합니다.

---

## 💡 프로젝트 개요

* **송신기 (Sender)**: "Hello world number X.XX" 포맷의 메시지를 일정 주기로 전송
* **수신기 (Receiver)**: 5초 간 수신된 메시지 중 가장 강한 RSSI를 가진 하나만 OLED에 출력

---

## ✅ 사용 보드 및 환경

| 구성 요소 | 설명                                  |
| ----- | ----------------------------------- |
| MCU   | Heltec WiFi LoRa 32 V3              |
| 주파수   | 915 MHz (미국 주파수 대역)                 |
| 화면    | 내장 OLED (128x64)                    |
| 라이브러리 | `LoRaWan_APP.h`, `HT_SSD1306Wire.h` |

---

## 🚀 작동 흐름

### 송신기

```plaintext
setup():
  - LoRa 설정 (주파수, SF, 대역폭 등)
  - 메시지 포맷 정의 ("Hello world number X.XX")
  
loop():
  - 1초 간격으로 메시지 전송
  - 전송 성공 시 "TX done" 출력
```

### 수신기

```plaintext
setup():
  - OLED 디스플레이 초기화
  - LoRa 수신 설정 및 콜백 등록

loop():
  - LoRa 인터럽트 처리 (Radio.IrqProcess)
  - 5초마다 가장 강한 메시지를 OLED에 출력
  - 출력 후 기록 초기화

OnRxDone():
  - 새 패킷 수신 시, RSSI를 기준으로 가장 강한 메시지만 저장
```

---

## 📐 주요 코드 로직

### 송신기 (`sender.ino`)

```cpp
sprintf(txpacket,"Hello world number %0.2f",txNumber);
Radio.Send((uint8_t *)txpacket, strlen(txpacket));
```

* 메시지를 포맷팅하여 1초 간격으로 전송
* `OnTxDone()`에서 성공 메시지를 시리얼에 출력

---

### 수신기 (`receiver.ino`)

```cpp
if (rssi > bestRSSI) {
    bestRSSI = rssi;
    memcpy(bestPacket, rxpacket, size + 1);
    newBestAvailable = true;
}
```

* 새 메시지를 수신할 때마다 `RSSI` 값과 비교
* 가장 강한 신호의 메시지만 `bestPacket`에 저장

---

## 🖥️ OLED 출력 예시

```plaintext
Best RX:
Hello world number 3.14
RSSI: -45
```

* `drawString()` 함수를 통해 메시지 내용과 RSSI 값을 OLED에 출력

---

## 🔁 전체 흐름 요약 (수신기 기준)

```
[보드 부팅]
     ↓
LoRa 수신 시작
     ↓
패킷 수신 시 RSSI 비교
     ↓
5초 동안 가장 강한 패킷만 저장
     ↓
OLED에 출력 후 초기화
     ↓
반복
```

---

## 📁 파일 구성

| 파일명            | 설명               |
| -------------- | ---------------- |
| `sender.ino`   | 송신기용 아두이노 코드     |
| `receiver.ino` | 수신기용 아두이노 코드     |
| `README.md`    | 프로젝트 문서          |
| `.gitignore`   | 빌드 캐시 무시 설정 (선택) |

---

## 📦 설치 및 업로드 방법

1. Arduino IDE 설치 후 Heltec ESP32 보드 매니저 추가
2. 필요한 라이브러리 설치:

   * `Heltec ESP32 Dev-Boards`
   * `HT_SSD1306Wire`
3. 각 보드에 `sender.ino` 또는 `receiver.ino` 업로드
4. 시리얼 모니터(115200bps)에서 로그 확인
5. 수신기 OLED에서 가장 가까운 송신 메시지 확인

---

## 📡 향후 확장 아이디어

* 여러 송신기 구분 (ID 포함)
* 메시지 암호화
* 수신 로그 저장
* 메시지 ACK 구현


