#define FLOWSENSOR D3

// armazena a quant. de pulsos.
volatile int frequencia;
// Calculated litres/hour
float consumo = 0;
float fluxoPorMinuto = 0;
float fluxoPorSegundo = 0;
//conforme datasheet
float calibrationFactor = 7.5;
unsigned long tempoLoop;

void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC
  delay(500);
  Serial.println("Iniciou");
  pinMode(FLOWSENSOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(FLOWSENSOR), flow, RISING); // Setup Interrupt
  tempoLoop = millis();
}

void loop(){
   calculaConsumo();
}


// ------------------- //
void calculaConsumo(){
  int tempoAtual = millis() - tempoLoop;
  if (tempoAtual >= 1000) {
      tempoLoop = millis();
      if (frequencia != 0) {
        detachInterrupt(digitalPinToInterrupt(FLOWSENSOR));
        //Pode ser que a rotina nao execute exatamente a cada segundo entao faz a compensação.
        fluxoPorMinuto = ((1000 / tempoAtual) * frequencia) / calibrationFactor;
        fluxoPorSegundo = fluxoPorMinuto / 60;
        consumo += fluxoPorSegundo;
        Serial.print("Hz: ");
        Serial.println(frequencia);
        Serial.print("l/m: ");
        Serial.println(fluxoPorMinuto,3);
        Serial.print("l/s: ");
        Serial.println(fluxoPorSegundo,3);
        Serial.print("Consumo: ");
        Serial.println(consumo, 3);
        frequencia = 0;
        attachInterrupt(digitalPinToInterrupt(FLOWSENSOR), flow, RISING);
      }
  }
}


ICACHE_RAM_ATTR void flow() 
{
   frequencia++;
}
