#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define FLOWSENSOR 2
#define PINSOLENOIDE 4
#define SS_PIN 10  //--> SDA / SS conectado no pino 10.
#define RST_PIN 9  //--> RST conectado no pino 9
#define ENDERECO 0x27 // Endereços comuns: 0x27, 0x3F
#define COL 20
#define LIN 4

//cria instancia do mfrc
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(ENDERECO, COL, LIN);

// armazena a quant. de pulsos.
volatile float frequencia;
// Calculated litres/hour
float consumo = 0;
float fluxoPorMinuto = 0;
float fluxoPorSegundo = 0;
//conforme datasheet
float calibrationFactor = 7.5;
float tempoLoop;
unsigned long tempoAberto;
float preco = 33.34, saldo = 50;
bool solenoide, enchendo;
byte readcard[4];
char str[32] = "";
String StrUID;

void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC
  SPI.begin();
  mfrc522.PCD_Init(); //--> Init MFRC522 card
  delay(10);
  mfrc522.PCD_DumpVersionToSerial();
  lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY
  msg("Hz: ",0,0);
  msg("l/s: ",0,1);
  msg("Cons: ",0,2);
  msg("Saldo", 14,2);
  msg((String)saldo, 14,3);
  pinMode(FLOWSENSOR, INPUT_PULLUP);
  pinMode(PINSOLENOIDE, OUTPUT);
  digitalWrite(FLOWSENSOR, LOW);
  digitalWrite(PINSOLENOIDE, LOW);
  
  attachInterrupt(digitalPinToInterrupt(FLOWSENSOR), flow, RISING); // Setup Interrupt
  tempoLoop = millis();
  solenoide = false;
  enchendo = false;
}

void loop(){
   if(getid()){
    solenoide = true;
    tempoAberto = millis();
    digitalWrite(PINSOLENOIDE, HIGH);
    delay(200);
   }
   if (solenoide)  
      fechaSolenoide();
   calculaConsumo();
}

// ------------------- //
void calculaConsumo(){
  float tempoAtual = millis() - tempoLoop;
  if (tempoAtual >= 200) {
      //msg((String)tempoAtual,12,1);
      msg((String)frequencia,4,0);
      tempoLoop = millis();
      if (frequencia != 0 && solenoide) {
        enchendo = true;
        detachInterrupt(digitalPinToInterrupt(FLOWSENSOR));
        //Pode ser que a rotina nao execute exatamente a cada segundo entao faz a compensação.
        fluxoPorSegundo = ((200 / tempoAtual) * frequencia) / 2250;
        //fluxo = (((200/tempoAtual) * frequencia) / calibrationFactor) / 60 ) / 5;
        //fluxoPorSegundo = fluxoPorMinuto / 60;
        //debita consumo do saldo do cliente
        atualizaSaldo(fluxoPorSegundo);
        fluxoPorSegundo *= 1000;
        consumo += fluxoPorSegundo * 5;
        msg((String)frequencia,4,0);
        msg((String)(fluxoPorSegundo * 5),5,1);
        msg((String)consumo,6,2);
        
        /*Serial.print("Hz: ");
        Serial.println(frequencia);
        Serial.print("l/m: ");
        Serial.println(fluxoPorMinuto,3);
        Serial.print("l/s: ");
        Serial.println(fluxoPorSegundo,3);
        Serial.print("Consumo: ");
        Serial.println(consumo, 3);*/
        attachInterrupt(digitalPinToInterrupt(FLOWSENSOR), flow, RISING);
     }else if (frequencia == 0) enchendo = false;
     frequencia = 0; 
  }
}

void atualizaSaldo(float fluxo){
  saldo -= fluxo * preco;
  msg((String)saldo, 14,3);
}


void fechaSolenoide(){
  float tempo = millis() - tempoAberto;
  if (tempo >= 3000){
     //msg((String)tempo, 12,0);
     tempoAberto = millis();
     if (!enchendo && solenoide){
        solenoide = false;
        //fecha solenoide
        digitalWrite(PINSOLENOIDE, LOW);     
        //Zera o consumo para o proximo pedido
        consumo = 0;
     }
  }  
}

void flow()
{
   frequencia++;
}


int getid() {  
  if(!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
  Serial.print("A tag ID lida é : "); 
  for(int i=0;i<4;i++){
    readcard[i]=mfrc522.uid.uidByte[i]; //armazena a tagID no array
    array_to_string(readcard, 4, str);
    StrUID = str;
  }
  //msg("TagID",10,0);  
  //msg(StrUID,10,1);
  Serial.println(StrUID);
  mfrc522.PICC_HaltA();
  return 1;
}


void array_to_string(byte array[], int len, char buffer[]) {
    for (int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

void msg(String msg, int col, int lin){
    lcd.setCursor(col, lin);
    lcd.print(msg);
    Serial.println(msg);
}
