#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "BrailleUTT.h"

bool Refresh = false;
unsigned long TIni=0;
int Posicion=0;
double xi = 0.0;
double yi = 0.0;
File archivo;
File PosIni;
File root;
String Letra;
String PorVector;
String cad, cad1, cad2, cad3, ID;
bool ok = false;
int cursor=0;
int pos1=0, pos2;
double XX0=0.0;
double XX1=0.0;
double YY0=0.0;
double YY1=0.0;
bool Confirmacion = false;
char Caso=0;
int Posini=0;
char Lista[50][100];
int NumDeArchivo=0;
void CarroPercutor(double milimetrosx0, double milimetrosx1, double _Resolucionx){
	int Motpasosx=0;
	double milimetrosx = milimetrosx1 - milimetrosx0; 
	if (milimetrosx < 0){
		digitalWrite(PinDeDireccionX, HIGH);
	}
	else{
		digitalWrite(PinDeDireccionX, LOW);
	}
	Motpasosx = abs((_Resolucionx/40.0)* milimetrosx);
	for(int xx = 0; xx < Motpasosx; xx++){
		digitalWrite(PinDePasoX, HIGH);
		delay(RetardoDePaso);
		digitalWrite(PinDePasoX, LOW);
		delay(RetardoDePaso);
	}
}
void Rodillo(double milimetrosy0, double milimetrosy1, double _Resoluciony){
	int Motpasosy=0;
	double milimetrosy = milimetrosy1 - milimetrosy0;
	if (milimetrosy < 0){
		digitalWrite(PinDeDireccionY, HIGH);
	}
	else{
		digitalWrite(PinDeDireccionY, LOW);
	}
	Motpasosy = abs((_Resoluciony/50) * milimetrosy);
	for(int xx = 0; xx < Motpasosy; xx++){
		digitalWrite(PinDePasoY, HIGH);
		delay(RetardoDePaso);
		digitalWrite(PinDePasoY, LOW);
		delay(RetardoDePaso);
	}
}
void FinalDeCarrera0(){
	digitalWrite(estado, LOW);
	digitalWrite(PinHabilitadorCarro, HIGH);
	digitalWrite(PinHabilitadorAlHoja, HIGH);
	digitalWrite(PinRamp1, HIGH);
	lcd.clear();
	while(1){
		delay(100);
		Serial.println("E,Carro percutor fuera de rango");
		lcd.setCursor(0,0);
		lcd.print("Carro fuera");
		lcd.setCursor(0,1);
		lcd.print("de rango");
	}
}
void FinalDeCarrera1(){
	digitalWrite(estado, LOW);
	digitalWrite(PinHabilitadorCarro, HIGH);
	digitalWrite(PinHabilitadorAlHoja, HIGH);
	digitalWrite(PinRamp0, HIGH);
	lcd.clear();
	while(1){
		delay(100);
		Serial.println("E,Carro percutor fuera de rango");
		lcd.setCursor(0,0);
		lcd.print("Carro fuera");
		lcd.setCursor(0,1);
		lcd.print("de rango");
	}
}
void Imprimir(){
	Confirmacion=true;
	Caso = 'B';
}
void WriteLeva(int datodecimal){
	analogWrite(PinLeva, datodecimal);
}
int Termistor0(int PinT0){
	int lectura = analogRead(PinT0);
	return(lectura);
}
int Termistor1(int PinT1){
	int lectura = analogRead(PinT1);
	return(lectura);
}
int Termistor2(int PinT2){
	int lectura = analogRead(PinT2);
	return(lectura);
}
void GrabarUltimaPosicion(){
	PosIni=SD.open("test.txt", FILE_WRITE);
	if (PosIni){
		PosIni.seek(0);
		PosIni.println(String(xi,4) + "," + String(yi,4));
		PosIni.close();
	}
}
void LeerUltimaPosicion(){
	PosIni=SD.open("test.txt");
	if (PosIni){
		PosIni.seek(0);
		char Letra = 0;
        String Cadena = "";
        while (!(Letra == 10)){
            Letra = PosIni.read(); 
            Cadena = Cadena+Letra;
        }
		Serial.println("L:" + Cadena);
		PosIni.close();
	}
}
void Arranque(){
	PosIni=SD.open("test.txt");
	if (PosIni){
		PosIni.seek(0);
		char Letra = 0;
        String Cadena = "";
		Serial.println("Leyendo");
		uint8_t conter=0;
        while (!(Letra == 10)&&(conter<13)){
            Letra = PosIni.read(); 
            Cadena = Cadena+Letra;
			Serial.print(Cadena);
			conter++;
        }
		Serial.println("Termino de leer");
		PosIni.close();
		int Posicion=Cadena.indexOf(',');
		String Lxi = Cadena.substring(0,Posicion);
		String Lyi = Cadena.substring(Posicion+1);
		xi = Lxi.toDouble();
		yi = Lyi.toDouble();
	}
	else{
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Error");
		Serial.println("Error al abrir el archivo");
	}
	delay(100);
	CarroPercutor(0.0,-xi,Resolucion);
	delay(100);
	Rodillo(0.0,-yi,Resolucion);
	xi=0.0;
	yi=0.0;
	Serial.println("Listo para grabar ultima posicion");
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Grabando");
	GrabarUltimaPosicion();
	XX0=0.0;
	XX1=0.0;
	YY0=0.0;
	YY1=0.0;
	WriteLeva(42);
}
void Recibr(){
	if (Serial.available()){
		char id=Serial.read();
		switch (id){
			case 'I':
				archivo = SD.open("prueba.txt", FILE_WRITE);
				Serial.flush();
				delay(200);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("Iniciando...");
				Serial.print("F");
				break;
			case 'W':
				Confirmacion = true;
				archivo.close();
				delay(200);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("Finalizado");
				Caso = 'A';
				Serial.print("F");
				Caso = 'A';
				break;
			case 'f':
				delay(100);
				Serial.print("F");
				break;
			case 'D':
				pos1=cad.indexOf(':');
				cad3 = cad.substring(pos1+1);
				WriteLeva(cad3.toInt());	
				Serial.print("F");
				break;
			case 'A':
				xi=0;
				yi=0;
				GrabarUltimaPosicion();
				Serial.print("F");
				break;
			case 'B':
				LeerUltimaPosicion();
				Serial.print("F");
				break;
			case 'X':
				cad=Serial.readString();
				pos1=cad.indexOf(':');
				cad1 = cad.substring(pos1+1);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("X:");
				lcd.setCursor(2,0);
				lcd.print(cad1);
				CarroPercutor(0,cad1.toDouble(),Resolucion);
				xi = cad1.toDouble() + xi;
				GrabarUltimaPosicion();
				Serial.print("F");
				break;
			case 'Y':
				cad=Serial.readString();
				pos1=cad.indexOf(':');
				cad2 = cad.substring(pos1+1);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("Y:");
				lcd.setCursor(2,0);
				lcd.print(cad2);
				Rodillo(0,cad2.toDouble(),Resolucion);
				yi = cad2.toDouble() + yi;
				GrabarUltimaPosicion();
				Serial.print("F");
				break;
			default:
				if (archivo){
					archivo.print(id);
				}
				break;
		}
	}
}
void Ejecutar(String NomArcSel){
	if(Confirmacion){
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Coloque una hoja");
		while(!digitalRead(DetectorHoja));
		delay(500);
		Arranque();
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("El caso es:");
		lcd.setCursor(0,1);
		lcd.print(Caso);
		switch (Caso) {
			case 'A':
				archivo = SD.open(NomArcSel.c_str());
			  	break;
			case 'B':
			  	archivo = SD.open(NomArcSel.c_str());
			  	break;
		}
      	if (archivo){
        	while (archivo.available()){
          		archivo.seek(Posicion);
          		char Letra = 0;
          		String Cadena = "";
          		while (!(Letra == 10)){
            		Letra = archivo.read(); 
            		Cadena = Cadena+Letra;
            		Posicion = archivo.position();
          		}
		  		cad = Cadena;
				ID=cad[0];
				if (ID.equals("G")){
					pos1=cad.indexOf(':');
					cad3 = cad.substring(pos1+1);
					delay(50);
					WriteLeva(cad3.toInt());
					delay(50);
				}
				else if(ID.equals("C")){
					pos1=cad.indexOf(':');
					pos2=cad.indexOf(',');
					cad1 = cad.substring(pos1+1,pos2);
					cad2 = cad.substring(pos2+1);
					XX1=cad1.toDouble();
					xi = cad1.toDouble();
					CarroPercutor(XX0,XX1,Resolucion);
					XX0=XX1;
					YY1=cad2.toDouble();
					Rodillo(YY0,YY1,Resolucion);
					yi =cad2.toDouble();
					YY0=YY1;
				}
				else if(ID.equals("H")){
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("Coloque la");
					lcd.setCursor(0,1);
					lcd.print("siguiente hoja");
				}
        	}
        	archivo.close();
			if (Caso == 'A'){
				char copy[] = "prueba.txt";
				SD.remove(copy);
			}
        	Confirmacion = false;
      	}
      	else{
        	Serial.println("E:error de memoria");
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Error de memoria");
			delay(500);
      	}	
		delay(500);
		Caso = 0;
		GrabarUltimaPosicion();
		Arranque();
		lcd.clear();
	}
}
int printDirectory(File dir, int numTabs) {
	lcd.clear();
	int n=0;
	while (true) {
		File entry=dir.openNextFile();
		String Nombres=entry.name();
		int PosSD=Nombres.indexOf('.');
		String SubNombres=Nombres.substring(PosSD+1);
		if(SubNombres.equals("TXT")){
			//Serial.println(Nombres);
			for (uint8_t i = 0; i < Nombres.length(); i++){
				Lista[n][i]=Nombres[i];
			}
			n++;
			delay(100);
		}
		if (! entry){
			break;
		}
		entry.close();
	}
	return n;
}
void setup() {
	TCCR1B = TCCR0B && B11111000 | B00000100;
  	pinMode(PinLeva, OUTPUT);
	Serial.begin(Velocidad);
  	pinMode(PinDeDireccionX, OUTPUT);
	pinMode(PinDePasoX, OUTPUT);
	pinMode(PinHabilitadorCarro, OUTPUT);
	pinMode(PinDeDireccionY, OUTPUT);
	pinMode(PinDePasoY, OUTPUT);
	pinMode(PinHabilitadorAlHoja, OUTPUT);
	pinMode(PinRamp0, OUTPUT);
	pinMode(PinRamp1, OUTPUT);
	pinMode(estado, OUTPUT);
	pinMode(PinTerm0, INPUT);
	pinMode(PinTerm1, INPUT);
	pinMode(PinTerm2, INPUT);
	pinMode(DetectorHoja, INPUT);
	pinMode(BotImprimir,INPUT);	
	digitalWrite(estado, HIGH);
	digitalWrite(PinHabilitadorCarro, LOW);
	digitalWrite(PinHabilitadorAlHoja, LOW);
	attachInterrupt(digitalPinToInterrupt(FxMax), FinalDeCarrera0, RISING);
	attachInterrupt(digitalPinToInterrupt(FxMin), FinalDeCarrera1, RISING);
	iniciaLCD();
	WriteLeva(42);
  	if (!SD.begin(SSpin)){
    	Serial.println("E:Fallo en la inicialización de memoria!");
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Error de inicio");
		lcd.setCursor(0,1);
		lcd.print("de memoria");
		while (1);
    return;
  	}
	Caso=0;
	root=SD.open("/");
	delay(100);
	NumDeArchivo = printDirectory(root, 0);
}

