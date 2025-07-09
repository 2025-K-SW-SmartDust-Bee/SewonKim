# SewonKim
sender-receiver
아래는 수신기 코드의 **로직 정리 요약**으로, GitHub README에 바로 붙여 넣을 수 있도록 Markdown 형식으로 작성한 내용입니다:

---

## 📡 LoRa 수신기 (RSSI 기반 필터링)

이 프로젝트는 **여러 LoRa 송신기 중 가장 가까운 송신기(신호가 가장 강한)** 메시지 하나만 선택하여 OLED에 출력하는 수신기 코드입니다. `Heltec ESP32 WiFi LoRa V3` 보드와 내장 OLED를 활용합니다.

---

### 🔁 작동 흐름

```
[수신기 보드 부팅]
       ↓
LoRa 수신 시작 (915 MHz, SF7, BW 125 kHz)
       ↓
수신된 메시지를 RSSI 값 기준으로 비교
       ↓
5초 동안 가장 강한 메시지 1개만 저장
       ↓
5초마다 OLED에 가장 가까운 송신기의 메시지 출력
       ↓
이전 기록 초기화 후 반복
```

---

### ⚙️ 주요 로직 요약

| 기능 | 설명 |
|------|------|
| `Radio.IrqProcess()` | LoRa 인터럽트 처리 루프 |
| `OnRxDone()` | 패킷 수신 성공 시 호출. RSSI 기준으로 가장 강한 메시지 저장 |
| `loop()` | 매 5초마다 OLED 화면 갱신: 가장 강한 메시지 1개 출력 |
| `OnRxTimeout()` / `OnRxError()` | 수신 실패 시 자동으로 수신 재시작 |
| `display.drawString()` | Heltec 보드 내장 OLED에 수신 정보 출력 |

---

### 📐 RSSI 필터링 방식

```cpp
if (rssi > bestRSSI) {
    bestRSSI = rssi;
    memcpy(bestPacket, rxpacket, size + 1);
    newBestAvailable = true;
}
```

- 신호 세기(RSSI)가 더 센 메시지일 경우만 갱신
- `bestPacket`은 5초마다 OLED에 출력됨

---

### 🖥️ OLED 출력 예시

```
Best RX:
Hello world number 1.23
RSSI: -47
```

---

### ✅ 사용 보드

- Heltec WiFi LoRa 32 (V3)
- 915 MHz (미국 주파수 대역)
