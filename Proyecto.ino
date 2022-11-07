
//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * IE3027: Electrónica Digital 2 - 2019
 */
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "TM4C123GH6PM.h"
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

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  
const int boton1 = PUSH1;
const int boton2 = PUSH2;
 int animation3 = 0;
 int animation3_2 = 0;
int x = 0;

int control1 = 0;
int control2 = 0;
int state1 = 0;
int state2 = 0;
int state3 = 0;
int state4 = 0;
int y_inicial = 240-71;
int x_inicial = 0;
int x_inicial1 = 0;
int y_inicial1  = 240-71;
int x_inicial2 = 160;
int y_inicial2  = 240-71;
int scene = 1;
int scene1 = 1;
int scene2 = 1;
int scene3 = 1;
int flag = 0;
int interval = 500;
extern uint8_t kirby_caminar[]; 
extern uint8_t kirby_quieto[];
extern uint8_t  piso_bajo[];
extern uint8_t logo[]; 
extern uint8_t  piso_alto[];
extern uint8_t  esquina_izquierda[];
extern uint8_t  esquina_derecha[];
extern uint8_t  agua[];
extern uint8_t  hoyo[];
extern uint8_t  bloque[];
extern uint8_t  meta[];
extern uint8_t icekirby_caminar[]; 
extern uint8_t icekirby_quieto[];
extern uint8_t icekirby_saltando[];
uint8_t previousMillis1;
uint8_t previousMillis2;
uint16_t initialscore1;
uint16_t initialscore2;
uint16_t initialscore3;
int data[5]= {0,0,0,0,0};
int variable;
String dato0;
String dato1;
String dato2;
String dato3;
String dato4;
//uint16_t SCREEN_WIDTH = 320;
//uint8_t SCREEN_HEIGHT = 240;


//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
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

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
/*
  SD card test

 This example shows how use the utility libraries on which the'
 SD library is based in order to get info about your SD card.
 Very useful for testing a card when you're not sure whether its working or not.

 The circuit:
  * SD card attached to SPI bus as follows (TM4C123)
 ** On SPI module 0:
 ** MOSI - PA_5
 ** MISO - PA_4
 ** CLK - PA_2
 ** CS - PA_3


 created  28 Mar 2011
 by Limor Fried
 modified 9 Apr 2012
 by Tom Igoe
 modified 4 oct 2022
 by Jose Morales
 */
// include the SD library:
#include <SPI.h>
#include <SD.h>


// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
File myFile;
const int chipSelect = PA_3; //cs PIN

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial2.begin(115200); 


  SPI.setModule(0);

  Serial.print("\nInitializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(chipSelect, OUTPUT);     // change this to 53 on a mega


  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);


if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
  SD.remove("scores.txt");
 myFile = SD.open("scores.txt", FILE_WRITE);
 myFile.close();
 myFile = SD.open("scores.txt", FILE_WRITE);
 myFile.println("643, 584, 581, 573, 571");
 myFile.close();

  
  


  // Frecuencia de reloj (utiliza TivaWare)
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  // Configuración del puerto (utiliza TivaWare)
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  
  // Inicialización de la pantalla
  LCD_Init();
  LCD_Clear(0x06DF);
 


//----------------------------------------------------------------


}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);

      if (Serial3.available()) {
      control2 = Serial3.read();
   // Serial.println(control1); 
  }
  
  if (Serial2.available()) {
     control1 = Serial2.read();
    //Serial.println(control2);
  }

  state1 = digitalRead(boton1);
  state2 = digitalRead(boton2);

  if(state1 == HIGH & state2 == LOW){
    control1 = 8;
  }
  if (control1 == 16){
    control1 = 16;
  }
  
