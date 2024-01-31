#define LED_1 2
#define LED_2 3
#define LED_3 4

void setup() {
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
  Serial.begin(9600);
}

void loop() {
  while(Serial.available())
  {
    switch(Serial.read()){
      case '1': 
      digitalWrite(LED_1, HIGH); 
      digitalWrite(LED_2, LOW);
      digitalWrite(LED_3, LOW);
      break;
      case '2': 
      digitalWrite(LED_1, LOW); 
      digitalWrite(LED_2, HIGH);
      digitalWrite(LED_3, LOW);
      break;
      case '3':
      digitalWrite(LED_1, LOW); 
      digitalWrite(LED_2, LOW);
      digitalWrite(LED_3, HIGH);
      break;
      default : break;
    }
  }
}
