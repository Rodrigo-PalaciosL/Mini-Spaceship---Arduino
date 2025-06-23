#include <EEPROM.h>
#include <LiquidCrystal.h>
#include "melodia.h"

const int buzz = 9;
const int btn = 3;

int moment = 0;
unsigned long tiempoAnterior = 0;

// música
int notaActual = 0;
unsigned long tiempoNotaAnterior = 0;
bool notaActiva = false;

// bandera de interrupción
volatile bool presionado = false;
//Definir pines del LCD

const int rs = 10, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte caracteres[8][8] = {
  { B00000, B00110, B01111, B11111, B01111, B00110, B00000, B11111 },
  { B00000, B00000, B00110, B01111, B00110, B00000, B00000, B11111 },
  { B01100, B00100, B11110, B01111, B11110, B00100, B01100, B01110 }
};

//Asignar el joystick
const int joystick_Y = A0; //Palanca hacia arriba o abajo (Movimiento en Y)
const int joystick_X = A1; //Palanca izquierda o derecha (Movimiento en X)
const int Ancho = 16; //Ancho de la pantalla lcd

//Estado del juego
char fila0[Ancho]; //Obstáculos en Primera fila
char fila1[Ancho]; //Obstáculos en Segunda fila
int spaceship_X = 1; //Posición de la astronave en las columnas
int spaceship_Y = 0; //Posición de la astronave en las filas
int intervaloJuego = 300; //Tiempo de actualización de escenario
int Difficulty = 6; //Dificultad progresiva con el tiempo
const int DifficultyRange = 7000; //Cada 7 segundos sube la dificultad
int Points = 0; //Acumulador de puntos
int ruptura0 = 0; int ruptura1 = 0; //Lectura para empezar a jugar
int maxPoints = 0;

//Posiciones y Movimiento del Joystick
unsigned long ultimoPaso = 0;
unsigned long ultimoMovimientoX = 0;
unsigned long ultimoAjuste = 0;
const int intervaloMovimientoX = 150;

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
//  EEPROM.write(0, 0); Reinicio de Record

  pinMode(btn, INPUT_PULLUP);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(btn), botonPresionado, FALLING);

  pinMode(joystick_X, INPUT);
  pinMode(joystick_Y, INPUT);
  randomSeed(analogRead(A3)); //Generación aleatoria

  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, caracteres[i]);
  }
  
  pinMode(buzz, OUTPUT);
  noTone(buzz);
}

void loop() {
  reproducirMusica();
  actualizarJuego();
}

void botonPresionado() {
  presionado = true;
}


void actualizarJuego() {
  if (moment == 0) {
    inicioJuego();
  } else if (moment == 1) {
    juego();
  } else {
    finJuego();
  }
}

void reproducirMusica() {
  unsigned long ahora = millis();
  if (notaActual < numNotas) {
    if (!notaActiva) {
      tone(buzz, melodia[notaActual][0]);
      tiempoNotaAnterior = ahora;
      notaActiva = true;
    }

    if (ahora - tiempoNotaAnterior >= melodia[notaActual][1]) {
      noTone(buzz);
      notaActual++;
      notaActiva = false;
    }
  } else {
    notaActual = 0;
  }
}

void inicioJuego() {
//Pantalla inicial
  maxPoints = EEPROM.read(0);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MINI SPACESHIP");
  lcd.setCursor(9,1);
  lcd.print("ARDUINO");
  delay(2000);
  lcd.clear();

  delay(500);

  lcd.setCursor(0, 0);
  lcd.print("Record: ");
  lcd.print(maxPoints);
  lcd.setCursor(0, 1);
  lcd.print("Click para iniciar");
  delay(200);

  while (!presionado) {
    delay(100);
  }
  presionado = false;
  moment = 1;

  //Generación limpia (Sin obstáculos) del escenario
  lcd.clear();
  lcd.setCursor(0,0);
  for (int i=0; i<Ancho; i++) {
    fila0[i] = ' ';
    fila1[i] = ' ';
  }
}