if (flag == 0){
if (scene1 == 1){
  LCD_Clear(0x3838);
  LCD_Bitmap(60, 17, 200, 120, logo);
  LCD_Print("Press to start", 50, 165, 2, 0xFFFF, 0x3838);
  scene1++;
 
 }
 else if (scene1== 2){
  
  if (control1 == 16 | control1 == 8){
     scene1= 3;}
 }
  else if (scene1 == 3){
 LCD_Clear(0x3838);
 LCD_Print("Player Mode", 70, 15, 2, 0xFFFF, 0x3838);
 LCD_Print("Single Player",80, 80, 2, 0xFFFF, 0x3838);
 LCD_Print("Multiplayer",80, 145, 2, 0xFFFF, 0x3838);
 scene1++;
 
}
else if (scene1 == 4){
 if (control1 == 16 | control1 == 8){
     scene1= 5;}
  }

else if (scene1 == 5){

 LCD_Print("Player Mode", 70, 15, 2, 0xFFFF, 0x3838);
 LCD_Print("Single Player",80, 80, 2, 0xE981, 0x3838);
 LCD_Print("Multiplayer",80, 145, 2, 0xFFFF, 0x3838);
 scene1++; 

}
else if (scene1 == 6){
 if (control1 == 16){
     scene1= 7;}
  else if (control1 == 8){
     flag= 1;}
}
else if (scene1 == 7){

 LCD_Print("Player Mode", 70, 15, 2, 0xFFFF, 0x3838);
 LCD_Print("Single Player",80, 80, 2, 0xFFFF, 0x3838);
 LCD_Print("Multiplayer",80, 145, 2, 0xE981, 0x3838);
 scene1++;


}
else if (scene1 == 8){
  if (control1 == 16 ){
     scene1= 5;}
  else if (control1 == 8){
     flag= 3;}

}

else if (scene1 == 9){
  LCD_Clear(0x3838);
  String puntaje1 = String(int (initialscore1));
  LCD_Print("Score:", 120, 100, 2, 0xFFFF, 0x3838);
  LCD_Print(puntaje1, 120, 120, 2, 0xFFFF, 0x3838);
  LCD_Print("Press to continue",25, 220, 2, 0xFFFF, 0x3838);
  variable = initialscore1;
  delay(1000);
     scene1 = 13;
  
     scene1 = 12;

}

else if (scene1 == 10){
  LCD_Clear(0x3838);
  String puntaje2 = String(int (initialscore2));
  LCD_Print("Winner: Player 1",30, 90, 2, 0xFFFF, 0x3838);
 // LCD_Print("Press to continue",25, 220, 2, 0xFFFF, 0x3838);
  LCD_Print("Score:", 120, 110, 2, 0xFFFF, 0x3838);
  LCD_Print(puntaje2, 160, 130, 2, 0xFFFF, 0x3838);
  variable = initialscore2;
  delay(1000);
     scene1 = 13;
     
}


else if (scene1 == 11){
  LCD_Clear(0x3838);
  String puntaje3 = String(int (initialscore3));
  LCD_Print("Winner: Player 2",30, 90, 2, 0xFFFF, 0x3838);
  LCD_Print("Score:", 120, 110, 2, 0xFFFF, 0x3838);
  LCD_Print(puntaje3, 120, 130, 2, 0xFFFF, 0x3838);
  variable = initialscore3;
  delay(1000);
     scene1 = 13;
  
}

else if (scene1 == 12){

LCD_Clear(0x3838);
myFile = SD.open("scores.txt");
LoadSD(myFile, data);
String dato1 = String(int(data[4]));
String dato2 = String(int(data[3]));
String dato3 = String(int(data[2]));
String dato4 = String(int(data[1]));
String dato5 = String(int(data[0]));
 LCD_Print("5 top scores", 15, 15, 2, 0xFFFF, 0x3838);
 LCD_Print("1.Score:", 0, 80, 2, 0xFFFF, 0x3838);
 LCD_Print("2.Score:", 0, 100, 2, 0xFFFF, 0x3838);
 LCD_Print("3.Score:", 0, 120, 2, 0xFFFF, 0x3838);
 LCD_Print("4.Score:", 0, 140, 2, 0xFFFF, 0x3838);
 LCD_Print("5.Score:", 0, 160, 2, 0xFFFF, 0x3838);

 LCD_Print(dato1 , 140, 80, 2, 0xFFFF, 0x3838);
 LCD_Print(dato2 , 140, 100, 2, 0xFFFF, 0x3838);
 LCD_Print(dato3 , 140, 120, 2, 0xFFFF, 0x3838);
 LCD_Print(dato4 , 140, 140, 2, 0xFFFF, 0x3838);
 LCD_Print(dato5 , 140, 160, 2, 0xFFFF, 0x3838);


  
}

else if (scene1 == 13){
LCD_Clear(0x3838);
myFile = SD.open("scores.txt");
LoadSD(myFile, data);
String dato1 = String(int(data[4]));
String dato2 = String(int(data[3]));
String dato3 = String(int(data[2]));
String dato4 = String(int(data[1]));
String dato5 = String(int(data[0]));
 LCD_Print("5 top scores", 15, 15, 2, 0xFFFF, 0x3838);
 LCD_Print("1.Score:", 0, 80, 2, 0xFFFF, 0x3838);
 LCD_Print("2.Score:", 0, 100, 2, 0xFFFF, 0x3838);
 LCD_Print("3.Score:", 0, 120, 2, 0xFFFF, 0x3838);
 LCD_Print("4.Score:", 0, 140, 2, 0xFFFF, 0x3838);
 LCD_Print("5.Score:", 0, 160, 2, 0xFFFF, 0x3838);


 LCD_Print(dato1 + "53", 140, 80, 2, 0xFFFF, 0x3838);
 LCD_Print(dato2 + "46", 140, 100, 2, 0xFFFF, 0x3838);
 LCD_Print(dato3 + "78", 140, 120, 2, 0xFFFF, 0x3838);
 LCD_Print(dato4 + "87", 140, 140, 2, 0xFFFF, 0x3838);
 LCD_Print(dato5 + "33", 140, 160, 2, 0xFFFF, 0x3838);

delay(1000);
     scene1= 14;
 
}

else if (scene1 == 14){
 
     flag= 0;
     scene1= 0;

}

}


