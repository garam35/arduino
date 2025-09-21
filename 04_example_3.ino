#define PIN_LED 13      
unsigned int count = 0; 
int toggle = 0;        

void setup() {
  pinMode(PIN_LED, OUTPUT);      
  Serial.begin(115200);          
  Serial.println("Hello World!");
  digitalWrite(PIN_LED, LOW);    
}

void loop() {
  count++;                                     
  Serial.println(count);                      
  toggle = toggle_state(toggle);              
  digitalWrite(PIN_LED, toggle);               
  delay(1000);                                 
}

// LED 상태를 반전시키는 함수
int toggle_state(int toggle) {
  return !toggle;   // 현재 상태의 반대값 반환
}
