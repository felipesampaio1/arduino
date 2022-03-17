#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// DEFINIÇÕES LCD
#define endereco  0x27 // Endereços comuns: 0x27, 0x3F
#define colunas   20
#define linhas    4

const int ECHO = 10;
const int TRIG = 9;
const int SENSORLEVEL = 8;
const int BOMBA = 12;
const int SOLENOIDE = 11;
const int SENSORTEMP = 6;
const int BUZZER = 2;

float distancia = 0;
float temp = 0;
float volume = 0; ;

unsigned long MINUTO = 60000;
unsigned long CINCOMIN = 300000;
unsigned long HORA = 3600000;
unsigned long DIA = 86400000;
unsigned long leituraAnteriorEnchendo, leituraAnteriorLevel, leituraAnteriorUltra, horaUltEnchimento, tempoUltRep, leituraAnteriorTemp;
boolean enchendo, encheu, travada;

// Instanciando LCD
LiquidCrystal_I2C lcd(endereco, colunas, linhas);

OneWire oneWire(SENSORTEMP);
//Sensor de Temp.
DallasTemperature sensors(&oneWire);
//Armazena endereço Sensore
DeviceAddress sumpTerm;


void setup() {
  
  //Ultrasonic
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  //Reles
  pinMode(BOMBA, OUTPUT);
  pinMode(SOLENOIDE, OUTPUT);
  //Level
  pinMode(SENSORLEVEL, INPUT_PULLUP);
  //Buzzer
  //pinMode(BUZZER, OUTPUT);
   
  digitalWrite(BOMBA, HIGH);
  digitalWrite(SOLENOIDE, HIGH);
  //digitalWrite(BUZZER, LOW);

  sensors.begin();

  //Setando Precisao do sensor
  sensors.setResolution(11);

  travada = false;
  encheu = false;
  enchendo = false;
  tempoUltRep = 15000;
  splash();
  distancia = sensorUltra(TRIG, ECHO);
  calculaVolume();
  tela();
  if (!sensors.getAddress(sumpTerm, 0)) {
      lcd.setCursor(15, 0);
      lcd.print("N/A");
      //buzzer();
  }
}

// the loop function runs over and over again forever
void loop() {
  reposicao();
  filtroRO();
  imprimeValores();
  getTemperatura();
}


void getTemperatura(){
  if (millis() - leituraAnteriorTemp >= 5000){
    leituraAnteriorTemp = millis();
    sensors.requestTemperatures();
    temp = sensors.getTempC(sumpTerm);
  }
}

void filtroRO(){
   if (millis() - leituraAnteriorUltra >= CINCOMIN) {
    distancia = sensorUltra(TRIG, ECHO);
    calculaVolume();
    leituraAnteriorUltra = millis();
    if (distancia >= 20 && !enchendo){
      enchendo = true;
      digitalWrite(SOLENOIDE, LOW);
    }
  } 
  if (enchendo)
     if (millis() - leituraAnteriorEnchendo >= CINCOMIN){
         distancia = sensorUltra(TRIG, ECHO);
         if (distancia <= 3){
            enchendo = false;
            digitalWrite(SOLENOIDE, HIGH);
         }  
     }      
}

void reposicao(){
    if (millis() - leituraAnteriorLevel >= MINUTO){
     leituraAnteriorLevel = millis();
     while (digitalRead(SENSORLEVEL) == LOW && !travada) {
       if (((millis() - leituraAnteriorLevel) / tempoUltRep) >= 6){
          digitalWrite(BOMBA, HIGH);
          travada = true;
          break;
       }
       digitalWrite(BOMBA, LOW);
       delay(1000);
       encheu = true;
     }
     if (encheu){
        tempoUltRep = millis() - leituraAnteriorLevel;
        horaUltEnchimento = millis();
        encheu = false;
        digitalWrite(BOMBA, HIGH);
     }   
     alameAbastecimento();
   }
}  


void alameAbastecimento(){
  if ((millis() - horaUltEnchimento) > DIA){
      lcd.setCursor(10, 2);
      lcd.print("FALTA ABS");
  }
  if (travada){
      lcd.setCursor(12, 2);
      lcd.print("TRAVADA");
  }
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

void imprimeValores(){
  lcd.setCursor(8, 0);
  lcd.print(tempoUltRep/1000);
  lcd.print("s ");
  lcd.setCursor(10, 1);
  if (enchendo){
    lcd.print("ON ");
    lcd.backlight();
  }
  else lcd.print("OFF");
  if (travada)
    lcd.backlight();
    
  lcd.setCursor(9, 2);
  lcd.print(volume,0);
  lcd.print("l");
  lcd.setCursor(9, 3);
  lcd.print(((millis()-horaUltEnchimento)/1000)/60);
  lcd.print("min ");
  lcd.setCursor(14, 0);
  lcd.print(temp,1);
  lcd.setCursor(18, 0);
  lcd.write(223);
  lcd.print("C");
}

void tela(){
  lcd.setCursor(0, 0);
  lcd.print("T. Repo:");
  lcd.setCursor(0, 1);
  lcd.print("Filtro RO:");
  lcd.setCursor(0, 2);
  lcd.print("Nivel RO:");
  lcd.setCursor(0, 3);
  lcd.print("Ult. Rep:");  
}

void splash(){
  //Inicializacao LCD
  lcd.init(); 
  lcd.backlight(); 
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print("####################");
  lcd.setCursor(0, 1);
  lcd.print("##");
  lcd.setCursor(0, 2);
  lcd.print("##");
  lcd.setCursor(0, 3);
  lcd.print("####################");
  lcd.setCursor(18, 1);
  lcd.print("##");
  lcd.setCursor(18, 2);
  lcd.print("##");
  lcd.setCursor(4, 1);
  lcd.print("MEU AQUARIO");
  lcd.setCursor(7, 2);
  lcd.print("V1.0");
  delay(2000);
  lcd.clear();
  lcd.noBacklight();
}


void calculaVolume(){
  volume = (1256 * (50 - distancia)) / 1000;
}

/*
void buzzer(){
  for (int i=0; i < 3; i++){
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }
}*/
