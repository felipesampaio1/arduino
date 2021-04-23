#include <ThreeWire.h> 
#include <RtcDS1302.h> 

//Pinos que estão conectados o módulo RTc
ThreeWire myWire(4, 5, 2); 
RtcDS1302<ThreeWire> Rtc(myWire);

//Pino que está conectado o Buzz
const int pinoBuzzer = 9;

typedef struct { 
  String hora;
  int quantidade;
  int tipoSom;
} dicionarioHoras;

const dicionarioHoras dicionarioHorasArray[] {
  {"00:00:00",12,1},
  {"01:00:00",1,1},
  {"02:00:00",2,1},
  {"03:00:00",3,1},
  {"04:00:00",4,1},
  {"05:00:00",5,1},
  {"06:00:00",6,1},
  {"07:00:00",7,1},
  {"08:00:00",8,1},
  {"09:00:00",9,1},
  {"10:00:00",10,1},
  {"11:00:00",11,1},
  {"12:00:00",12,1},
  {"13:00:00",1,1},
  {"14:00:00",2,1},
  {"15:00:00",3,1},
  {"16:00:00",4,1},
  {"17:00:00",5,1},
  {"18:00:00",6,1},
  {"19:00:00",7,1},
  {"20:00:00",8,1},
  {"21:00:00",9,1},
  {"22:00:00",10,1},
  {"23:00:00",11,1},
  {"18:58:00",6,1}
};


/*
 * Variáveis
 */
boolean debug = true;
char horaAtualString[20]; 
int tamanhoHorasString = sizeof(horaAtualString) / sizeof(horaAtualString[0]); 

void setup () {
  
    Serial.begin(9600); 
    Rtc.Begin(); 

    RtcDateTime dataDoSistema = RtcDateTime(__DATE__, __TIME__);
    imprimir("Data do sistema durante a compilação: "); 
    printDateTime(dataDoSistema);
    imprimir("");

    if(Rtc.GetIsWriteProtected()){ 
        imprimir("Habilitando a gravação do RTC...");
        Rtc.SetIsWriteProtected(false); 
    }

    if(!Rtc.GetIsRunning()){ 
        imprimir("Iniciando RTC..."); 
        Rtc.SetIsRunning(true); 
    }

    RtcDateTime dataAtual = Rtc.GetDateTime(); 
    if (dataAtual < dataDoSistema) { 
      
        imprimir("Atualizando módulo RTC com a data e hora do sistema durante a compilação."); 
        Rtc.SetDateTime(dataDoSistema); 
        printDateTime(Rtc.GetDateTime());        
        
    }

    pinMode(pinoBuzzer, OUTPUT);  
    bipar(1);  
    
}

void loop () {
    
    RtcDateTime dt = Rtc.GetDateTime(); 

    snprintf_P(horaAtualString, 
              tamanhoHorasString,
              PSTR("%02u:%02u:%02u"),
              dt.Hour(), //HORA
              dt.Minute(), //MINUTOS
              dt.Second() ); //SEGUNDOS   
    
    imprimir(String(horaAtualString));
    
    for(int i = 0; i < sizeof(dicionarioHorasArray)/sizeof(dicionarioHoras); ++i) {
      
      if(String(horaAtualString) == dicionarioHorasArray[i].hora){
        imprimir("Bitando " + String(dicionarioHorasArray[i].quantidade) + " vezes.");
        bipar(dicionarioHorasArray[i].quantidade);
      }    
      
    }
    
    delay(1000); 
    
}

void bipar(int q){
  
  for (int i = 0; i < q; i++) {

    tone(pinoBuzzer, 500, 1000);
    delay(2000);
    noTone(pinoBuzzer);

    imprimir("Bit: " + String(i));
        
  }    
  
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt){
  
    char datestring[20]; 

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Day(), //DIA
            dt.Month(), //MÊS
            dt.Year(), //ANO
            dt.Hour(), //HORA
            dt.Minute(), //MINUTOS
            dt.Second() ); //SEGUNDOS
            
    Serial.print(datestring); 
    
}

void imprimir(String msg){
  if(debug){
    Serial.println(msg);
  }
}