if (flag ==1){
  LCD_Clear(0x06DF);
  initialscore1 = 250;
  flag++;
}
 else if (flag ==2){
   unsigned long currentMillis1 = millis();

  if (currentMillis1 - previousMillis1 >= interval) {
    initialscore1 = initialscore1 - 20;
    previousMillis1 = currentMillis1;

  }

if (scene == 1){
  LCD_Clear(0x06DF);
  for (int x_pb = 0; x_pb < 298; x_pb += 33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo );
  }
  for (int x_pb = 53; x_pb < 134; x_pb += 10) {
    LCD_Bitmap(x_pb, 190, 19, 19,piso_alto );
  }
  LCD_Bitmap(53, 190, 19, 19,esquina_izquierda );
  LCD_Bitmap(133, 190, 19, 19,esquina_derecha );
  LCD_Bitmap(161, 207, 48, 14,agua );
  for (int x_pb = 221; x_pb < 302; x_pb += 10) {
    LCD_Bitmap(x_pb, 190, 19, 19,piso_alto );
  }
  LCD_Bitmap(220, 190, 19, 19,esquina_izquierda );
  
scene++;
 }
 else if (scene == 3){
  LCD_Clear(0x06DF);
  for (int x_pb = 0; x_pb < 298; x_pb +=33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo  );
  }
  for (int x_pb = 0; x_pb < 136; x_pb += 10) {
    LCD_Bitmap(x_pb, 190, 19, 19,piso_alto );
  }
  LCD_Bitmap(138, 190, 19, 19,esquina_derecha );
  LCD_Bitmap(251, 190, 60, 49,bloque );
  LCD_Bitmap(311, 190, 19, 19,esquina_izquierda );
  scene++;
 }
  else if (scene == 5){
  LCD_Clear(0x06DF);
  for (int x_pb = 0; x_pb < 298; x_pb +=33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo  );
  }
  for (int x_pb = 0; x_pb < 301; x_pb += 10) {
    LCD_Bitmap(x_pb, 190, 19, 19,piso_alto );
  }
  LCD_Bitmap(151, 190, 54, 49, hoyo);
  LCD_Bitmap(273, 127, 43,67, meta);
  scene++;
 }


//Salto derecha
if  (control1 == 8){ 
  control1 ==0;
 int x_1;
 int i = 0;
 int y = 0;
 
    delay(2);
  x = x_inicial;
  while (i == 0){
    for (int y = 0; y <26; y++){
    y_inicial--;
    int animationSpeed = (y_inicial/10) % 11; // El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    
     LCD_Sprite(x,y_inicial, 36, 36, kirby_saltando, 11, animationSpeed, 0, 0);
     H_line(x, y_inicial + 37, 36, 0x06DF);
     H_line(x, y_inicial + 36, 36, 0x06DF);

    }

      i++;
  }
 
for (int i = 0; i<31; i++){
    
  if (x > 283){
      x =0;
      if (scene==6){
        scene1 = 9;
        flag = 0;
        }
      else{
      initialscore1 = initialscore1 + 250;
      scene++;}
  }
  else{
    x++;
    }
    
    y =(-(x-x_inicial))+(y_inicial);
    if (y < 0){
      y = 0;
    }
    
    
    int animationSpeed = (x/10) % 11; // El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    
    LCD_Sprite(x,y, 36, 36, kirby_saltando, 11, animationSpeed, 0, 0);
    
    V_line(x - 1, y +36, 36, 0x06DF);
    H_line(x - 1, y + 36, 36, 0x06DF);

  }

  
                                                    
  y_inicial = (-(x-x_inicial))+(y_inicial);
  x_inicial = x;
  


 for (int i = 0; i<31; i++){
    x_1 = x_inicial;
    if (x_inicial> 283){
      x_inicial=0;
      if (scene==6){
        scene1 = 9;
        flag = 0;
        }
      else{
      initialscore1 = initialscore1 + 250;
      scene++;}
  }
  else{
    x_inicial++;
    }
  
    int y =((x_inicial-x_1))+(y_inicial);
    
    int animationSpeed2 = (x_inicial/10) % 11; // El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    
    LCD_Sprite(x_inicial,y, 36, 36, kirby_saltando, 11, animationSpeed2, 0, 0);
    V_line(x_inicial - 1, y, 36, 0x06DF);
    H_line(x_inicial - 1, y, 36, 0x06DF);
    
  }
  y_inicial = ((x_inicial-x_1))+(y_inicial);
  
 }


  //Adelante
   else if  (control1 == 16){
    control1 == 0 ;
 
    delay(2);
    x = x_inicial;
    if (scene == 2){
      if (x_inicial> 283){
      x_inicial =0;
      if (scene==6){
        scene1 = 9;
        flag = 0;
        }
      else{
      initialscore1 = initialscore1 + 250;
      scene++;}
      }
    else{
      x_inicial++;
      x++;      
     }
     if (x > 16 & y_inicial == 169){
        x_inicial = 15;
        x = 15;
      }
      else if (x>=0 & x<=16 & y_inicial >= 169){
        y_inicial = 169;
      }
      else if (x >= 17 & x <= 152 & y_inicial >= 150){
        y_inicial = 150;
      }
      else if (x >= 152 & x <= 184 & y_inicial > 169){
        y_inicial = 169;
      }
      else if (x >= 153 & x <= 183 & y_inicial == 169){
        x_inicial = 0;
        initialscore1 = initialscore1 - 50;
        FillRect(152, 171, 67, 36, 0x06DF);
      } 
      else if (x >= 184 & x <= 284 & y_inicial >= 150){
        y_inicial = 150;
      } 

      }
    
    
  else if (scene == 4){
    if (x_inicial> 283){
      x_inicial =0;
      if (scene==6){
        scene1 = 9;
        flag = 0;
        }
      else{
      initialscore1 = initialscore1 + 250;
      scene++;}
      }
    else{
      x_inicial++;
      x++;      
     }
    if (x > 215 & y_inicial == 169){
        x_inicial = 215;
        x = 215;
      }
      else if (x>=0 & x<=157 & y_inicial >= 150){
        y_inicial = 150;

      }
      else if (x >= 158 & x <= 215 & y_inicial >= 169){
        y_inicial = 169;
   
      }
      else if (x >=216 & x <= 320 & y_inicial >= 150){
        y_inicial = 150;
        
      }
  }
      
      else if (scene == 6){
      if (x_inicial> 283){
      x_inicial =0;
      if (scene==6){
        scene1 = 9;
        flag = 0;
        }
      else{
      initialscore1 = initialscore1 + 250;
      scene++;}
      }
    else{
      x_inicial++;
      x++;      
     }
      if (x > 236 & y_inicial == 150){
        x_inicial = 236;
        x = 236;
      }
      else if (x>=0 & x<=151 & y_inicial >= 150){
        y_inicial = 150;
      }
      else if (x >= 152 & x <= 205 & y_inicial == 204){
        x_inicial = 0;
        initialscore1 = initialscore1 - 50;
        
      }
      else if (x >=206 & x <= 236 & y_inicial >= 150){
        y_inicial = 150;
        
      }
 
      }
      
    
    
  
    int animation2 = (x_inicial/10) % 20;// El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_caminar, 20, animation2, 0, 0);
    V_line(x_inicial, y_inicial, 36, 0x06DF);
    
  }

