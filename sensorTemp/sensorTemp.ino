// INCLUSÃO DE BIBLIOTECAS
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// DEFINIÇÕES
#define endereco  0x27 // Endereços comuns: 0x27, 0x3F
#define colunas   20
#define linhas    4

// INSTANCIANDO OBJETOS
LiquidCrystal_I2C lcd(endereco, colunas, linhas);
//protocolo oneWire
OneWire  oneWire(10);
//Sensores
DallasTemperature sensors(&oneWire);
//Variaves
//armaenza o serial de cada sensor, futuramento colocarei 2
DeviceAddress ds18;
//Numero de sensores
byte c;

void sensorID(DeviceAddress sensor){
  //exibe id do sensor
  for (uint8_t i = 0; i < 8; i++){
    if (sensor[i] < 16)
      lcd.print("0");
    lcd.print(sensor[i], HEX);
  }
}

void alarm(uint8_t sensor){
  lcd.print("Alarme Sensor 1 ");
  sensorID(sensor);
  delay(500);
  lcd.clear();
}

void setup() {
  lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY
  lcd.setCursor(0, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA E LINHA 

  Serial.begin(115200);

  sensors.begin();
  sensors.setResolution(12);
  sensors.setAlarmHandler(&alarm);

  c = sensors.getDeviceCount();
  lcd.print("Sens. Ident: ");
  lcd.print(c);
  

  //define os alarmes inferiores e superiores
  for (byte i = 0; i < c; i++){
    sensors.getAddress(ds18,i);
    sensors.setLowAlarmTemp(ds18, 24.5);
    sensors.setHighAlarmTemp(ds18, 29.5);
    
  }
  
  //lcd.noBacklight(); // DESLIGA A ILUMINAÇÃO DO DISPLAY
  //lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
}




void loop() {
  //Solicita temp. dos sensores
  sensors.requestTemperatures();
  lcd.setCursor(1, 2);
  lcd.print("DAVI B. SAMPAIO");
  lcd.setCursor(1, 3);
  lcd.print("Temp: ");
  
  lcd.print(sensors.getTempC(ds18));

 sensors.processAlarms();
 delay(400);

}
