int x;
void setup() {
 Serial.begin(115200);
 Serial.setTimeout(1);
 pinMode(10, OUTPUT);
 pinMode(12, OUTPUT);

}
void loop() {
 while (!Serial.available());
 x = Serial.readString().toInt();
 if(x == 1)
 {
  digitalWrite(10, HIGH);
 }
 if(x == 2)
 {
  digitalWrite(12, HIGH);
 }
 if(x == 3)
 {
  digitalWrite(10, LOW);
 }
 if(x == 4)
 {
  digitalWrite(12, LOW);
 } 
}