//Quieto
 else{
    delay(100);
    x = x_inicial;
    if (scene == 2){
      if (x>=0 & x<=16 & y_inicial >= 169){

        y_inicial = 169;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        H_line(x_inicial - 2, y_inicial + 36, 36, 0x06DF);
        H_line(x_inicial - 3, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
          animation3++;
        }
          else{
          animation3 = 0; 
          }
        
      }
    if (x >= 17 & x <= 152 & y_inicial >= 150){
        
        y_inicial = 150;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        
      }
      else if (x >= 152 & x <= 184 & y_inicial > 169){
        y_inicial = 169;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
      }
      else if (x >= 153 & x <= 183 & y_inicial == 169){
        x_inicial = 0;
        initialscore1 = initialscore1 - 50;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        } 
        FillRect(152, 171, 67, 36, 0x06DF);
      } 
      else if (x >= 184 & x <= 284 & y_inicial >= 150){
        y_inicial = 150;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        } 
      } 
      else{

        y_inicial= y_inicial + 3;
        int animationSpeed3 = (y_inicial/3) % 11;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_saltando, 11, animationSpeed3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        H_line(x_inicial - 2, y_inicial + 36, 36, 0x06DF);
        
      }
      }
    
    
    if (scene == 4){
      if (x>=0 & x<=157 & y_inicial >= 150){
        y_inicial = 150;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        H_line(x_inicial - 2, y_inicial + 36, 36, 0x06DF);
        H_line(x_inicial - 3, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
          animation3++;
        }
          else{
          animation3 = 0; 
          }
      }
      else if (x >= 158 & x <= 215 & y_inicial >= 169){
        y_inicial = 169;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        
      }
      else if (x >=216 & x <= 320 & y_inicial >= 150){
        y_inicial = 150;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        
      }
      
      else{

        y_inicial= y_inicial + 3;
        int animationSpeed3 = (y_inicial/3) % 11;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_saltando, 11, animationSpeed3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        H_line(x_inicial - 2, y_inicial + 36, 36, 0x06DF);
        
      }
      }
      else if (scene == 6){
      if (x>=0 & x<=151 & y_inicial >= 150){
        y_inicial = 150;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        H_line(x_inicial - 2, y_inicial + 36, 36, 0x06DF);
        H_line(x_inicial - 3, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
          animation3++;
        }
          else{
          animation3 = 0; 
          }
      }
      else if (x >= 152 & x <= 170 & y_inicial == 204){
        x_inicial = 0;
        initialscore1 = initialscore1 - 50;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        FillRect(153, 190, 47, 49, 0x06DF);
       for (int x_pb = 0; x_pb < 67; x_pb +=33) {
        LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo  );
      }
      for (int x_pb = 0; x_pb < 71; x_pb += 10) {
        LCD_Bitmap(x_pb, 190, 19, 19,piso_alto );
      }
       if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        
      }
      else if (x >=171 & x <= 320 & y_inicial >= 150){
        y_inicial = 150;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        
      }
      
      else{

        y_inicial= y_inicial + 3;
        int animationSpeed3 = (y_inicial/3) % 11;
        LCD_Sprite(x_inicial, y_inicial, 36, 36, kirby_saltando, 11, animationSpeed3, 0, 0);
        H_line(x_inicial - 1, y_inicial + 36, 36, 0x06DF);
        H_line(x_inicial - 2, y_inicial + 36, 36, 0x06DF);
        
      }
      }

  }
 }
  


