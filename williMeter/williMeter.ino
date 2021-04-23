#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


void setup() {
  Serial.begin(9600);    // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

class Cliente {
  public:
  String nome, cpf, tagRFID;
  float saldo;
  Cliente (String s1, String s2){
    nome = s1;
    cpf = s2;
    saldo = 0;
    tagRFID = "";
  }

  float getSaldo(){
    return saldo;
  }

  void setSaldo(float s){
    saldo = s;
  }
};

class Bico{
 public:
  String nome;
  int solenoide, medidorFluxo;
  Bico (String nome, int solenoide, int medidorFluxo){
    pinMode(solenoide, OUTPUT);
    pinMode(medidorFluxo, INPUT);
  }
};
