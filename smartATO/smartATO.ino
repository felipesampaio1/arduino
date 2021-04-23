#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>


// DEFINIÇÕES LCD
#define endereco  0x27 // Endereços comuns: 0x27, 0x3F
#define colunas   20
#define linhas    4

// PINOS SENSORES
const int LEDRED = 13;
const int LEDGREEN = 4;
const int LEDBLUE = 5;
const int SENSORNIVEL = A0;
const int SENSORTEMP = 10;

// CONSTANTES
const unsigned long  MIN = 60000;
const unsigned long HORA = 3600000;
const unsigned long DIA = 86400000;
const unsigned long LIMITE = 600;

// armazenará tempo (s) da ult. reposição
unsigned long tempoUltReposicao = 10000;
unsigned long horaUltEnchimento = 1;
bool travada = false;
bool alarmeBomba = false;

//limites de temp.
float limInf = 24.50;
float limSup = 30.00;

// Instanciando LCD
LiquidCrystal_I2C lcd(endereco, colunas, linhas);
//protocolo oneWire
OneWire oneWire(SENSORTEMP);
//Sensor de Temp.
DallasTemperature sensors(&oneWire);
//Armazena endereço Sensore
DeviceAddress sumpTerm, displayTerm;



void setup() {
  //Inicializacao Leitura Sensores
  pinMode (SENSORNIVEL, INPUT);
  pinMode (LEDRED, OUTPUT);
  
  //Inicializacao LCD
  lcd.init(); 
  lcd.backlight(); 
  lcd.clear(); 
  lcd.setCursor(0, 0);

  //Inicializacao Sensor TEMP
  Serial.begin(115200);
  sensors.begin();
  if (!sensors.getAddress(sumpTerm, 0)) 
      msgNormal("Sensor do SUMP Não Localizado!", 0, 3);
  //Setando Precisao do sensor
  sensors.setResolution(11);
  //Setando valores de alarm
  setAlarmTemp(limInf, limSup);
  sensors.setAlarmHandler(&alarm);

  

}



void loop() {
  //apaga LCD
  //lcd.noBacklight();
  
  // Leitura da TEMP
  sensors.requestTemperatures();
  impTemp(sensors.getTempC(sumpTerm), 14,0);
  sensors.processAlarms();
  

  //Se houver algum problema na reposicao, suspende.
  if (!alarmeBomba){
    // Leitura Reposição de Agua
    reposicaoAgua();
    alameAbastecimento();
  }
  
}

void alameAbastecimento(){
  if ((millis() - horaUltEnchimento) > DIA || travada){
    msgNormal("Alarme Bomba.", 0,2);    
    alarmeBomba = true;
  }
}


// Trata Reposicao de Agua
void reposicaoAgua(){
  unsigned long tempo = millis();
  bool encheu = false;
  while (analogRead(SENSORNIVEL) < LIMITE && !travada){
    //ativa bomba
    digitalWrite(LEDRED, HIGH);
    encheu = true;
    if ((millis() - tempo) / tempoUltReposicao >= 3){
      //desativa bomba
      digitalWrite(LEDRED, LOW);
      travada = !travada;
      msgNormal("                  ", 0, 2);
    }
   msgNormal("Enchendo!!!", 0, 2);
   //para estabilizar leitura do sensor
   delay(50);
  }
  if (encheu && !travada){
    //atualiza tempo de reposicao
    tempoUltReposicao = millis() - tempo;
    //atualiza a hora da ult. reposicao
    horaUltEnchimento = millis();
    digitalWrite(LEDRED, LOW);
    msgNormal("                  ", 0, 2);
  }
    
  //Verifica se sensor está alinhado corretamente e limpo
  if (analogRead(SENSORNIVEL) > 800) {
    msgNormal("Sensor Desalinhado", 0, 2);
    }
  else if (analogRead(SENSORNIVEL) > LIMITE) {
    msgNormal("                  ", 0, 2);
    }
      
}


//Trata Alarme de Temp
void alarm(DeviceAddress deviceAddress){
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC > sensors.getHighAlarmTemp(deviceAddress)) msgNormal("Alarme TEMP. alta", 0, 2);
  else if (tempC < sensors.getLowAlarmTemp(deviceAddress)) msgNormal("Alarme TEMP. baixa", 0, 2);
}


//Configura as temperaturas de Alarme
void setAlarmTemp(int limInf, int limSup){
    //Posso incluir um sensor para o SUMP e outro para DISPLAY
    sensors.getAddress(sumpTerm,0);
    sensors.setLowAlarmTemp(sumpTerm, limInf);
    sensors.setHighAlarmTemp(sumpTerm, limSup);
}


void msgNormal(String msg, int col, int row){
  lcd.setCursor(col, row);
  lcd.print(msg);  
}

void msgNormal(float msg, int col, int row){
  lcd.setCursor(col, row);  
  lcd.print(msg);  
}

void impTemp(float temp, int col, int row){
  lcd.setCursor(col, row);
  lcd.print(temp,1);
  lcd.setCursor(col+4, row);
  lcd.write(223);
  lcd.print("C");
}
