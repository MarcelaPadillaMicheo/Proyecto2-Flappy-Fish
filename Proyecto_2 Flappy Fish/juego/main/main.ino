/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * IE3027: Electrónica Digital 2 - 2019
 * Johann Haeussler y Marcela padilla
 */
//*
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include <SD.h>
#include <SPI.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
int BOTON_PIN1=35;
int BOTON_PIN2=34;
int BOTON_PIN3=36;
#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  
//*
// Functions Prototypes
//*
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);

extern uint8_t fondo[];
extern uint8_t fondo1[];
extern uint8_t menu[];

// Variables para el menú
int q = 320;int state = 0; // Variable de estado para alternar entre bala y nave
int q1=320;
int inicio;
int start;
int cambios;
int estado = 0;
void handleButton1();
void handleButton2();
//*
// Inicialización
//*

void setup() {
  
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);
  SPI.setModule(0);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  //SD.begin(10);
  pinMode(PA_3, OUTPUT);// CS in SD card
  LCD_Init();
  LCD_Clear(0x00);


 // FillRect(0, 0, 319, 206, 0x000);
  //String text1 = "Flappy Bird";
  //LCD_Print(text1, 20, 100, 2, 0xffff, 0x000);
   if (!SD.begin(PA_3)) {// iniciar SD
    // Serial.println("initialization failed!");
    return;
  }
//LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);

    // Continúa con el resto del programa aquí 

  // pinMode(BOTON_PIN, INPUT_PULLUP);
  // pinMode(BOTON_PIN2, INPUT_PULLUP);
   pinMode(BOTON_PIN1, INPUT_PULLUP);
   pinMode(BOTON_PIN2, INPUT_PULLUP);
   pinMode(BOTON_PIN3, INPUT_PULLUP);
    // FillRect(0, 0, 320, 120, 0xEFFB);
  //  FillRect(0, 120, 320, 120, 0x9654);
   attachInterrupt(digitalPinToInterrupt(BOTON_PIN1), handleButton1, CHANGE);

   // Attach interrupt for button 2
   attachInterrupt(digitalPinToInterrupt(BOTON_PIN2), handleButton2, CHANGE);

}
//*
// Loop Infinito
//*
int y = 52; // Variable y que controlaremos con la señal en PC6
int y2 = 170; // Variable y que controlaremos con la señal en 
int x;
int anim2 = (y / 35) % 3;

bool jumping = false; // Variable para controlar si el flappy está saltando
bool jumping2 = false; // Variable para controlar si el flappy está saltando
int r=random(130,220);
int b=random(0,110);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool botonPresionado1 = digitalRead(BOTON_PIN1) == LOW; // Si está presionado, será LOW
bool botonPresionado2 = digitalRead(BOTON_PIN2) == LOW; // Si está presionado, será LOW

