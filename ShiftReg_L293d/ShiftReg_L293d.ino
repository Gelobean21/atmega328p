byte clock = A0;
byte latch = A1;
byte input = A2;
byte enA = 3;
byte enB = 5;
byte enC = 6;
byte enD = 9;

byte output[] = {85,170,102,153};
//int output[] = {63,6,91,79,102,109,125,7,127,103};

void setup() {
  Serial.begin(115200);
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(input, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(enC, OUTPUT);
  pinMode(enD, OUTPUT);
  digitalWrite(clock, LOW);
  digitalWrite(latch, LOW);
  digitalWrite(input, LOW);
}

void loop() {
    Serial.println("FORWARD");
    ControlSpeed(255);
    Drive(output[0]);
    delay(5000);
    Serial.println("BACKWARD");
   
    Drive(output[1]);
    delay(5000);
    Serial.println("RIGHT");
   
    Drive(output[3]);
    delay(5000);
    Serial.println("LEFT");
    
    Drive(output[2]);
    delay(5000);
}

void Drive(int pinum){
  digitalWrite(latch, LOW);
  for(int i = 0; i < 8 ; i++){ //8 clock bits
    digitalWrite(input, !!(pinum & (1 << (7 - i))));
    digitalWrite(clock, LOW);
    digitalWrite(clock, HIGH);
  }
  digitalWrite(latch, HIGH);
}

void ControlSpeed(byte speed)
{
  analogWrite(enA, speed);
  analogWrite(enB, speed);
  analogWrite(enC, speed);
  analogWrite(enD, speed);
}
