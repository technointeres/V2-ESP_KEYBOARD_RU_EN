#include <Adafruit_GFX.h>    
#include <Arduino_ST7789.h> 
#include <SPI.h>
#include <Wire.h>
#include <ADS1115_lite.h>
#include "NOTO_FONT_ALL.h"

#define TFT_DC    1 //PIN
#define TFT_RST   0 //PIN
#define TFT_CS    A0 //VIRTUAL PIN?
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_CS);

ADS1115_lite ads(ADS1115_DEFAULT_ADDRESS);

int h = 27-1;//HEIGHT IMAGE 
int w = 2436;//WIDTH IMAGE  
bool stringComplete = false; 
float raw0 = 0;
float raw1 = 0;
float raw2 = 0;
float raw3 = 0;
float raw0pre = 0;
float raw1pre = 0;
float raw2pre = 0;
float raw3pre = 0;
float top_resolution = 1;
double resolution = 0;
float result0;
float result1;
float result2;
float result3;
float button_space0 = 1;
float numButton0 = -1;
float button_space1 = 1;
float numButton1 = -1;
float button_space2 = 1;
float numButton2 = -1;
float button_space3 = 1;
float numButton3 = -1;
char previous_char;
int startX = 2; 
int startY = 0;
int scaleX = 1; // >=1
int scaleY = 1; // >=1
double timeCursor;
String MyText = "";
String substrSpecific; 
String noteStr;
unsigned long timePast = 0;
uint16_t charColo = WHITE;
uint16_t BackColor;
//int BRIG = -1;
//int BRIG_LAYOUT = -1;
int layout = 1;

