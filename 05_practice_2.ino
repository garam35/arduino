#define LED_PIN 7

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // 1. 처음 1초 동안 LED 켜기 (LOW가 켜짐)
  digitalWrite(LED_PIN, LOW);
  delay(1000);

  // 2. 다음 1초 동안 LED 5회 깜빡이기
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, LOW);   // 켜기
    delay(100);
    digitalWrite(LED_PIN, HIGH);  // 끄기
    delay(100);
  }

  // 3. LED 끄고 무한루프
  digitalWrite(LED_PIN, HIGH);  // 꺼진 상태 유지
  for (;;) {
    // 무한루프: LED는 꺼진 채 유지
  }
}
