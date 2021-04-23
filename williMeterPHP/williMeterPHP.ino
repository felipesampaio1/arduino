/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# RFID MFRC522 / RC522 Library : https://github.com/miguelbalboa/rfid # 
#                                                                     # 
#                 Installation :                                      # 
# NodeMCU ESP8266/ESP12E    RFID MFRC522 / RC522                      #
#         D2       <---------->   SDA/SS                              #
#         D5       <---------->   SCK                                 #
#         D7       <---------->   MOSI                                #
#         D6       <---------->   MISO                                #
#         GND      <---------->   GND                                 #
#         D1       <---------->   RST                                 #
#         3V/3V3   <---------->   3.3V                                #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

//---------------------------------------- adiciona as library ---------------------------------------------------------------------------------------------------------------//
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

#define SS_PIN D2  //--> SDA / SS is connected to pinout D2
#define RST_PIN D1  //--> RST is connected to pinout D1
#define FLOWSENSOR D3 // Sensor de Fluxo
#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router
//conforme datasheet, Hz = 7.5*Q onde Q é o fluxo em L/M
#define calibrationFactor 7.5
//---------------------------------------- info do wifi -------------------------------------------------------------------------------------------------------------//
const char* ssid = "SAMPAIO1";
const char* password = "familiasampaio";
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

MFRC522 mfrc522(SS_PIN, RST_PIN);  //--> Create MFRC522 instance.
ESP8266WebServer server(80);  //--> Server on port 80

// armazena a quant. de pulsos.
volatile int frequencia = 0;
// variaveis para teste
float consumo;
float fluxoPorMinuto;
float fluxoPorSegundo;
int tempoLoop, tempoAberto;
bool solenoide;
bool enchendo;
byte readcard[4];
char str[32] = "";
String StrUID;

//-----------------------------------------------------------------------------------------------SETUP--------------------------------------------------------------------------------------//
void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC
  SPI.begin();      //--> Init SPI bus
  mfrc522.PCD_Init(); //--> Init MFRC522 card
  //Delay apenas por precaução para dar tempo dos sensores estabilizarem
  delay(500);

  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  
  pinMode(FLOWSENSOR, INPUT);
  pinMode(ON_Board_LED,OUTPUT); 
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  
  // adiciona a interrupção do sensor
  attachInterrupt(digitalPinToInterrupt(FLOWSENSOR), flow, RISING); // Setup Interrupt
  // para calculo de fluxo
  tempoLoop = millis();
  tempoAberto = millis();
  solenoide = false;
  enchendo = false;
  frequencia = 0;
  consumo = 0;
  fluxoPorMinuto = 0;
  fluxoPorSegundo = 0;
  //----------------------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Aproxime um cartão para liberar a torneira");
  Serial.println("");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------------LOOP---------------------------------------------------------------------------------------//
void loop() {
  
  //Quando obtiver leitura do RFID, entra nessa seção e envia para o front end.
  if(getid()) {  
    digitalWrite(ON_Board_LED, LOW);
    HTTPClient http;    //Declare object of class HTTPClient
 
    String UIDresultSend, postData;
    UIDresultSend = StrUID;
   
    //Post Data
    postData = "UIDresult=" + UIDresultSend;
  
    http.begin("http://10.0.1.12/williMeter/getUID.php");  
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
    
    Serial.println(UIDresultSend);
    Serial.println(http.POST(postData));   //Print HTTP return code
    //Get the response payload
    Serial.println(http.getString());    //Print request response payload  
    http.end();  //Close connection
    // esse delay precisa testar e ver se no caso de duas torneiras dará problema.
    delay(1000);
    digitalWrite(ON_Board_LED, HIGH);
    // precisa tratar, caso a tagID seja cadastrada e tenha saldo, liberar solenoide
    solenoide = true;
  }
  
/*  if (solenoide) {
    digitalWrite(ON_Board_LED, LOW);
    abreFecha();
  }*/
  calculaConsumo();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//---------------------------------------- Leitura do cartão ---------------------------------------------------------------------------------//
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
  mfrc522.PICC_HaltA();
  return 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//---------------------------------------- Converte o array lido em String ------------------------------------------------------------------------------//
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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

// -------------------- Função da interrupção do sensor quando houver passagem -------------------------------------------------------------------------------------------------------------//
ICACHE_RAM_ATTR void flow () 
{
   frequencia++;
}

// ------------------- Calcula o consumo caso tenha ---------------------------------------------------------------------------------------------------------------------------------------- //
void calculaConsumo(){
  int tempoAtual = millis() - tempoLoop;
  if (tempoAtual >= 1000) {
      tempoLoop = millis();
      if (frequencia != 0) {
        //retira a interrupção no momento de calcular para evitar saída do método e possível incremento na frequencia.
        detachInterrupt(digitalPinToInterrupt(FLOWSENSOR));
        //Pode ser que a rotina nao execute exatamente a cada segundo entao faz a compensação.
        fluxoPorMinuto = ((1000 / tempoAtual) * frequencia) / calibrationFactor;
        fluxoPorSegundo = fluxoPorMinuto / 60;
        consumo += fluxoPorSegundo;
        //prints para teste
        Serial.print("Hz: ");
        Serial.println(frequencia);
        Serial.print("l/m: ");
        Serial.println(fluxoPorMinuto);
        Serial.print("l/s: ");
        Serial.println(fluxoPorSegundo);
        Serial.print("Consumo: ");
        Serial.println(consumo);
        frequencia = 0;
        // abateSaldo();
        attachInterrupt(digitalPinToInterrupt(FLOWSENSOR), flow, RISING);
        //enchendo = true;
      }
     // else enchendo = false;
  }
}


// ------------------- Caso passe 3 segungos sem fluxo fecha solenoide ------------------------------------------------------------------------------------------------------------------------ //
/* void abreFecha(){
  int tempoAtual = millis() - tempoAberto;
  if (tempoAtual >= 3000){
    tempoAberto = millis();
    if (!enchendo){
        solenoide = false;
        //fecha solenoide
        digitalWrite(ON_Board_LED, HIGH);
    }
  }
}*/