void setup()   { 
  pinMode(7, OUTPUT); //BL PIN                                
  analogWrite(7, 110); //BL PIN = BRIGHTNESS
  pinMode(3, OUTPUT); //RU PIN
  analogWrite(3, 0);
  pinMode(10, OUTPUT); //ENG PIN
  analogWrite(10, 200);

  tft.init(240, 240);   
  uint16_t time = millis();
  ads_config();
               }        
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
             NOTE();
            }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PressedButton(int which) {
  int ix0 = 0, ix1 = 0, ix2 = 0, ix3 = 0;
  int times = 2; //HOW MANY
  int incdec = 500;
  
  if(which == 0) {//CHECK ONLY 1 CIRCUIT [LAYOUT, ESC, SHIFT, EXTRA, BACKSPACE, ENTER, <>, MENU, UP, DOWN]
  raw0=ads_read0();
  raw1=ads_read1();
  raw2=ads_read2();
  raw3=ads_read3();
  top_resolution = max(max(raw0,raw1),max(raw2,raw3));
  button_space1 = top_resolution / 11; //11 BUTTONS
  numButton1 = round((raw1-incdec) / button_space1); 
    while(ix1 < times) {raw1=ads_read1();
                        if(round((raw1-incdec) / button_space1) != numButton1) {break;}
                        ix1++; }   
                        if(ix1 < times || numButton1 ==  11) {numButton1 = -1;} //11 BUTTONS 
                 }
                 else {//CHECK ALL KEYS WHICH CONNECTED TO ADS1115
                 
  raw0=ads_read0();
  raw1=ads_read1();
  raw2=ads_read2();
  raw3=ads_read3();
  top_resolution = max(max(raw0,raw1),max(raw2,raw3));
  button_space0 = top_resolution / 11; //11 BUTTONS
  numButton0 = round((raw0-incdec) / button_space0);    
  button_space1 = top_resolution / 11; //11 BUTTONS
  numButton1 = round((raw1-incdec) / button_space1);
  button_space2 = top_resolution / 11; //11 BUTTONS
  numButton2 = round((raw2-incdec) / button_space2);
  button_space3 = top_resolution / 12; //12 BUTTONS
  numButton3 = round((raw3-incdec) / button_space3);  
           
   ix0 = 0; ix1 = 0; ix2 = 0; ix3 = 0;
   
  while(ix0 < times) {raw0=ads_read0();
                      if(round((raw0-incdec) / button_space0) != numButton0) {break;}
                      ix0++; }   
                      if(ix0 < times || numButton0 ==  11) {numButton0 = -1;} //11 BUTTONS
  while(ix1 < times) {raw1=ads_read1();
                      if(round((raw1-incdec) / button_space1) != numButton1) {break;}
                      ix1++; }    
                      if(ix1 < times || numButton1 ==  11) {numButton1 = -1;} //11 BUTTONS
  while(ix2 < times) {raw2=ads_read2();
                      if(round((raw2-incdec) / button_space2) != numButton2) {break;}
                      ix2++; }   
                      if(ix2 < times || numButton2 ==  11) {numButton2 = -1;} //11 BUTTONS
  while(ix3 < times) {raw3=ads_read3();
                      if(round((raw3-incdec) / button_space3) != numButton3) {break;}
                      ix3++; }   
                      if(ix3 < times || numButton3 ==  12) {numButton3 = -1;} //12 BUTTONS   
                     }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////                    
uint16_t color565( byte r, byte g, byte b ) {
  return ( ( r & 0xF8 ) << 8 ) | ( ( g & 0xFC ) << 3 ) | ( ( b & 0xF8 ) >> 3 );
                                            }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////  

void NOTE() { 
  uint16_t wall; 
  wall = color565(100,0,30); tft.fillScreen(wall);
  uint16_t curCol = color565(0,200,250); //CURSOR
  int curW = 8, curH = 22;                                               
  startX = 2; startY = 0; 
  int add = 0; int prevX, prevY;//ADD = SHIFT Y
  noteStr = "";
  charColo = WHITE; startX = 2; startY = 0;
  tft.fillRect(startX, startY+2, 8, 22, curCol);
  numButton1 = -1; 
  timeCursor = millis();
  PressedButton(0);
 
  while(numButton1 != 10 && numButton1 != 5) {  
                                                              
    PressedButton(1); 
                                         if(millis() - timeCursor < 150) {continue;}//PAUSE FOR COMFORT
                                         if(numButton0 == -1 &&
                                            numButton1 == -1 && 
                                            numButton2 == -1 &&
                                            numButton3 == -1   ) {continue;} //NOTHING PRESSED
                                         
                                         if(numButton0 != -1 || numButton2 != -1 || numButton3 != -1) {timeCursor = millis();}//PAUSE ONLY IF SYMBOL KEY PRESSED
 
if((numButton1 == 7 || numButton1 == 8) && (numButton0 == -1 && numButton2 == -1 && numButton3 == -1)) {goto forfaster;}//SHIFT OR EXTRA AND NOTHING ELSE
 
if(layout == 1) {                       
    if(numButton0 == 10){MyText = "1"; if(numButton1 == 7) {MyText = "1";} if(numButton1 == 8) {MyText = "1";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 9) {MyText = "q"; if(numButton1 == 7) {MyText = "Q";} if(numButton1 == 8) {MyText = "2";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 8) {MyText = "w"; if(numButton1 == 7) {MyText = "W";} if(numButton1 == 8) {MyText = "3";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 7) {MyText = "e"; if(numButton1 == 7) {MyText = "E";} if(numButton1 == 8) {MyText = "4";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 6) {MyText = "r"; if(numButton1 == 7) {MyText = "R";} if(numButton1 == 8) {MyText = "5";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton0 == 5) {MyText = "t"; if(numButton1 == 7) {MyText = "T";} if(numButton1 == 8) {MyText = "6";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 4) {MyText = "y"; if(numButton1 == 7) {MyText = "Y";} if(numButton1 == 8) {MyText = "7";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton0 == 3) {MyText = "u"; if(numButton1 == 7) {MyText = "U";} if(numButton1 == 8) {MyText = "8";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 2) {MyText = "i"; if(numButton1 == 7) {MyText = "I";} if(numButton1 == 8) {MyText = "9";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton0 == 1) {MyText = "o"; if(numButton1 == 7) {MyText = "O";} if(numButton1 == 8) {MyText = "0";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 0) {MyText = "p"; if(numButton1 == 7) {MyText = "P";} if(numButton1 == 8) {MyText = "-";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
        
    if(numButton2 == 10){MyText = "a"; if(numButton1 == 7) {MyText = "A";} if(numButton1 == 8) {MyText = "\134";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 9) {MyText = "s"; if(numButton1 == 7) {MyText = "S";} if(numButton1 == 8) {MyText = "/";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 8) {MyText = "d"; if(numButton1 == 7) {MyText = "D";} if(numButton1 == 8) {MyText = "@";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 7) {MyText = "f"; if(numButton1 == 7) {MyText = "F";} if(numButton1 == 8) {MyText = ":";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 6) {MyText = "g"; if(numButton1 == 7) {MyText = "G";} if(numButton1 == 8) {MyText = ";";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton2 == 5) {MyText = "h"; if(numButton1 == 7) {MyText = "H";} if(numButton1 == 8) {MyText = "%";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 4) {MyText = "j"; if(numButton1 == 7) {MyText = "J";} if(numButton1 == 8) {MyText = "\42";}  noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton2 == 3) {MyText = "k"; if(numButton1 == 7) {MyText = "K";} if(numButton1 == 8) {MyText = "*";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 2) {MyText = "l"; if(numButton1 == 7) {MyText = "L";} if(numButton1 == 8) {MyText = "(";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton2 == 1) {MyText = ")"; if(numButton1 == 7) {MyText = ")";} if(numButton1 == 8) {MyText = ")";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 0) {MyText = "!"; if(numButton1 == 7) {MyText = "!";} if(numButton1 == 8) {MyText = "!";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 

    if(numButton3 == 11){MyText = "z";   if(numButton1 == 7) {MyText = "Z";}   if(numButton1 == 8) {MyText = "<";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 10){MyText = "x";   if(numButton1 == 7) {MyText = "X";}   if(numButton1 == 8) {MyText = ">";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 9) {MyText = "c";   if(numButton1 == 7) {MyText = "C";}   if(numButton1 == 8) {MyText = "[";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 8) {MyText = "\40"; if(numButton1 == 7) {MyText = "_";}   if(numButton1 == 8) {MyText = "|";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 7) {MyText = "v";   if(numButton1 == 7) {MyText = "V";}   if(numButton1 == 8) {MyText = "]";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton3 == 6) {MyText = "b";   if(numButton1 == 7) {MyText = "B";}   if(numButton1 == 8) {MyText = "{";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 5) {MyText = "n";   if(numButton1 == 7) {MyText = "N";}   if(numButton1 == 8) {MyText = "}";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton3 == 4) {MyText = "m";   if(numButton1 == 7) {MyText = "M";}   if(numButton1 == 8) {MyText = "?";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 3) {MyText = ",";   if(numButton1 == 7) {MyText = ",";}   if(numButton1 == 8) {MyText = ",";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton3 == 2) {MyText = ".";   if(numButton1 == 7) {MyText = ".";}   if(numButton1 == 8) {MyText = ".";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 1) {MyText = "=";   if(numButton1 == 7) {MyText = "=";}   if(numButton1 == 8) {MyText = "=";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}  
    if(numButton3 == 0) {MyText = "+";   if(numButton1 == 7) {MyText = "+";}   if(numButton1 == 8) {MyText = "+";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}                             
                }
if(layout == 2) {                       
    if(numButton0 == 10){MyText = "ё"; if(numButton1 == 7) {MyText = "Ё";} if(numButton1 == 8) {MyText = "1";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 9) {MyText = "й"; if(numButton1 == 7) {MyText = "Й";} if(numButton1 == 8) {MyText = "2";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 8) {MyText = "ц"; if(numButton1 == 7) {MyText = "Ц";} if(numButton1 == 8) {MyText = "3";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 7) {MyText = "у"; if(numButton1 == 7) {MyText = "У";} if(numButton1 == 8) {MyText = "4";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 6) {MyText = "к"; if(numButton1 == 7) {MyText = "К";} if(numButton1 == 8) {MyText = "5";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton0 == 5) {MyText = "е"; if(numButton1 == 7) {MyText = "Е";} if(numButton1 == 8) {MyText = "6";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 4) {MyText = "н"; if(numButton1 == 7) {MyText = "Н";} if(numButton1 == 8) {MyText = "7";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton0 == 3) {MyText = "г"; if(numButton1 == 7) {MyText = "Г";} if(numButton1 == 8) {MyText = "8";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 2) {MyText = "ш"; if(numButton1 == 7) {MyText = "Ш";} if(numButton1 == 8) {MyText = "9";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton0 == 1) {MyText = "щ"; if(numButton1 == 7) {MyText = "Щ";} if(numButton1 == 8) {MyText = "0";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton0 == 0) {MyText = "з"; if(numButton1 == 7) {MyText = "З";} if(numButton1 == 8) {MyText = "-";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
        
    if(numButton2 == 10){MyText = "ф"; if(numButton1 == 7) {MyText = "Ф";} if(numButton1 == 8) {MyText = "\134";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 9) {MyText = "ы"; if(numButton1 == 7) {MyText = "Ы";} if(numButton1 == 8) {MyText = "/";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 8) {MyText = "в"; if(numButton1 == 7) {MyText = "В";} if(numButton1 == 8) {MyText = "@";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 7) {MyText = "а"; if(numButton1 == 7) {MyText = "А";} if(numButton1 == 8) {MyText = ":";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 6) {MyText = "п"; if(numButton1 == 7) {MyText = "П";} if(numButton1 == 8) {MyText = ";";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton2 == 5) {MyText = "р"; if(numButton1 == 7) {MyText = "Р";} if(numButton1 == 8) {MyText = "%";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 4) {MyText = "о"; if(numButton1 == 7) {MyText = "О";} if(numButton1 == 8) {MyText = "\42";}  noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton2 == 3) {MyText = "л"; if(numButton1 == 7) {MyText = "Л";} if(numButton1 == 8) {MyText = "*";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 2) {MyText = "д"; if(numButton1 == 7) {MyText = "Д";} if(numButton1 == 8) {MyText = "(";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton2 == 1) {MyText = "ж"; if(numButton1 == 7) {MyText = "Ж";} if(numButton1 == 8) {MyText = ")";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton2 == 0) {MyText = "э"; if(numButton1 == 7) {MyText = "Э";} if(numButton1 == 8) {MyText = "!";}    noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 

    if(numButton3 == 11){MyText = "я";   if(numButton1 == 7) {MyText = "Я";} if(numButton1 == 8) {MyText = "<";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 10){MyText = "ч";   if(numButton1 == 7) {MyText = "Ч";} if(numButton1 == 8) {MyText = ">";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 9) {MyText = "с";   if(numButton1 == 7) {MyText = "С";} if(numButton1 == 8) {MyText = "[";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 8) {MyText = "\40"; if(numButton1 == 7) {MyText = "_";} if(numButton1 == 8) {MyText = "|";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 7) {MyText = "м";   if(numButton1 == 7) {MyText = "М";} if(numButton1 == 8) {MyText = "]";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton3 == 6) {MyText = "и";   if(numButton1 == 7) {MyText = "И";} if(numButton1 == 8) {MyText = "{";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 5) {MyText = "т";   if(numButton1 == 7) {MyText = "Т";} if(numButton1 == 8) {MyText = "}";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton3 == 4) {MyText = "ь";   if(numButton1 == 7) {MyText = "Ь";} if(numButton1 == 8) {MyText = "?";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 3) {MyText = "б";   if(numButton1 == 7) {MyText = "Б";} if(numButton1 == 8) {MyText = ",";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;} 
    if(numButton3 == 2) {MyText = "ю";   if(numButton1 == 7) {MyText = "Ю";} if(numButton1 == 8) {MyText = ".";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
    if(numButton3 == 1) {MyText = "ъ";   if(numButton1 == 7) {MyText = "Ъ";} if(numButton1 == 8) {MyText = "=";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}  
    if(numButton3 == 0) {MyText = "х";   if(numButton1 == 7) {MyText = "Х";} if(numButton1 == 8) {MyText = "+";} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}                             
                }
                forfaster:
           
    if(numButton1 == 6) {if(noteStr.charAt(noteStr.length()-2) == '\320' ||  //IF BACKSPACE
                            noteStr.charAt(noteStr.length()-2) == '\321'   ) { tft.fillRect(startX,startY+2,curW,curH,wall); //DELETE CURSOR                   
                             MyText = noteStr.substring(noteStr.length()-2), noteStr.substring(noteStr.length()-1); //RU CHAR
                                                                           charColo = wall;
                                                                           if(startX < 3 && startY > 1+add) {startX = 223; startY-=22; event();} //MOVE CURSOR TO CLEAR SYMBOL[BACKGROUND COLOR]
                                                                           else{if(startX < 3 && startY < 1+add) {startX = 2; startY = 0+add; event();}}
                                                                           if(startX > 2) {startX-=13; event();}
                                                                           if(startX < 3 && startY > 1+add) {startX = 223; startY-=22;         } //MOVE CURSOR AGAIN TO PRINT ANY NEW SYMBOL
                                                                           else{if(startX < 3 && startY < 1+add) {startX = 2;   startY = 0+add;         }}
                                                                           if(startX > 2) {startX-=13;         }
                             noteStr.remove(noteStr.length()-2,2); //REMOVE LAST RU CHAR
                                                                               tft.fillRect(startX,startY+2,curW,curH,curCol); //REDRAW CURSOR 
                                                                             } 
                                        /////////////////////////////////////////////////////////////                                                   
                                                                        else { tft.fillRect(startX,startY+2,curW,curH,wall); //DELETE CURSOR
                             MyText = noteStr.charAt(noteStr.length()-1);                                          //ENG CHAR OR SYMBOL
                                                                           charColo = wall;
                                                                           if(startX < 3 && startY > 1+add) { startX = 223; startY-=22; event();} //MOVE CURSOR TO CLEAR SYMBOL[BACKGROUND COLOR]
                                                                           else{if(startX < 3 && startY < 1+add) {startX = 2; startY = 0+add; event();}}
                                                                           if(startX > 2) {startX-=13; event();}
                                                                           if(startX < 3 && startY > 1+add) {startX = 223; startY-=22;         } //MOVE CURSOR AGAIN TO PRINT ANY NEW SYMBOL
                                                                           else{if(startX < 3 && startY < 1+add) {startX = 2;   startY = 0+add;         }}
                                                                           if(startX > 2) {startX-=13;         }                                             
                             noteStr.remove(noteStr.length()-1,1);   //REMOVE LAST OTHER CHAR  
                                                                                    tft.fillRect(startX,startY+2,curW,curH,curCol); //REDRAW CURSOR                                             
                                                                             }
     MyText = noteStr; //event();
                        } charColo = WHITE;

//                  
    if(numButton1 == 9 && layout == 1) {analogWrite(3, 200); analogWrite(10, 0);   layout = 2; delay(350); numButton1 = -1;}
    if(numButton1 == 9 && layout == 2) {analogWrite(3, 0);   analogWrite(10, 200); layout = 1; delay(350); numButton1 = -1;}
                       
                         }//END WHILE 
                           
            }//END NOTE();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void event() {
int charByteStart, charByteEnd, dobByte, strLen = MyText.length();
for (charByteStart=0; charByteStart<strLen+0; charByteStart+=dobByte) {
                        if(MyText.charAt(charByteStart) == '\320' ||
                           MyText.charAt(charByteStart) == '\321') {dobByte=2;}//+BYTE IF RUSSIAN   
                                                              else {dobByte=1;}    
                                                           charByteEnd=charByteStart+dobByte;        
substrSpecific = MyText.substring(charByteStart, charByteEnd);
                                                           chooseSymbol();
                                                                      }
             } 
///////////////////////////////////////////////////////////////////////////////
void symbolDraw(int shift){
  int row = 0, col = 0, buffidx=0; int16_t colo; //int h = 27-1, w = 2436; 
  int symbolW = 14, symbolWidth = 1;
                                    if(startX > (240-(symbolW*scaleX))) {startX = 2; startY = startY+(h*scaleY)+(-4);}//-X =BETWEEN ROWS
  for (col=startY; col<h*scaleY+startY; col+=scaleY) {  symbolWidth = 1;                                                 
  for (row=startX; row<w*scaleX+startX; row+=scaleX) { 
      colo = pgm_read_word(NOTO_FONT_ALL + buffidx + shift*symbolW); //COLOR EVERY POINT OF BITMAP
                                                      symbolWidth++;
      if(colo != -1) {//-1 IF INT16_T //65535 IF UINT16_T //NOT DRAW WHITE PIXEL
                      //if (originalCol != true) {colo = charColo;}//IF YOU NEED A COLOR NOT EXISTING IN THE BITMAP, BUT WITHOUT HALFOTONES
                      tft.fillRect(row,col,scaleX,scaleY,charColo); 
                      //tft.drawPixel(row,col,charColo);
                     }
      buffidx++;
                      if(symbolWidth > (symbolW)) {buffidx = buffidx + (w - symbolW); break;}                                 
                                                     }    
                                                     }
                                                      startX = row; 
                           }
///////////////////////////////////////////////////////////////////////////////  
void chooseSymbol() {
//if(substrSpecific == "\40") {startX+=15; return;}
if(substrSpecific == "а") {symbolDraw(0); return;}
if(substrSpecific == "б") {symbolDraw(1); return;}
if(substrSpecific == "в") {symbolDraw(2); return;}
if(substrSpecific == "г") {symbolDraw(3); return;}
if(substrSpecific == "д") {symbolDraw(4); return;}
if(substrSpecific == "е") {symbolDraw(5); return;}
if(substrSpecific == "ё") {symbolDraw(6); return;}
if(substrSpecific == "ж") {symbolDraw(7); return;}
if(substrSpecific == "з") {symbolDraw(8); return;}
if(substrSpecific == "и") {symbolDraw(9); return;}
if(substrSpecific == "й") {symbolDraw(10); return;}
if(substrSpecific == "к") {symbolDraw(11); return;}
if(substrSpecific == "л") {symbolDraw(12); return;}
if(substrSpecific == "м") {symbolDraw(13); return;}
if(substrSpecific == "н") {symbolDraw(14); return;}
if(substrSpecific == "о") {symbolDraw(15); return;}
if(substrSpecific == "п") {symbolDraw(16); return;}
if(substrSpecific == "р") {symbolDraw(17); return;}
if(substrSpecific == "с") {symbolDraw(18); return;}
if(substrSpecific == "т") {symbolDraw(19); return;}
if(substrSpecific == "у") {symbolDraw(20); return;}
if(substrSpecific == "ф") {symbolDraw(21); return;}
if(substrSpecific == "х") {symbolDraw(22); return;}
if(substrSpecific == "ц") {symbolDraw(23); return;}
if(substrSpecific == "ч") {symbolDraw(24); return;}
if(substrSpecific == "ш") {symbolDraw(25); return;}
if(substrSpecific == "щ") {symbolDraw(26); return;}
if(substrSpecific == "ъ") {symbolDraw(27); return;}
if(substrSpecific == "ы") {symbolDraw(28); return;}
if(substrSpecific == "ь") {symbolDraw(29); return;}
if(substrSpecific == "э") {symbolDraw(30); return;}
if(substrSpecific == "ю") {symbolDraw(31); return;}
if(substrSpecific == "я") {symbolDraw(32); return;}

if(substrSpecific == "a") {symbolDraw(35); return;}
if(substrSpecific == "b") {symbolDraw(36); return;}
if(substrSpecific == "c") {symbolDraw(37); return;}
if(substrSpecific == "d") {symbolDraw(38); return;}
if(substrSpecific == "e") {symbolDraw(39); return;}
if(substrSpecific == "f") {symbolDraw(40); return;}
if(substrSpecific == "g") {symbolDraw(41); return;}
if(substrSpecific == "h") {symbolDraw(42); return;}
if(substrSpecific == "i") {symbolDraw(43); return;}
if(substrSpecific == "j") {symbolDraw(44); return;}
if(substrSpecific == "k") {symbolDraw(45); return;}
if(substrSpecific == "l") {symbolDraw(46); return;}
if(substrSpecific == "m") {symbolDraw(47); return;}
if(substrSpecific == "n") {symbolDraw(48); return;}
if(substrSpecific == "o") {symbolDraw(49); return;}
if(substrSpecific == "p") {symbolDraw(50); return;}
if(substrSpecific == "q") {symbolDraw(51); return;}
if(substrSpecific == "r") {symbolDraw(52); return;}
if(substrSpecific == "s") {symbolDraw(53); return;}
if(substrSpecific == "t") {symbolDraw(54); return;}
if(substrSpecific == "u") {symbolDraw(55); return;}
if(substrSpecific == "v") {symbolDraw(56); return;}
if(substrSpecific == "w") {symbolDraw(57); return;}
if(substrSpecific == "x") {symbolDraw(58); return;}
if(substrSpecific == "y") {symbolDraw(59); return;}
if(substrSpecific == "z") {symbolDraw(60); return;}

if(substrSpecific == "А") {symbolDraw(63); return;}
if(substrSpecific == "Б") {symbolDraw(64); return;}
if(substrSpecific == "В") {symbolDraw(65); return;}
if(substrSpecific == "Г") {symbolDraw(66); return;}
if(substrSpecific == "Д") {symbolDraw(67); return;}
if(substrSpecific == "Е") {symbolDraw(68); return;}
if(substrSpecific == "Ё") {symbolDraw(69); return;}
if(substrSpecific == "Ж") {symbolDraw(70); return;}
if(substrSpecific == "З") {symbolDraw(71); return;}
if(substrSpecific == "И") {symbolDraw(72); return;}
if(substrSpecific == "Й") {symbolDraw(73); return;}
if(substrSpecific == "К") {symbolDraw(74); return;}
if(substrSpecific == "Л") {symbolDraw(75); return;}
if(substrSpecific == "М") {symbolDraw(76); return;}
if(substrSpecific == "Н") {symbolDraw(77); return;}
if(substrSpecific == "О") {symbolDraw(78); return;}
if(substrSpecific == "П") {symbolDraw(79); return;}
if(substrSpecific == "Р") {symbolDraw(80); return;}
if(substrSpecific == "С") {symbolDraw(81); return;}
if(substrSpecific == "Т") {symbolDraw(82); return;}
if(substrSpecific == "У") {symbolDraw(83); return;}
if(substrSpecific == "Ф") {symbolDraw(84); return;}
if(substrSpecific == "Х") {symbolDraw(85); return;}
if(substrSpecific == "Ц") {symbolDraw(86); return;}
if(substrSpecific == "Ч") {symbolDraw(87); return;}
if(substrSpecific == "Ш") {symbolDraw(88); return;}
if(substrSpecific == "Щ") {symbolDraw(89); return;}
if(substrSpecific == "Ъ") {symbolDraw(90); return;}
if(substrSpecific == "Ы") {symbolDraw(91); return;}
if(substrSpecific == "Ь") {symbolDraw(92); return;}
if(substrSpecific == "Э") {symbolDraw(93); return;}
if(substrSpecific == "Ю") {symbolDraw(94); return;}
if(substrSpecific == "Я") {symbolDraw(95); return;}

if(substrSpecific == "A") {symbolDraw(98); return;}
if(substrSpecific == "B") {symbolDraw(99); return;}
if(substrSpecific == "C") {symbolDraw(100); return;}
if(substrSpecific == "D") {symbolDraw(101); return;}
if(substrSpecific == "E") {symbolDraw(102); return;}
if(substrSpecific == "F") {symbolDraw(103); return;}
if(substrSpecific == "G") {symbolDraw(104); return;}
if(substrSpecific == "H") {symbolDraw(105); return;}
if(substrSpecific == "I") {symbolDraw(106); return;}
if(substrSpecific == "J") {symbolDraw(107); return;}
if(substrSpecific == "K") {symbolDraw(108); return;}
if(substrSpecific == "L") {symbolDraw(109); return;}
if(substrSpecific == "M") {symbolDraw(110); return;}
if(substrSpecific == "N") {symbolDraw(111); return;}
if(substrSpecific == "O") {symbolDraw(112); return;}
if(substrSpecific == "P") {symbolDraw(113); return;}
if(substrSpecific == "Q") {symbolDraw(114); return;}
if(substrSpecific == "R") {symbolDraw(115); return;}
if(substrSpecific == "S") {symbolDraw(116); return;}
if(substrSpecific == "T") {symbolDraw(117); return;}
if(substrSpecific == "U") {symbolDraw(118); return;}
if(substrSpecific == "V") {symbolDraw(119); return;}
if(substrSpecific == "W") {symbolDraw(120); return;}
if(substrSpecific == "X") {symbolDraw(121); return;}
if(substrSpecific == "Y") {symbolDraw(122); return;}
if(substrSpecific == "Z") {symbolDraw(123); return;}

if(substrSpecific == "1") {symbolDraw(126); return;}
if(substrSpecific == "2") {symbolDraw(127); return;}
if(substrSpecific == "3") {symbolDraw(128); return;}
if(substrSpecific == "4") {symbolDraw(129); return;}
if(substrSpecific == "5") {symbolDraw(130); return;}
if(substrSpecific == "6") {symbolDraw(131); return;}
if(substrSpecific == "7") {symbolDraw(132); return;}
if(substrSpecific == "8") {symbolDraw(133); return;}
if(substrSpecific == "9") {symbolDraw(134); return;}
if(substrSpecific == "0") {symbolDraw(135); return;}

if(substrSpecific == ".") {symbolDraw(138); return;}
if(substrSpecific == ":") {symbolDraw(139); return;}
if(substrSpecific == ",") {symbolDraw(140); return;}
if(substrSpecific == ";") {symbolDraw(141); return;}
if(substrSpecific == " ") {symbolDraw(142); return;}
if(substrSpecific == "'") {symbolDraw(143); return;}
if(substrSpecific == "\42") {symbolDraw(144); return;}// "
if(substrSpecific == "(") {symbolDraw(145); return;}
if(substrSpecific == "!") {symbolDraw(146); return;}
if(substrSpecific == "?") {symbolDraw(147); return;}
if(substrSpecific == ")") {symbolDraw(148); return;}
if(substrSpecific == "+") {symbolDraw(149); return;}
if(substrSpecific == "-") {symbolDraw(150); return;}
if(substrSpecific == "*") {symbolDraw(151); return;}
if(substrSpecific == "/") {symbolDraw(152); return;}
if(substrSpecific == "=") {symbolDraw(153); return;}
if(substrSpecific == "@") {symbolDraw(154); return;}
if(substrSpecific == "#") {symbolDraw(155); return;}
if(substrSpecific == "|") {symbolDraw(156); return;}
if(substrSpecific == "<") {symbolDraw(157); return;}
if(substrSpecific == ">") {symbolDraw(158); return;}
if(substrSpecific == "[") {symbolDraw(159); return;}
if(substrSpecific == "]") {symbolDraw(160); return;}
if(substrSpecific == "{") {symbolDraw(161); return;}
if(substrSpecific == "}") {symbolDraw(162); return;}
if(substrSpecific == "&") {symbolDraw(163); return;}
if(substrSpecific == "%") {symbolDraw(164); return;}
if(substrSpecific == "\134") {symbolDraw(165); return;}
if(substrSpecific == "_") {symbolDraw(173); return;}
//166 t* | 167 LEFT | 168 RIGHT | 169 UP | 170 DOWN | 171 SMART_HOME | 172 HUMIDITY

return;
                    }
                    
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void ads_config(){
  ads.setGain(ADS1115_REG_CONFIG_PGA_4_096V); // GAIN_ONE and resolution to ± 4.096V
  ads.setSampleRate(ADS1115_REG_CONFIG_DR_250SPS); // Samples per sec//8 16 32 64 128 250 475 860
                 }

int16_t ads_read0(){ 
  ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_0);  // Single mode input on AIN0 (AIN0 - GND)
  ads.triggerConversion();  // Triggered mannually 
  return ads.getConversion();  // returns int16_t value
                   }
int16_t ads_read1(){ 
  ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_1);  // Single mode input on AIN1 (AIN0 - GND)
  ads.triggerConversion();  // Triggered mannually 
  return ads.getConversion();  // returns int16_t value
                   }
int16_t ads_read2(){ 
  ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_2);  // Single mode input on AIN2 (AIN0 - GND)
  ads.triggerConversion();  // Triggered mannually 
  return ads.getConversion();  // returns int16_t value
                   }
int16_t ads_read3(){ 
  ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_3);  // Single mode input on AIN3 (AIN0 - GND)
  ads.triggerConversion();  // Triggered mannually 
  return ads.getConversion();  // returns int16_t value
                   } 