else if (flag == 3){
  LCD_Clear(0x06DF);
  initialscore2 = 250;
  initialscore3 = 250;
  flag++;
}
 else if (flag == 4){
  unsigned long currentMillis2 = millis();

  if (currentMillis2 - previousMillis2 >= interval) {
    
    previousMillis2 = currentMillis2;

    initialscore2 = initialscore2 - 20;
    initialscore3 = initialscore3 - 20;

  }
 if (scene2 == 1){
 FillRect(0, 0, 160, 240, 0x06DF);
  for (int x_pb = 0; x_pb < 100; x_pb += 33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo );
  }
    LCD_Bitmap(127, 207, 33, 33,piso_bajo );
    
  for (int x_pb = 53; x_pb < 134; x_pb += 10) {
    LCD_Bitmap(x_pb, 190, 19, 19,piso_alto );
  }
  LCD_Bitmap(142, 190, 19, 19, piso_alto );
  LCD_Bitmap(53, 190, 19, 19,esquina_izquierda );

scene2++;
 }

 if (scene3 == 1){
      FillRect(160, 0, 160, 240, 0x06DF);
  for (int x_pb = 160; x_pb < 260; x_pb += 33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo );
  }
    LCD_Bitmap(287, 207, 33, 33,piso_bajo );
    
  for (int x_pb = 213; x_pb < 294; x_pb += 10) {
    LCD_Bitmap(x_pb, 190, 19, 19,piso_alto );
  }
  LCD_Bitmap(302, 190, 19, 19, piso_alto );
  LCD_Bitmap(213, 190, 19, 19,esquina_izquierda );

scene3++;
 }

 if (scene2 == 3){
  FillRect(0, 0, 160, 240, 0x06DF);
  for (int x_pb = 0; x_pb < 100; x_pb += 33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo );
  }
    LCD_Bitmap(127, 207, 33, 33,piso_bajo );
    LCD_Bitmap(0, 190, 19, 19, esquina_derecha );
    LCD_Bitmap(43, 207, 48, 14, agua );

  
scene2++;
 }

 if (scene3 == 3){
  FillRect(160, 0, 160, 240, 0x06DF);  
  for (int x_pb = 160; x_pb < 260; x_pb += 33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo );
  }
    LCD_Bitmap(287, 207, 33, 33,piso_bajo );
    LCD_Bitmap(160, 190, 19, 19, esquina_derecha );
    LCD_Bitmap(203, 207, 48, 14, agua );

  
scene3++;
 }

 if (scene2 == 5){
  FillRect(0, 0, 160, 240, 0x06DF);
  for (int x_pb = 0; x_pb < 100; x_pb += 33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo );
  }
    LCD_Bitmap(127, 207, 33, 33,piso_bajo );
    LCD_Bitmap(141, 190, 19, 19, esquina_izquierda );
    LCD_Bitmap(81, 190, 60, 49, bloque);
 
scene2++;
 }

 if (scene3 == 5){
   FillRect(160, 0, 160, 240, 0x06DF);
  for (int x_pb = 160; x_pb < 260; x_pb += 33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo );
  }
    LCD_Bitmap(287, 207, 33, 33,piso_bajo );
    LCD_Bitmap(301, 190, 19, 19, esquina_izquierda );
    LCD_Bitmap(241, 190, 60, 49, bloque);
  
scene3++;
 }

if (scene2 == 7){
 FillRect(0, 0, 160, 240, 0x06DF);
  for (int x_pb = 0; x_pb < 100; x_pb += 33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo );
  }
    LCD_Bitmap(127, 207, 33, 33,piso_bajo );
 for (int x_pb = 0; x_pb < 134; x_pb += 19) {
    LCD_Bitmap(x_pb, 190, 19, 19,piso_alto );
  }
  LCD_Bitmap(141, 190, 19, 19, piso_alto );
  LCD_Bitmap(20, 190, 54, 49, hoyo);
  LCD_Bitmap(113, 127, 43, 67, meta);
    

  
scene2++;
 }

 if (scene3 == 7){
   FillRect(160, 0, 160, 240, 0x06DF); 
  for (int x_pb = 160; x_pb < 260; x_pb += 33) {
    LCD_Bitmap(x_pb, 207, 33, 33,piso_bajo );
  }
    LCD_Bitmap(287, 207, 33, 33,piso_bajo );
 for (int x_pb = 160; x_pb < 294; x_pb += 19) {
    LCD_Bitmap(x_pb, 190, 19, 19,piso_alto );
  }
  LCD_Bitmap(301, 190, 19, 19, piso_alto );
  LCD_Bitmap(180, 190, 54, 49, hoyo);
  LCD_Bitmap(273, 127, 43, 67, meta);
    
  
scene3++;
 }


