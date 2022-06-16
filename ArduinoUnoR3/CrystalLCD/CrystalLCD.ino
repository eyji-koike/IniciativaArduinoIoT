//
//

//includes
#include "LCDFunctions.h"

const int rs = 13, 
          en = 12, 
          d4 = 7, 
          d5 = 6, 
          d6 = 5, 
          d7 = 4;


LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup()
{
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);
  Serial.begin(9600);
}
 
void loop()
{
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(3, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("FILIPEFLOP");
  lcd.setCursor(3, 1);
  lcd.print(" LCD 16x2");
  delay(5000);
   
  //Rolagem para a esquerda
  for (int posicao = 0; posicao < 3; posicao++)
  {
    lcd.scrollDisplayLeft();
    delay(300);
  }
   
  //Rolagem para a direita
  for (int posicao = 0; posicao < 6; posicao++)
  {
    lcd.scrollDisplayRight();
    delay(300);
  }
}
