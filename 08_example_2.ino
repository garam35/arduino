// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // convert duration to distance (mm)

unsigned long last_sampling_time;   // unit: msec

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  
  pinMode(PIN_ECHO, INPUT);   
  digitalWrite(PIN_TRIG, LOW);  
  
  Serial.begin(57600);
}

void loop() { 
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  float distance = USS_measure(PIN_TRIG, PIN_ECHO); // mm
  int brightness = 255; // 기본적으로 꺼짐

  if (distance > 100 && distance <= 200) {
    // 100mm ~ 200mm: 밝기 증가 (255 → 0)
    brightness = (int)(255.0 * (200 - distance) / 100.0);
  } 
  else if (distance > 200 && distance < 300) {
    // 200mm ~ 300mm: 밝기 감소 (0 → 255)
    brightness = (int)(255.0 * (distance - 200) / 100.0);
  } 
  else if (distance == 200) {
    brightness = 0;  // 최대 밝기
  } 
  else if (distance == 150 || distance == 250) {
    brightness = 127;  // 50% 밝기
  }

  analogWrite(PIN_LED, brightness); // Active Low

  // 디버깅용 출력
  Serial.print("Distance(mm): "); Serial.print(distance);
  Serial.print(" -> Brightness: "); Serial.println(brightness);

  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