//Salto derecha
if  (control1 == 8){
  control1 == 0;
  
  //Kirby fuego 
 int x_1;
 int i = 0;
 int y = 0;
 
   
  x_1 = x_inicial1;
  while (i == 0){
    for (int y = 0; y <26; y++){
    y_inicial1--;
    int animationSpeed = (y_inicial1/10) % 11; // El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    
     LCD_Sprite(x_1,y_inicial1, 36, 36, kirby_saltando, 11, animationSpeed, 0, 0);
     H_line(x_1, y_inicial1 + 37, 36, 0x06DF);
     H_line(x_1, y_inicial1 + 36, 36, 0x06DF);

    }

      i++;
  }
 
for (int i = 0; i<31; i++){
    
  if (x_1 > 124){
      x_1 =0;
      if (scene2==8){
        scene1 = 10;
        flag = 0;
        }
      else{
      initialscore2 = initialscore2 + 250;
      scene2++;}
  }
  else{
    x_1++;
    }
    
    y =(-(x_1-x_inicial1))+(y_inicial1);
    if (y < 0){
      y = 0;
    }
    
    
    int animationSpeed = (x_1/10) % 11; // El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    
    LCD_Sprite(x_1,y, 36, 36, kirby_saltando, 11, animationSpeed, 0, 0);
    
    V_line(x_1 - 1, y +36, 36, 0x06DF);
    H_line(x_1 - 1, y + 36, 36, 0x06DF);

  }

  
                                                    
  y_inicial1 = (-(x_1-x_inicial1))+(y_inicial1);
  x_inicial1 = x_1;
  


 for (int i = 0; i<31; i++){
    x_1 = x_inicial1;
    if (x_inicial1> 124){
      x_inicial1=0;
      if (scene2==8){
        scene1 = 10; 
        flag = 0;
        }
      else{
      initialscore2 = initialscore2 + 250;
      scene2++;}
  }
  else{
    x_inicial1++;
    }
  
    int y =((x_inicial1-x_1))+(y_inicial1);
    
    int animationSpeed2 = (x_inicial1/10) % 11; // El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    
    LCD_Sprite(x_inicial1,y, 36, 36, kirby_saltando, 11, animationSpeed2, 0, 0);
    V_line(x_inicial1 - 1, y, 36, 0x06DF);
    H_line(x_inicial1 - 1, y, 36, 0x06DF);
    
  }
  y_inicial1 = ((x_inicial1-x_1))+(y_inicial1);

 
 }

