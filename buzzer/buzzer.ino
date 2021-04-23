//MODIFICAR O VALOR DA CONSTANTE PARA VARIAR O TEMPO DAS BADALADAS CONFORME DESEJADO EM MILISEGUNDOS
const unsigned long TEMPO = 10000;
int unsigned long aux = 0;
int badalada = 0;


void setup() {
  pinMode(5, OUTPUT);
  // put your setup code here, to run once:
}

void loop() {
    if (contaTempo()){
        tocaSino(++badalada);
        if (badalada == 12) 
            badalada = 0;
    }
}


void tocaSino(int badalada){
  for (int i=0; i < badalada; i++){
    digitalWrite(5, HIGH);
    delay(100);
    digitalWrite(5, LOW);
    delay(200);
  }
}


bool contaTempo(){
  if (millis() - aux > TEMPO){
    aux = millis();
    return true;
  }
  else return false;
}
