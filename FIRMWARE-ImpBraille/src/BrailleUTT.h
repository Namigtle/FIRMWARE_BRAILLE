/*
 * BrailleUTT.h
 *
 * Created: 06/07/2021 01:08:02 p. m.
 *  Author: lalo_
 */ 

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0X27,16,2);

#define Velocidad 9600
//declaración de terminales para los controladores A4988
#define PinDeDireccionX A1
#define PinDePasoX A0
#define PinHabilitadorCarro 38

#define PinDeDireccionY A7
#define PinDePasoY A6
#define PinHabilitadorAlHoja A2

#define Resolucion 400.0
#define RetardoDePaso 1

#define PotArAb A5
#define PotIzqDer A10
#define BotImprimir 44

#define estado 13
//declaración de terminales para los finales de carrera del carro percutor
#define FxMax 2
#define FxMin 3


#define PinLeva 11

//declaracion de terminales para los termistores
#define PinTerm0 A13
#define PinTerm1 A14
#define PinTerm2 A15

#define PinRamp0 9
#define PinRamp1 10

#define SSpin 53

#define DetectorHoja 32

#ifndef BRAILLEUTT_H_
#define BRAILLEUTT_H_

void iniciaLCD(){
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Impresora");
    lcd.setCursor(0,1);
    lcd.print("Braille");
    delay(1000);
	lcd.clear();
    Serial.println("Hola");
}




#endif /* BRAILLEUTT_H_ */