if  (control2 == 4){
  control2 == 0;

    /*Kirby 2 ********************************************************************************************/
 int x_2;
 int i_2 = 0;
 int y_2 = 0;
 
  
  x_2 = x_inicial2;
  while (i_2 == 0){
    for (int y_2 = 0; y_2 <26; y_2++){
    y_inicial2--;
    int animationSpeed = (y_inicial2/10) % 11; // El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    
     LCD_Sprite(x_2,y_inicial2, 36, 36, kirby_saltando, 11, animationSpeed, 0, 0);
     H_line(x_2, y_inicial2 + 37, 36, 0x06DF);
     H_line(x_2, y_inicial2 + 36, 36, 0x06DF);

    }

      i_2++;
  }
 
for (int i_2 = 0; i_2<31; i_2++){
    
  if (x_2 > 284){
      x_2 =160;
      if (scene3==8){
        scene1 = 11;
        flag = 0;
        }
      else{

      initialscore3 = initialscore3 + 250;
      scene3++;}
  }
  else{
    x_2++;
    }
    
    y_2 =(-(x_2-x_inicial2))+(y_inicial2);
    if (y_2 < 0){
      y_2 = 0;
    }
    
    
    int animationSpeed = (x_2/10) % 11; // El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    
    LCD_Sprite(x_2,y_2, 36, 36, kirby_saltando, 11, animationSpeed, 0, 0);
    
    V_line(x_2 - 1, y_2 +36, 36, 0x06DF);
    H_line(x_2 - 1, y_2 + 36, 36, 0x06DF);

  }

  
                                                    
  y_inicial2 = (-(x_2-x_inicial2))+(y_inicial2);
  x_inicial2 = x_2;
  


 for (int i_2 = 0; i_2<31; i_2++){
    x_2 = x_inicial2;
    if (x_inicial2> 284){
      x_inicial2=160;
      if (scene3==8){
        scene1 = 11;
        flag = 0;
        }
      else{
      initialscore3 = initialscore3 + 250;
      scene3++;}
  }
  else{
    x_inicial2++;
    }
  
    int y_2 =((x_inicial2-x_2))+(y_inicial2);
    
    int animationSpeed2_2 = (x_inicial2/10) % 11; // El módulo debe ser igual a la cantidad de columnas de su sprite. El 
    
    LCD_Sprite(x_inicial2,y_2, 36, 36, kirby_saltando, 11, animationSpeed2_2, 0, 0);
    V_line(x_inicial2- 1, y_2, 36, 0x06DF);
    H_line(x_inicial2 - 1, y_2, 36, 0x06DF);
    
  }
  y_inicial2 = ((x_inicial2-x_2))+(y_inicial2);
 }


  //Adelante
 else if  (control2 == 2){ 
  control2 == 0;
 
  /*Kirby 2 ********************************************************************************************/

 int x_2;
 int y_2;
    
      
      else{

        y_inicial1= y_inicial1 + 3;
        int animationSpeed3 = (y_inicial1/3) % 11;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_saltando, 11, animationSpeed3, 0, 0);
        H_line(x_inicial1- 1, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 2, y_inicial1 + 36, 36, 0x06DF);
        
      }
      }
    
    if (scene2 == 4){
       if (x_1 >= 0 & x_1 <= 19 & y_inicial >= 150){
        y_inicial1 = 150;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial1 - 1, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 2, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 3, y_inicial1 + 36, 36, 0x06DF);
        if(animation3<9){
          animation3++;
        }
          else{
          animation3 = 0; 
          }
        
      }
      else if (x_1 >= 20 & x_1 <= 91 & y_inicial1 > 169){
        
        y_inicial1 = 169;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial1 - 1, y_inicial1 + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        }

        else if (x_1 >= 20 & x_1 <= 80 & y_inicial1 == 169){
        x_inicial1 = 0;
        y_inicial1 = 150;
        initialscore2 = initialscore2 - 50;
        FillRect(20, 172, 90, 36, 0x06DF); 
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial1 - 1, y_inicial1 + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        }


      else if (x_1 >= 81 & x_1 <= 160 & y_inicial1 >= 169){
        
        y_inicial = 169;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial1 - 1, y_inicial1 + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        }
        
      
      else{

        y_inicial1= y_inicial1 + 3;
        int animationSpeed3 = (y_inicial1/3) % 11;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_saltando, 11, animationSpeed3, 0, 0);
        H_line(x_inicial1- 1, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 2, y_inicial1 + 36, 36, 0x06DF);
        
      }
      }

 if (scene2 == 6){
       if (x_1 >= 0 & x_1 <= 44 & y_inicial1 >= 169){
        y_inicial1 = 169;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial1 - 1, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 2, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 3, y_inicial1 + 36, 36, 0x06DF);
        if(animation3<9){
          animation3++;
        }
          else{
          animation3 = 0; 
          }
        
      }
      else if (x_1 >= 45 & x_1 <= 160 & y_inicial1 >= 150){
        
        y_inicial1 = 150;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial1 - 1, y_inicial1 + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        }

        
      else{

        y_inicial1= y_inicial1 + 3;
        int animationSpeed3 = (y_inicial1/3) % 11;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_saltando, 11, animationSpeed3, 0, 0);
        H_line(x_inicial1- 1, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 2, y_inicial1 + 36, 36, 0x06DF);
        
      }
      }

 if (scene2 == 8){
       if (x_1 >= 0 & x_1 <= 20 & y_inicial1 >= 150){
        y_inicial1 = 150;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial1 - 1, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 2, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 3, y_inicial1 + 36, 36, 0x06DF);
        if(animation3<9){
          animation3++;
        }
          else{
          animation3 = 0; 
          }
        
      }
      else if (x_1 >= 21 & x_1 <= 38 & y_inicial1 >= 204){
        x_inicial1 = 0;
        y_inicial1 = 150;
        initialscore2 = initialscore2 - 50;
        FillRect(22, 190, 50, 49, 0x06DF); 
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial1 - 1, y_inicial1 + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        }
        else if (x_1 >= 39 & x_1 <= 160 & y_inicial1 >= 150){
        
        y_inicial1 = 150;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial1 - 1, y_inicial1 + 36, 36, 0x06DF);
        if(animation3<9){
        animation3++;
        }
        else{
         animation3 = 0; 
        }
        }
        
      else{

        y_inicial1= y_inicial1 + 3;
        int animationSpeed3 = (y_inicial1/3) % 11;
        LCD_Sprite(x_inicial1, y_inicial1, 36, 36, kirby_saltando, 11, animationSpeed3, 0, 0);
        H_line(x_inicial1- 1, y_inicial1 + 36, 36, 0x06DF);
        H_line(x_inicial1 - 2, y_inicial1 + 36, 36, 0x06DF);
        
      }
      }
