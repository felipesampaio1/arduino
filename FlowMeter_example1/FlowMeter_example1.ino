byte sensorInterrupt = 3;  // 0 = digital pin 2
#define FlowMeterPulsePerSecondPerLiter 6.9
//#define FlowMeterPulsePerSecondPerLiter 7.5

volatile float PreviousPulseTime;
volatile float PulseDuration;
float LPM;
float averageLPM;
float CumulativeLPM;
float averageCount;
unsigned long litros = 0;
int LPMdecimal;

void setup() 
  {
  Serial.begin(115200);  
  PreviousPulseTime = 0;
  PulseDuration     = 0;
  LPM               = 0;
  averageLPM        = 0;
  averageCount      = 0;
  CumulativeLPM     = 0;
  LPMdecimal        = 2;
  litros            = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, RISING);
  }

void loop() 
  {

  CalculateFlow1();

  }

void CalculateFlow1()
  {
  CumulativeLPM = CumulativeLPM + (1000/(PulseDuration*FlowMeterPulsePerSecondPerLiter)); 
  averageCount++; 
  averageLPM = CumulativeLPM/averageCount;
  if (averageCount > 10000) 
    { 
    LPM = averageLPM;
    Serial.print("LPM: ");
    Serial.println(LPM,LPMdecimal);
    Serial.print("Cum.: ");
    Serial.println(CumulativeLPM,LPMdecimal);
    averageCount=0;   CumulativeLPM = 0; 
    }
  }
  
void pulseCounter()
  {
  PulseDuration = millis() - PreviousPulseTime;
  PreviousPulseTime = millis(); 
  }





  
