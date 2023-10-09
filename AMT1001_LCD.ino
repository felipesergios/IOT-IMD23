#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4); 
const int pinoAnalogico = 0; // Substitua pelo número do pino analógico que você deseja ler

float voltOutHum = 0; 

void setup() {
  lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Initram");
  lcd.setCursor(2,1);
  lcd.print("Device checked");
   lcd.setCursor(0,2);
  lcd.print("Happy hack!");
   lcd.setCursor(2,3);
  lcd.print("Power By Feras");
  Serial.begin(115200); // Inicializa a comunicação serial
}

void loop() {
  int valorAnalogico = analogRead(pinoAnalogico); // Lê o valor analógico do pino

  voltOutHum = valorAnalogico / 30;
  
  
  Serial.print("Valor Analógico: ");
  Serial.println(valorAnalogico);
  Serial.print("Valor %: ");
  Serial.println(voltOutHum); // Imprime o valor no monitor serial
  delay(1000); // Aguarda 1 segundo antes de ler novamente (ajuste conforme necessário)
}
