#include <Servo.h>

// Arduino pin assignment
#define PIN_IR    A0        // IR sensor at Pin A0
#define PIN_LED   9
#define PIN_SERVO 10

// Servo duty range (in microseconds)
#define _DUTY_MIN 1000      // servo full clock-wise (0 degree)
#define _DUTY_NEU 1500      // servo neutral (90 degree)
#define _DUTY_MAX 2000      // servo full counter-clockwise (180 degree)

// Distance range (in mm)
#define _DIST_MIN 100.0     // 10 cm
#define _DIST_MAX 250.0     // 25 cm

// EMA filter parameters
#define EMA_ALPHA 0.2       // smoothing factor (0~1)

// Loop interval
#define LOOP_INTERVAL 20    // msec

Servo myservo;
unsigned long last_loop_time = 0;   // unit: msec

float dist_prev = _DIST_MIN;
float dist_ema  = _DIST_MIN;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);
  
  Serial.begin(1000000);    // 1,000,000 bps
}

void loop()
{
  unsigned long time_curr = millis();
  if (time_curr < (last_loop_time + LOOP_INTERVAL)) return;
  last_loop_time = time_curr;

  int duty;
  float a_value, dist_raw;

  // --- Read IR Sensor ---
  a_value = analogRead(PIN_IR);
  dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0;

  // --- Range filter (10cm~25cm only) ---
  if (dist_raw < _DIST_MIN) dist_raw = _DIST_MIN;
  if (dist_raw > _DIST_MAX) dist_raw = _DIST_MAX;

  // LED control
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX)
    digitalWrite(PIN_LED, HIGH);
  else
    digitalWrite(PIN_LED, LOW);

  // --- EMA filter ---
  dist_ema = (EMA_ALPHA * dist_raw) + ((1 - EMA_ALPHA) * dist_prev);
  dist_prev = dist_ema;

  // --- Linear mapping without map() ---
  // duty = _DUTY_MIN + (dist_ema - _DIST_MIN) * ( (_DUTY_MAX - _DUTY_MIN) / (_DIST_MAX - _DIST_MIN) )
  duty = _DUTY_MIN + (int)((dist_ema - _DIST_MIN) * ((_DUTY_MAX - _DUTY_MIN) / (_DIST_MAX - _DIST_MIN)));

  // --- Servo control ---
  myservo.writeMicroseconds(duty);

  // --- Serial output for monitoring ---
  Serial.print("_DUTY_MIN:");  Serial.print(_DUTY_MIN);
  Serial.print(",_DIST_MIN:"); Serial.print(_DIST_MIN);
  Serial.print(",IR:");        Serial.print(a_value);
  Serial.print(",dist_raw:");  Serial.print(dist_raw);
  Serial.print(",ema:");       Serial.print(dist_ema);
  Serial.print(",servo:");     Serial.print(duty);
  Serial.print(",_DIST_MAX:"); Serial.print(_DIST_MAX);
  Serial.print(",_DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println();
}
