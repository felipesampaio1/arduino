#include <LiquidCrystal_I2C.h>

// DEFINIÇÕES
#define endereco  0x27 // Endereços comuns: 0x27, 0x3F
#define colunas   20
#define linhas    4

// INSTANCIANDO OBJETOS
LiquidCrystal_I2C lcd(endereco, colunas, linhas);




byte bignums[10][2][3] = {
    {
        {255, 0, 255},
        {255, 80, 255}
    }, {
        {0, 255, 254},
        {1, 255, 1}
    }, {
        {2, 2, 255},
        {255, 1, 1}
    }, {
        {0, 2, 255},
        {1, 1, 255}
    }, {
        {255, 1, 255},
        {254, 254, 255}
    }, {
        {255, 2, 2},
        {1, 1, 255}
    }, {
        {255, 2, 2},
        {255, 1, 255}
    }, {
        {0, 0, 255},
        {254, 255, 254}
    }, {
        {255, 2, 255},
        {255, 1, 255}
    }, {
        {255, 2, 255},
        {254, 254, 255}
    }
};

/*
void loadchars() {
    for (int i = 0; i < 8; i++)
        lcd.createChar(i, custchar[i]);
    
/*    
    lcd.command(64);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            lcd.write(custchar[j]);

    lcd.home();
}*/

void printbigchar(byte digit, byte col, byte row, byte symbol = 0) {
    if (digit > 9) return;
    for (int i = 0; i < 2; i++) {
        lcd.setCursor(col, row + i);
        for (int j = 0; j < 3; j++) {
            lcd.write(bignums[digit][i][j]);     //.kbv add the [i]
        }
        lcd.write(254);
    }
    if (symbol == 1) {
        lcd.setCursor(col + 3, row + 1);
        lcd.write(3);
    } else if (symbol == 2) {
        lcd.setCursor(col + 3, row);
        lcd.write(4);
        lcd.setCursor(col + 3, row + 1);
        lcd.write(4);
    }

    lcd.setCursor(col + 4, row);
}




void setup() {
  lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY
  lcd.setCursor(0, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA E LINHA
lcd.leftToRight();
    printbigchar(0, 0, 0);
   
 //   printbigchar(1, 4, 0);
   /* printbigchar(2, 8, 0);
    printbigchar(3, 12, 0);
    printbigchar(4, 16, 0, 1);
    printbigchar(5, 0, 2);
    printbigchar(6, 4, 2);
    printbigchar(7, 8, 2);
    printbigchar(8, 12, 2);
    printbigchar(9, 16, 2, 2);*/
}

void loop() {
  // put your main code here, to run repeatedly:

}
