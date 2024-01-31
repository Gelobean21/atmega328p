int clock = 7;
int latch = 8;
int input = 13;

byte perseg = 6;
byte secseg = 12;
byte thirseg = 5;

int output[] = {63,6,91,79,102,109,125,7,127,103};

void setup()
{
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(input, OUTPUT);
  Serial.begin(115200);
  digitalWrite(perseg, HIGH);
  digitalWrite(secseg, LOW);
  digitalWrite(thirseg, HIGH);
}

void loop()
{
  for(int led = 0 ; led < 10 ; led++){
    ShitOut(input, clock, latch, output[led]);
    Serial.println(led + 1);
  }
}

void ShitOut(int in, int cl, int lh,int pinum){
  digitalWrite(lh, LOW);
  for(int i = 0; i < 8 ; i++){ //8 clock bits
    digitalWrite(in, !!(pinum & (1 << (7 - i))));
    digitalWrite(cl, LOW);
    digitalWrite(cl, HIGH);
  }
  
  digitalWrite(lh, HIGH);
  delay(500);
}