void juego() {
  if (tiempoAnterior == 0) {
    tiempoAnterior = millis();  
  }

//Leer joystick
  int ValorX = analogRead(joystick_X);
  int ValorY = analogRead(joystick_Y);

//Movimiento por joystick
  if (ValorY < 400) {
    spaceship_Y = 0;
  } else if (ValorY > 600) {
    spaceship_Y = 1;
  } //Para Y

  if (millis() - ultimoMovimientoX > intervaloMovimientoX) {
    if (ValorX < 400 && spaceship_X > 0) {
      spaceship_X--;
      ultimoMovimientoX = millis();
    } else if (ValorX > 600 && Ancho-5 > spaceship_X) {
      spaceship_X++;
      ultimoMovimientoX = millis();
    }
  } //Para X

  lcd.setCursor(spaceship_X,spaceship_Y);
  lcd.write(byte(0)); //Imprimir caracter de la astronave

  lcd.setCursor(spaceship_X,spaceship_Y);
  lcd.write(byte(1));

  lcd.setCursor(spaceship_X+1,spaceship_Y);
  lcd.write(byte(2));
//---------------------------------------------------------------

  if (millis() - ultimoPaso > intervaloJuego) {
    ultimoPaso = millis();
  //Generación de obstáculos
    int r = random(0, Difficulty);
    if (r == 0) {
      fila0[15] = '*';
      fila1[15] = ' ';
    } else if (r == 1) {
      fila0[15] = ' ';
      fila1[15] = '*';
    } else {
      fila0[15] = ' ';
      fila1[15] = ' ';
    }

//Aumento de Dificultad cada 7 segundos
    if (millis() - ultimoAjuste > DifficultyRange && intervaloJuego > 100) {
        intervaloJuego -= 50; //Aumento de velocidad del Juego
    if (Difficulty > 2) {
      Difficulty -= 1; //Aumento de probabilidad de obstáculos
  }
    ultimoAjuste = millis();
}

  //Movimiento del Escenario
    for (int i=0; i<Ancho-1; i++) {
      fila0[i] = fila0[i+1];
      fila1[i] = fila1[i+1];
    }
    fila0[Ancho-1] = ' ';
    fila1[Ancho-1] = ' ';

// GAME OVER por Colisión con Obstáculo
    if ((spaceship_Y == 0 && fila0[spaceship_X+1] == '*') ||
    (spaceship_Y == 1 && fila1[spaceship_X+1] == '*')) {
      Points = int((millis()-tiempoAnterior)/1000);
      moment = 2;
      return;
    }

//Imprimir Obstáculos
    for (int i = 0; i < Ancho; i++) {
      lcd.setCursor(i, 0);
      lcd.print(fila0[i]);
      lcd.setCursor(i, 1);
      lcd.print(fila1[i]);
    }
  }
}

void finJuego() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("GAME OVER");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("PUNTOS: ");
  lcd.print(Points);
  delay(2000);

  // Reinicio del récord si aplica
  if (Points > maxPoints){
    maxPoints = Points;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nuevo Record: ");
    lcd.setCursor(0, 1);
    lcd.print(Points);
    EEPROM.write(0, maxPoints);
    delay(2000);
  }

  // Mostrar récord actual
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Record: ");
  lcd.print(EEPROM.read(0));
  lcd.setCursor(0, 1);
  lcd.print("Click para reiniciar");
  delay(2000);

  // Reiniciar variables globales
  spaceship_X = 1;
  spaceship_Y = 0;
  intervaloJuego = 300;
  Difficulty = 6;
  ultimoPaso = 0;
  ultimoMovimientoX = 0;
  ultimoAjuste = 0;

  // Reset musical
  notaActual = 0;
  notaActiva = false;
  noTone(buzz);

  // Limpiar filas de obstáculos
  for (int i = 0; i < Ancho; i++) {
    fila0[i] = ' ';
    fila1[i] = ' ';
  }

  // Esperar que se presione el botón de nuevo
  while (!presionado) {
    delay(50);
  }

  Points = 0;
  tiempoAnterior = 0;  
  presionado = false;
  moment = 1;
}