//bool botonPresionado2 = digitalRead(BOTON_PIN2) == LOW; // Si está presionado, será LOW
int contadorFlappy1 = 0; // Contador para el flappy 1
int contadorFlappy2 = 0; // Contador para el flappy 2
bool juegoTerminado = false;
bool juegoReiniciado = false;
void loop() {
 
bool botonPresionado1 = digitalRead(BOTON_PIN1) == LOW;
bool botonPresionado2 = digitalRead(BOTON_PIN2) == LOW;

File jugar = SD.open("JUGAR.TXT", FILE_READ);     // Trarjeta SD 
File titulo = SD.open("TITULO.TXT", FILE_READ);
File info = SD.open("INFO.TXT", FILE_READ);
File inst = SD.open("INST.TXT", FILE_READ);


  if (jugar && titulo) {              // Mandar a llamar variables 
    String jugarContent = "";
    while (jugar.available()) {
      char a = jugar.read();
      jugarContent += a;
    }
    jugar.close();
    
    String tituloContent = "";
    while (titulo.available()) {
        char a = titulo.read();
        tituloContent += a;
    }
    titulo.close();
    
    String infoContent = "";
    while (info.available()) {
        char a = info.read();
        infoContent += a;
    }
    info.close();  
        String instContent = "";
    while (inst.available()) {
        char a = inst.read();
        instContent += a;
    }
    inst.close();  

 if (estado==0) {               // Estado del menu
  inicio = 0;
   LCD_Print(tituloContent, 70, 40, 2, 0xFFFF, 0x0000);
   LCD_Print(jugarContent, 60, 170, 2, 0xFFFF, 0x0000);
   LCD_Print(infoContent, 180, 170, 2, 0xFFFF, 0x0000);
 }

 else if (estado == 3)  {     // Estado de Flappp 1 ganador
    if (start == 0){
    LCD_Clear(0x37DC); // Borra la pantalla
    LCD_Print("Gana Flappy 1", 50, 100, 2, 0x000,0x37DC);
   // LCD_Sprite(x, 70, 19, 15, flappy, 1, 0, 0, 1);
    start++;
  }
}

else if (estado == 4)  {      // Estado de Flappy 2 ganador
    if (start  == 0){
    LCD_Clear(0x37DC); // Borra la pantalla
    LCD_Print("Gana Flappy 2", 50, 100, 2, 0x000, 0x37DC);
    start++;
  }
}
else if (estado ==1 && !juegoTerminado) {  //Estado en el que se presenta el juego
  if (inicio == 0){
    FillRect(0, 0, 320, 120, 0xEFFB);
    FillRect(0, 120, 320, 120, 0x9654);
    inicio++;
  }   

  
  int anim2 = (y / 35) % 3;

  LCD_Sprite(80, y, 19, 15, flappy, 3, anim2, 0, 1);
  LCD_Sprite(80, y+16, 19, 3, borde, 1, 0, 0, 1);


  if (y >= 115 - 21 ) {
      y=115-22 ; // Cambia la dirección en el eje Y
    }
     if (y <=  0) {
      y=1 ; // Cambia la dirección en el eje Y
    }


  //  flappy siempre caiga
  fall2();
  fall();

  LCD_Sprite(80, y2, 19, 15, flappy2, 3, anim2, 0, 1);
  LCD_Sprite(81, y2+16, 18,3, borde1, 1, 0, 0, 1);
  
  if (y2 >= 240 - 21 ) {
      y2=240-22 ; // Cambia la dirección en el eje Y
    }
     if (y2 <=  120) {
      y2=121 ; // Cambia la dirección en el eje Y
    }
delay(0);

// Estados en los que empiezan los flappys a salir a jugar 
  if (state==0) {
    LCD_Sprite(q, r, 39, 24, bala, 1, 0, 0, 1);
    V_line(q + 39, r, 24, 0x9654);
  }
  if (state==1) {
    LCD_Sprite(q, b, 39, 24, bala1, 1, 0, 0, 1);
    V_line(q + 39, b, 24, 0xEFFB);
      }      
  q--;


  if (q < -40) {
    q = 320;
    r=random(170,200);
    b=random(40,100);
    state = 1 - state; // Alternar entre 0 y 1
  }
  Flap1 ();
mostrarContadores(contadorFlappy1, contadorFlappy2);
}

else if (estado == 2)  {  // Estado de instrucciones del juego
 LCD_Print(instContent, 30, 40, 2, 0xFFFF, 0x0000);
 delay(10);
}

// Cambio ente estados del menu
cambioes();


}

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cambioes() {
    static int lastState1 = HIGH;
    static int lastState2 = HIGH;
    
    
    int state1 = digitalRead(BOTON_PIN1);
    int state2 = digitalRead(BOTON_PIN2);
    
    if (state1 == LOW && lastState1 == HIGH) {
        estado = 1;
    }
    
    if (state2 == LOW && lastState2 == HIGH) {
        estado = 1;
    }


    lastState1 = state1;
    lastState2 = state2;
}

void Flap1 (){
if ( 80+19 >= q && 80<= q+24 && y +19>= b && y <= b)  {
 contadorFlappy1++; // Incrementa el contador del flappy 1
}

if ( 80+19 >= q && 80<= q+24 && y2 +19>= r && y2 <= r)  {
contadorFlappy2++; // Incrementa el contador del flappy }
}
 // Comprueba si alguno de los Flappys llegó a 30
    if (contadorFlappy1 >= 30 && !juegoReiniciado) {
     juegoReiniciado = true;
      // Muestra el mensaje de victoria del Flappy 1 en la pantalla      
      estado = 3;
    } else if (contadorFlappy2 >= 30 && !juegoReiniciado) {
     juegoReiniciado = true;
      // Muestra el mensaje de victoria del Flappy 2 en la pantalla
      estado = 4;
    }
}
void mostrarContadores(int contadorFlappy1, int contadorFlappy2) {
  String text1 = "1 : " + String(contadorFlappy1);
  String text2 = "2 : " + String(contadorFlappy2);
  
  // Mostrar los contadores en la pantalla en las posiciones deseadas
  LCD_Print(text1, 20, 10, 2, 0xFFFF, 0x0000);
  LCD_Print(text2, 200, 10, 2, 0xFFFF, 0x0000);
}

void jump() {
  y -= 3; // Incrementa gradualmente la posición Y hacia arriba
  delay(0); // Ajusta este valor para controlar la velocidad del movimiento
  jumping = true;
}

void fall() {
  y += 1; // Incrementa gradualmente la posición Y hacia abajo
  delay(0); // Ajusta este valor para controlar la velocidad del movimiento
  jumping = false;
}
void jump2() {
  y2 -= 3; // Incrementa gradualmente la posición Y hacia arriba
  delay(0); // Ajusta este valor para controlar la velocidad del movimiento
  jumping2 = true;
}

void fall2() {
  y2 += 1; // Incrementa gradualmente la posición Y hacia abajo
  delay(10); // Ajusta este valor para controlar la velocidad del movimiento
  jumping2 = false;
}

// Button 1 handler
void handleButton1() {
  if (botonPresionado1 && !jumping) {
    jump();
  }
}

// Button 2 handler
void handleButton2() {
  if (botonPresionado2 && !jumping2) {
    jump2();
  }
}
//*
// Función para inicializar LCD
//*
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //**
  // Secuencia de Inicialización
  //**
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //**
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //**
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //**
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //**
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //**
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //**
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //**
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //**
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //**
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //**
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //**
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //**
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //**
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//*
// Función para enviar comandos a la LCD - parámetro (comando)
//*
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//*
// Función para enviar datos a la LCD - parámetro (dato)
//*
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//*
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//*
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//*
// Función para borrar la pantalla - parámetros (color)
//*
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//*
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//* 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//*
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//* 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//*
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//*
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//*
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//*
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}
*/

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+w;
  y2 = y+h;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = w*h*2-1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
      
      //LCD_DATA(bitmap[k]);    
      k = k - 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//*
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//*
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//*
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//*
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//*
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//*
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
  for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width -1 - offset)*2;
      k = k+width*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k - 2;
     } 
  }
  }else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
  }
    
    
    }
  digitalWrite(LCD_CS, HIGH);
}
