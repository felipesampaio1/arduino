// the setup function runs once when you press reset or power the board
const int ledred = 3;
const int ledgreen = 6;

const int echo = 10;
const int trig = 9;

void setup() {
 
  // initialize digital pin 13 as an output.
  pinMode(ledred, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  
  //ultrasonic
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
 
  int distancia = sensorUltra(trig, echo);
  
  Serial.print("distancia em CM: ");
  Serial.println(distancia);
  Serial.println();
  delay(200); 
 
/* 
  digitalWrite(3, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(6, LOW);   // turn the LED on (HIGH is the voltage level)  
  delay(1000);              // wait for a second
  digitalWrite(3, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(6, HIGH);     // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
  
 */
}

//metodo para calculo da distancia
int sensorUltra(int trig, int echo){
 //informacoes coletadas no datasheet do modulo
 digitalWrite(trig, LOW);
 delayMicroseconds(2);
 digitalWrite(trig, HIGH);
 delayMicroseconds(10); 
 digitalWrite(trig, LOW);
 //divide por 58 para centimetros
 return pulseIn(echo, HIGH)/58;
}
