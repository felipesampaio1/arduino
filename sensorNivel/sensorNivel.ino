// the setup function runs once when you press reset or power the board
const int ledred = 3;
const int ledgreen = 10;
const int floatsensor = 7;

void setup() {
 
  // initialize digital pin 13 as an output.
  //pinMode(ledred, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  pinMode(floatsensor, INPUT);
  //Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  
  if (digitalRead(floatsensor) == HIGH) {
      digitalWrite(ledgreen, HIGH);
  }else {
      digitalWrite(ledgreen, LOW);
  }
  
  }
  
 
/* 
  digitalWrite(3, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(6, LOW);   // turn the LED on (HIGH is the voltage level)  
  delay(1000);              // wait for a second
  digitalWrite(3, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(6, HIGH);     // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
  
 */

