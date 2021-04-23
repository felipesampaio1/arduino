#include <LiquidCrystal_I2C.h>

// DEFINIÇÕES
#define endereco  0x27 // Endereços comuns: 0x27, 0x3F
#define colunas   20
#define linhas    4

// INSTANCIANDO OBJETOS
LiquidCrystal_I2C lcd(endereco, colunas, linhas);

void setup() {
   lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY
  lcd.setCursor(0, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA E LINHA
}

void loop() {
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 19; j++){
      lcd.setCursor(j, i);
      lcd.print ("F");
      delay(100);
      lcd.clear();
    }
  }

}
