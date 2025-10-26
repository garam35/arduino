#include <Servo.h>

// Arduino pin assignment
#define PIN_LED   9   // LED active-low
#define PIN_TRIG  12  // sonar sensor TRIGGER
#define PIN_ECHO  13  // sonar sensor ECHO
#define PIN_SERVO 10  // servo motor

// configurable parameters for sonar
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 180.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 360.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficient to convert duration to distance

#define _EMA_ALPHA 0.3    // EMA weight of new sample (range: 0 to 1)

// servo parameters
#define _DUTY_MIN 1000 // servo full clockwise position (0 degree)
#define _DUTY_NEU 1500 // servo neutral position (90 degree)
#define _DUTY_MAX 2000 // servo full counterclockwise position (180 degree)

// global variables
float dist_raw = 0.0;
float dist_prev = _DIST_MAX;
float dist_ema = _DIST_MAX;
unsigned long last_sampling_time = 0;

Servo myservo;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);

  Serial.begin(57600);
  dist_prev = _DIST_MAX;
}

void loop() {
  float dist_filtered;

  // wait until next sampling time
  if (millis() < last_sampling_time + INTERVAL)
    return;

  // measure distance
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // 범위 필터 (18~36cm = 180~360mm)
  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX) || (dist_raw == 0.0)) {
    dist_filtered = dist_prev; // 이전값 유지
  } else {
    dist_filtered = dist_raw;  // 정상범위면 업데이트
    dist_prev = dist_raw;
  }

  // EMA 필터 적용
  dist_ema = _EMA_ALPHA * dist_filtered + (1 - _EMA_ALPHA) * dist_ema;

  // LED 제어: 범위 내일 때 ON
  if (dist_filtered >= _DIST_MIN && dist_filtered <= _DIST_MAX)
    digitalWrite(PIN_LED, HIGH);
  else
    digitalWrite(PIN_LED, LOW);

  // 거리(18~36cm)에 따른 서보 각도 제어
  int angle;
  if (dist_ema <= _DIST_MIN) {
    angle = 0;
  } else if (dist_ema >= _DIST_MAX) {
    angle = 180;
  } else {
    // 18~36cm → 0°~180° 비례 변환
    angle = map((int)dist_ema, (int)_DIST_MIN, (int)_DIST_MAX, 0, 180);
  }

  myservo.write(angle);

  // 시리얼 출력 (플로터용)
  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",dist:");  Serial.print(dist_raw);
  Serial.print(",ema:");   Serial.print(dist_ema);
  Serial.print(",Servo:"); Serial.print(myservo.read());
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.println("");

  // update time
  last_sampling_time = millis();
}

// 초음파 거리 측정 함수 (mm 단위)
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