void loop(){
	if(!digitalRead(BotImprimir)){
		Serial.println("entra");
		while(!digitalRead(BotImprimir));
		lcd.clear();
		while(!ok){
			if(analogRead(PotArAb) <= 300){
				lcd.setCursor(14,cursor);
				lcd.print("  ");
				cursor++;
				if(cursor>1){cursor=0;}
				lcd.setCursor(14,cursor);
				lcd.print("<=");
				delay(500);
			}
			else if(analogRead(PotArAb) >= 600){
				lcd.setCursor(14,cursor);
				lcd.print("  ");
				cursor--;
				if(cursor<0){cursor=1;}
				lcd.setCursor(14,cursor);
				lcd.print("<=");
				delay(500);
			}
			else if(analogRead(PotIzqDer) >= 600){
				ok=true;
				lcd.clear();
			}
			else if(!digitalRead(BotImprimir)){	
				while (!digitalRead(BotImprimir));
				if (cursor == 1){
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("Esperando");
					lcd.setCursor(0,1);
					lcd.print("conexion...");
					while(!Confirmacion){
						Recibr();
					}
					delay(1000);
					while (Confirmacion){
						Ejecutar("prueba.txt");
					}
				}
				else if (cursor == 0){
					lcd.clear();
					int Cursor2=0;
					int Cursor3=1;
					int Cursor4=0;
					int Cursor5=0;
					lcd.setCursor(14,Cursor4);
					lcd.print("<=");
					while (1){	
						if (analogRead(PotArAb) <= 300){
							lcd.setCursor(14,Cursor4);
							lcd.print("  ");
							Cursor4--;
							Cursor5--;
							if(Cursor4<0){
								lcd.clear();
								Cursor4=0;
								Cursor2--;
								Cursor3--;
								if(Cursor2<0){
									Cursor2=Cursor2+1;
									Cursor3=Cursor3+1;
									Cursor5=Cursor5+1;
								}
							}
							lcd.setCursor(14,Cursor4);
							lcd.print("<=");
							while (analogRead(PotArAb) <= 300);
						}
						else if(analogRead(PotArAb) >= 600){ // Flecha hacia abajo
							lcd.setCursor(14,Cursor4);
							lcd.print("  ");
							Cursor4++;
							Cursor5++;
							if(Cursor4>1){
								lcd.clear();
								Cursor4=1;
								Cursor2++;
								Cursor3++;
								if (Cursor3>=NumDeArchivo){
									Cursor3=Cursor3-1;
									Cursor2=Cursor2-1;
									Cursor5=Cursor5-1;
								}
							}
							lcd.setCursor(14,Cursor4);
							lcd.print("<=");
							while (analogRead(PotArAb) >= 600);
						}
						else if(analogRead(PotIzqDer) >= 600){
							while (analogRead(PotIzqDer) >= 600);
							Serial.println("Cerrado");
							lcd.clear();
							break;
						}
						else if(!digitalRead(BotImprimir)){
							while (!digitalRead(BotImprimir));
							String ArchivoSelc="";
							for (int i2 = 0; i2 < 100; i2++){
								if(Lista[Cursor5][i2] == '.'){break;}
								else{ArchivoSelc= ArchivoSelc + Lista[Cursor5][i2];}
							}
							ArchivoSelc=ArchivoSelc+".txt";
							Confirmacion=true;
							Caso='B';
							Ejecutar(ArchivoSelc);
						}
						else{
							String Nom1="";
							String Nom2="";
							for (uint8_t i = 0; i <100; i++){
								if(Lista[Cursor2][i] == '.'){break;}
								else{Nom1 = Nom1 + Lista[Cursor2][i];}
							}
							for (uint8_t ii = 0; ii <100; ii++){
								if(Lista[Cursor3][ii] == '.'){break;}
								else{Nom2 = Nom2 + Lista[Cursor3][ii];}
							}
							Nom1 = Nom1+".txt";
							Nom2 = Nom2+".txt";
							lcd.setCursor(0,0);
							lcd.print(Nom1);
							lcd.setCursor(0,1);
							lcd.print(Nom2);
						}
					}
				}
			}
			else{
				lcd.setCursor(0,0);
				lcd.print("Manual");
				lcd.setCursor(0,1);
				lcd.print("Tranferencia");
				lcd.setCursor(14,cursor);
				lcd.print("<=");
			}
		}
	}
	else{
		lcd.setCursor(0,0);
		lcd.print("Home");
		lcd.setCursor(0,1);
		lcd.print("Iniciar");
		lcd.setCursor(14,1);
		if((millis()-TIni)>=500){
			Refresh=!Refresh;
			TIni = millis();
		}
		if (Refresh){lcd.print("<=");}
		else{lcd.print("  ");}
		ok = false;
		Serial.println("Home");
		Serial.println("Iniciar");
	}
}   
