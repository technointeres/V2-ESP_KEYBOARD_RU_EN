#include <Adafruit_GFX.h>    
#include <Arduino_ST7789.h> 
#include <SPI.h>
#include "FreeMono12pt7b.h" //THE FONT FILE MUST BE IN THE FOLDER

#define TFT_DC    1 //PIN
#define TFT_RST   0 //PIN
#define TFT_CS    A0 //VIRTUAL PIN?
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_CS);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() { 
  pinMode(7, OUTPUT);  //BL PIN                                
  analogWrite(7, 110); //DISPLAY BRIGHTNESS
  pinMode(3, OUTPUT);  //RU PIN
  analogWrite(3, 200); //RU BRIGHTNESS
  pinMode(10, OUTPUT); //ENG PIN
  analogWrite(10, 200);//ENG BRIGHTNESS
  
  tft.setRotation(1);
  tft.init(240, 240); 
  tft.fillScreen(color565(17,17,155));
  tft.fillRect(40,111, 40,17, color565(55,255,55));    //RECT
  tft.drawCircle(120,120, 119, color565(255,255,255)); //CIRCLE


    tft.setTextWrap(false);
    tft.setCursor(80, 20);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println("Hello World!"); //DEFAULT FONT

                   tft.setFont(&FreeMono12pt7b); //OPTIONAL FONT //THE FONT FILE MUST BE IN THE FOLDER
                   tft.setTextWrap(true);
                   tft.setCursor(40, 55);
                   tft.setTextColor(color565(255,55,55));
                   tft.setTextSize(1, 1);
                   tft.print("Hello again!");
             }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

            }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////                    
uint16_t color565( byte r, byte g, byte b ) {
  return ( ( r & 0xF8 ) << 8 ) | ( ( g & 0xFC ) << 3 ) | ( ( b & 0xF8 ) >> 3 );
                                            }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