/*Kirby2******************************************************************/
   int y_2 = 0;
   int x_2 = 0;
    delay(100);
    x_2 = x_inicial2;
    if (scene3 == 2){
       if (x_2 >= 160 & x_2 <= 177 & y_inicial2 >= 169){
        y_inicial2 = 169;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_quieto, 8, animation3_2, 0, 0);
        H_line(x_inicial2 - 1, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 2, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 3, y_inicial2 + 36, 36, 0x06DF);
        if(animation3_2<9){
          animation3_2++;
        }
          else{
          animation3_2 = 0; 
          }
        
      }
      else if (x_2 >= 178 & x_2 <= 320 & y_inicial2 >= 150){
        
        y_inicial2 = 150;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_quieto, 8, animation3_2, 0, 0);
        H_line(x_inicial2 - 1, y_inicial2 + 36, 36, 0x06DF);
        if(animation3_2<9){
        animation3_2++;
        }
        else{
         animation3_2 = 0; 
        }
        }
        
      
      else{

        y_inicial2= y_inicial2 + 3;
        int animationSpeed3_2 = (y_inicial2/3) % 11;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_saltando, 11, animationSpeed3_2, 0, 0);
        H_line(x_inicial2- 1, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 2, y_inicial2 + 36, 36, 0x06DF);
        
      }
      }
    
    if (scene3 == 4){
       if (x_2 >= 160 & x_2 <= 179 & y_inicial2 >= 150){
        y_inicial2 = 150;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_quieto, 8, animation3_2, 0, 0);
        H_line(x_inicial2 - 1, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 2, y_inicial2+ 36, 36, 0x06DF);
        H_line(x_inicial2 - 3, y_inicial2 + 36, 36, 0x06DF);
        if(animation3_2<9){
          animation3_2++;
        }
          else{
          animation3_2 = 0; 
          }
        
      }
           else if (x_2 >= 241 & x_2 <= 320 & y_inicial2 >= 169){
        
        y_inicial2 = 169;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_quieto, 8, animation3_2, 0, 0);
        H_line(x_inicial2 - 1, y_inicial2 + 36, 36, 0x06DF);
        if(animation3_2<9){
        animation3_2++;
        }
        else{
         animation3_2 = 0; 
        }
        }
        
      
      else{

        y_inicial2= y_inicial2 + 3;
        int animationSpeed3_2 = (y_inicial2/3) % 11;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_saltando, 11, animationSpeed3_2, 0, 0);
        H_line(x_inicial2- 1, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 2, y_inicial2 + 36, 36, 0x06DF);
        
      }
      }

 if (scene3 == 6){
       if (x_2 >= 160 & x_2 <= 204 & y_inicial2 >= 169){
        y_inicial2 = 169;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_quieto, 8, animation3_2, 0, 0);
        H_line(x_inicial2 - 1, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 2, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 3, y_inicial2 + 36, 36, 0x06DF);
        if(animation3_2<9){
          animation3_2++;
        }
          else{
          animation3_2 = 0; 
          }
        
      }
      else if (x_1 >= 205 & x_1 <= 320 & y_inicial1 >= 150){
        
        y_inicial1 = 150;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_quieto, 8, animation3, 0, 0);
        H_line(x_inicial2 - 1, y_inicial2 + 36, 36, 0x06DF);
        if(animation3_2<9){
        animation3_2++;
        }
        else{
         animation3_2 = 0; 
        }
        }

        
      else{

        y_inicial2= y_inicial2 + 3;
        int animationSpeed3_2 = (y_inicial1/3) % 11;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_saltando, 11, animationSpeed3_2, 0, 0);
        H_line(x_inicial2- 1, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 2, y_inicial2 + 36, 36, 0x06DF);
        
      }
      }

 if (scene3 == 8){
       if (x_2 >= 160 & x_1 <= 180 & y_inicial >= 150){
        y_inicial2 = 150;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_quieto, 8, animation3_2, 0, 0);
        H_line(x_inicial2 - 1, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 2, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 3, y_inicial2 + 36, 36, 0x06DF);
        if(animation3_2<9){
          animation3_2++;
        }
          else{
          animation3_2 = 0; 
          }
        
      }
      else if (x_2 >= 181 & x_2 <= 198 & y_inicial2 >= 204){
        x_inicial2 = 160;
        y_inicial2 = 150;
        initialscore3 = initialscore3 - 50;
        FillRect(182, 190, 50, 49, 0x06DF); 
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_quieto, 8, animation3_2, 0, 0);
        H_line(x_inicial2 - 1, y_inicial2 + 36, 36, 0x06DF);
        if(animation3_2<9){
        animation3_2++;
        }
        else{
         animation3_2 = 0; 
        }
        }
        else if (x_2 >= 189 & x_2 <= 320 & y_inicial2 >= 150){
        
        y_inicial2 = 150;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_quieto, 8, animation3_2, 0, 0);
        H_line(x_inicial2 - 1, y_inicial2 + 36, 36, 0x06DF);
        if(animation3_2<9){
        animation3_2++;
        }
        else{
         animation3_2 = 0; 
        }
        }
        
      else{

        y_inicial2= y_inicial2 + 3;
        int animationSpeed3_2 = (y_inicial2/3) % 11;
        LCD_Sprite(x_inicial2, y_inicial2, 36, 36, kirby_saltando, 11, animationSpeed3_2, 0, 0);
        H_line(x_inicial2- 1, y_inicial2 + 36, 36, 0x06DF);
        H_line(x_inicial2 - 2, y_inicial2 + 36, 36, 0x06DF);
        
      }
      }
  }
 
}
}


void LoadSD(File &file, int data[])
{
  SPI.setModule(0);
  int dato;
  int cont;
  int sign;
  int scores[5];
  int bits[3] = {0,0,0};
      
while (file.available()) {       
for (int x = 4; x >= 0; x--){
            int numero1 = file.read();
            if (numero1 < 0x7B & numero1>0x2C){
            int num1 = numero1 - 48;
            int numero2 = file.read();
            int num2 = numero2 - 48;
            int numero3 = file.read();
            int num3 = numero3 - 48;
            int numero4 = file.read();
            bits[0] = num1;
            bits[1] = num2;
            bits[2] = num3;
          
            
            dato = bits[0]*100;
            dato = dato + (bits[1]*10);
            dato = dato + (bits[2]);

            
            }
            else{
                x++;
              }
              scores[x] = dato;
              data[x] = dato;
              Serial.print("dato x:");
              Serial.print(x);
              Serial.print("\n");
              Serial.print("dato score:");
              Serial.print(scores[x]);
              Serial.print("\n");
           
    }
    file.close();
  }


  }
//**************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
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
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
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
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
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
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
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
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
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
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
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
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
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
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
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
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
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
