#include <Adafruit_GFX.h>    
#include <Arduino_ST7789.h> 
#include <SPI.h>
#include "FreeMono12pt7b.h"
#include <Wire.h>
#include <ADS1115_lite.h>
#include "NOTO_FONT_ALL.h"
#include "TEMP_PIC.h"
#include "H_PIC.h"
#include "NET_PIC.h"
#include "EEPROM.h"
#include <HTTPClient.h>
#include <ArduinoJson.h> 
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP,"pool.ntp.org", 10800, -4000);//
#include <SHT3x.h>
SHT3x Sensor;
#include <BleCombo.h>

#define TFT_DC    1 //PIN
#define TFT_RST   0 //PIN
#define TFT_CS    A0 //VIRTUAL PIN?
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_CS);

ADS1115_lite ads(ADS1115_DEFAULT_ADDRESS);  // 0x48 addr pin connected to GND

int h = 27-1;//HEIGHT IMAGE 
int w = 2436;//WIDTH IMAGE
int layout = 1;//ENG ON START
double timeCursor;
String apiC = "Moscow";
String apiCountry = "ru";
String apiKey = "";
String InText = "";     
bool stringComplete = false; 
bool originalCol = false;
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
float p = 3.1415926;
int startX = 2; 
int startY = 0;
int scaleX = 1; // >=1
int scaleY = 1; // >=1
String MyText = "";
String substrSpecific; 
String noteStr;
int rectMenuX = 0;
int rectMenuY = 0;
int selectMenuP = 0;
unsigned long timePast = 0;
int InMenu = 0;
int colorR = 180;
int colorG = 180;
int colorB = 250;
int selectColorProg = 2; //GREEN
uint16_t charColo = WHITE;
uint16_t BackColor;
uint16_t inactElem;
int BRIG = -1;
int BRIG_LAYOUT = -1;
int tempFine;
int humFine;
String StrToBLE;

void setup()   { 
  
  Sensor.Begin(); //SHT30
  
    EEPROM.begin(1200);
  if(EEPROM.read(0) == 255)   {EEPROM.writeString(0,  ""); EEPROM.commit();}//NOTE
  if(EEPROM.read(400) == 255) {EEPROM.writeString(400,""); EEPROM.commit();}//SSID
  if(EEPROM.read(450) == 255) {EEPROM.writeString(450,""); EEPROM.commit();}//PASSWORD
  if(EEPROM.read(490) == 255) {EEPROM.writeInt   (490,80); EEPROM.commit();}//BRIGHTNESS  
  if(EEPROM.read(494) == 255) {EEPROM.writeInt   (494,80); EEPROM.commit();}//BRIGHTNESS_LAYOUT
  if(EEPROM.read(500) == 255) {EEPROM.writeInt   (500,0);  EEPROM.commit();}//FLAG TO CHANGE BACKGROUND//[0 OR 100]
  if(EEPROM.read(505) == 255) {EEPROM.writeInt   (505,60); EEPROM.commit();}//R
  if(EEPROM.read(506) == 255) {EEPROM.writeInt   (506,0);  EEPROM.commit();}//G
  if(EEPROM.read(507) == 255) {EEPROM.writeInt   (507,70); EEPROM.commit();}//B
  if(EEPROM.read(515) == 255) {EEPROM.writeInt   (515,2);  EEPROM.commit();}//LAYOUT[1,2]
  if(EEPROM.read(520) == 255) {EEPROM.writeInt   (520,0);  EEPROM.commit();}//SMART HOME CHANNEL[0,1,2,3]
  if(EEPROM.read(525) == 255) {EEPROM.writeInt   (525,0);  EEPROM.commit();}//WI-FI AUTO CONNECT[0,1] 
  if(EEPROM.read(530) == 255) {EEPROM.writeString(530,apiC);       EEPROM.commit();}//CITY
  if(EEPROM.read(590) == 255) {EEPROM.writeString(590,apiCountry); EEPROM.commit();}//COUNTRY
  if(EEPROM.read(650) == 255) {EEPROM.writeString(650,apiKey);     EEPROM.commit();}//KEY FROM OPENWEATHERMAP.ORG
  if(EEPROM.read(800) == 255) {EEPROM.writeInt   (800,9);  EEPROM.commit();}//TEMPFINE//"-1" = PROBLEM//9  = VIRTUAL 0
  if(EEPROM.read(804) == 255) {EEPROM.writeInt   (804,9);  EEPROM.commit();}//HUMFINE//"-1"  = PROBLEM//9  = VIRTUAL 0
  if(EEPROM.read(808) == 255) {EEPROM.writeInt   (808,12); EEPROM.commit();}//SavedUTC//"-1" = PROBLEM//12 = VIRTUAL 0
  if(EEPROM.read(820) == 255)  {EEPROM.writeString(820, "Hello world! Characters are sent at intervals for connection stability. String=1. ");                 EEPROM.commit();}//BLE STRING 1
  if(EEPROM.read(920) == 255)  {EEPROM.writeString(920, "This string can contain up to 98 characters. For example, to store the URL or password. String=2. "); EEPROM.commit();}//BLE STRING 2
  if(EEPROM.read(1020) == 255) {EEPROM.writeString(1020,"For Bluetooth to work successfully, restart it on both devices after connecting. String=3. ");        EEPROM.commit();}//BLE STRING 3
 

  BRIG =        EEPROM.readInt(490); if(BRIG < 10)        {BRIG = 110;        EEPROM.writeInt(490, BRIG);        EEPROM.commit();}//BRIGHTNESS FOR FIRST START
  BRIG_LAYOUT = EEPROM.readInt(494); if(BRIG_LAYOUT < 10) {BRIG_LAYOUT = 140; EEPROM.writeInt(494, BRIG_LAYOUT); EEPROM.commit();}//BRIGHTNESS_LAYOUT FOR FIRST START
  if(EEPROM.readInt(500) == 100) {BackColor = color565(EEPROM.readInt(505),EEPROM.readInt(506),EEPROM.readInt(507));} //THEME FROM EEPROM//500 = FLAG 
                            else {BackColor = color565(60,0,70);} //DEFAULT THEME
  pinMode(7, OUTPUT); //BL PIN                                
  analogWrite(7, BRIG); //BL PIN = BRIGHTNESS
  pinMode(3, OUTPUT); //RU PIN
  analogWrite(3, BRIG_LAYOUT);
  pinMode(10, OUTPUT); //ENG PIN
  analogWrite(10, BRIG_LAYOUT);
  //////////////////////
  tft.init(240, 240);   
  uint16_t time = millis();
  inactElem = color565(180,180,180); 
  //tft.fillScreen(0xdbde);
  ads_config();
  if(EEPROM.readInt(525) == 1) {CONNECT_WIFI(1);}//(1) = RENURN HERE WITHOUT PRESS ESC 
  startMenu();
                 }        
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  if(numButton1 == 10 || numButton1 == 2) {InMenu = 0; startMenu(); //HERE AFTER EXIT FROM VOIDS WITHOUT RECHECK NUMBUTTON1==10(ESC KEY)// 2 = MENU KEY
                                           analogWrite(3, BRIG_LAYOUT); //RU
                                           analogWrite(10, BRIG_LAYOUT);} //ENG
                                           numButton0 = -1; numButton1 = -1; numButton2 = -1; numButton3 = -1;
PressedButton(0);
if(numButton1 > -1) {if(InMenu < 0.5)     {moveCursor();}
                     if(numButton1 == 5 && selectMenuP < 12)  {chooseMenu(); InMenu = 0; numButton1 = -1; startMenu();}}//ENTER PRESSED AND AFTER RETURN
            }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void startMenu() {       
  tft.fillScreen(BackColor);  
  int numMenu = 1; startY = 0; InMenu = 0; rectMenuX = 0; rectMenuY = 0; scaleX = 1; scaleY = 1; charColo = WHITE;
  while(numMenu <= 11) {drawMenu(numMenu); numMenu ++; startY += 21;}
    rectMenuY = 0; startY = rectMenuY; selectMenuP = 1;
    if(WiFi.status() == WL_CONNECTED) {tft.drawBitmap(214, 0,  NET_PIC, 25, 21, WHITE);}
    tft.fillRect(rectMenuX,rectMenuY+1, 17, 22,color565(255,255,255) );//CURSOR //FIRST [MENU]   
                 }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void drawMenu(int numMenu){
  if(numMenu == 1)  {startX = 17; MyText = "Wi-Fi сеть"; event();}
  if(numMenu == 2)  {startX = 17; MyText = "Заметка"; event();}
  if(numMenu == 3)  {startX = 17; MyText = "ЧАСЫ "; event(); scaleX = 2; symbolDraw(166); scaleX = 1; MyText =" HUMIDITY"; event();}
  if(numMenu == 4)  {startX = 17; MyText = "Вольтметр [4 ch]"; event();}
  if(numMenu == 5)  {startX = 17; MyText = "RGB тест цвета"; event();}
  if(numMenu == 6)  {startX = 17; MyText = "ЧАТЫ и УПРАВЛЕНИЕ"; event();}//UART chat РУ+EN
  if(numMenu == 7)  {startX = 17; MyText = "МОЙ УМНЫЙ ДОМ "; event(); scaleX = 2; symbolDraw(171); scaleX = 1;}
  if(numMenu == 8)  {startX = 17; MyText = "Настройки"; event();}
  if(numMenu == 9)  {startX = 17; MyText = "Погода"; event();}
  if(numMenu == 10) {startX = 17; MyText = "BLUETOOTH app-s"; event();} 
  if(numMenu == 11) {startX = 17; MyText = "Игры /// games"; charColo = inactElem; event(); charColo = WHITE;} 
                          }   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void moveCursor() {
  if((numButton1 == 3 || numButton1 == 1) && millis() - timePast > 100 && selectMenuP < 11) {//NEXT MENU AND DELAY
                timePast = millis()-1;
                      tft.fillRect(rectMenuX,rectMenuY+1,   17,22,BackColor);
                      rectMenuY += 21; 
                      selectMenuP ++;
                      if(selectMenuP == 5) {
                      tft.fillRect(rectMenuX,rectMenuY+1,    6,22,color565(255,0,0) );//RGB MENU_P4  
                      tft.fillRect(rectMenuX+6,rectMenuY+1,  6,22,color565(0,255,0) );
                      tft.fillRect(rectMenuX+12,rectMenuY+1, 6,22,color565(0,0,255) );
                                           } else {
                      tft.fillRect(rectMenuX,rectMenuY+1,   17,22,color565(255,255,255) );
                                                  }
                     }
  if((numButton1 == 4 || numButton1 == 0) && millis() - timePast > 100 && selectMenuP > 1) {//PREVIOUS MENU AND DELAY 
                timePast = millis()-1;
                      tft.fillRect(rectMenuX,rectMenuY+1,   17,22,BackColor);
                      rectMenuY -= 21; 
                      selectMenuP --;
                      
                      if(selectMenuP == 5) {
                      tft.fillRect(rectMenuX,rectMenuY+1,    6,22,color565(255,0,0) );//RGB MENU_P4  
                      tft.fillRect(rectMenuX+6,rectMenuY+1,  6,22,color565(0,255,0) );
                      tft.fillRect(rectMenuX+12,rectMenuY+1, 6,22,color565(0,0,255) );
                                           } else {
                      tft.fillRect(rectMenuX,rectMenuY+1,   17,22,color565(255,255,255) );
                                                  }
                     }
                  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void chooseMenu() {
  if(selectMenuP == 1)  {InMenu = 1; WIFI_CONFIG();}
  if(selectMenuP == 2)  {InMenu = 1; NOTE(1);}
  if(selectMenuP == 3)  {InMenu = 1; INFORMER();}
  if(selectMenuP == 4)  {InMenu = 1; VOLTMETER();}
  if(selectMenuP == 5)  {InMenu = 1; MENURGB();}
  if(selectMenuP == 6)  {InMenu = 1; CHATS();}
  if(selectMenuP == 7)  {InMenu = 1; SMART_HOME();}
  if(selectMenuP == 8)  {InMenu = 1; SETTINGS();}
  if(selectMenuP == 9)  {InMenu = 1; WEATHER();}
  if(selectMenuP == 10) {InMenu = 1; BLUETOOTHAPPS();}
  if(selectMenuP == 11) {} //GAMES //InMenu = 1; RAFT();
  if(selectMenuP == 12) {} 
                  }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////      
void PressedButton(int which) {
  int ix0 = 0; int ix1 = 0; int ix2 = 0; int ix3 = 0;
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
void WIFI_CONFIG(){ 
        tft.fillScreen(BackColor); 
   startX = 19; startY = 2;  MyText = "Connect"; event();
   startX = 19; startY = 24; MyText = "Disconnect"; event();
   startX = 19; startY = 46; MyText = "Edit ssid"; event();
   startX = 19; startY = 68; MyText = "Edit password"; event();
   rectMenuX = 0; rectMenuY = 0;//CURSOR
    tft.fillRect(rectMenuX, rectMenuY+3, 17, 22,color565(255,255,255));//FIRST 
    if(WiFi.status() == WL_CONNECTED) {tft.drawBitmap(214, 0,  NET_PIC, 25, 21, WHITE);}
    double pau = millis();
    PressedButton(0);
   while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;}
     if((numButton1 == 3 || numButton1 == 1) && rectMenuY < 62 && millis() - pau > 100) {tft.fillRect(0, 0, 17, 92,BackColor); //TO BACKGROUND COLOR 3 ROWS
                             rectMenuY+=22; tft.fillRect(rectMenuX, rectMenuY+3, 17, 22,color565(255,255,255));
                             pau = millis();}//NEXT    
     if((numButton1 == 4 || numButton1 == 0) && rectMenuY > 10 && millis() - pau > 100) {tft.fillRect(0, 0, 17, 92,BackColor); //TO BACKGROUND COLOR 3 ROWS
                             rectMenuY-=22; tft.fillRect(rectMenuX, rectMenuY+3, 17, 22,color565(255,255,255));
                             pau = millis();}//PREV 

                                   //IF ENTER KEY PRESSED
                                    if(numButton1 == 5 && rectMenuY == 0)  {CONNECT_WIFI(0); if(numButton1 == 2) {return;} WIFI_CONFIG(); PressedButton(0); break;}//GO TO CONNECT_WIFI()
                                    if(numButton1 == 5 && rectMenuY == 22) {DISCONNECT_WIFI(); if(numButton1 == 2) {return;} WIFI_CONFIG(); PressedButton(0); break;}//
                                    if(numButton1 == 5 && rectMenuY == 44) {NOTE(2); MyText = ""; if(numButton1 == 2) {return;} WIFI_CONFIG(); PressedButton(0); break;}//TO EDIT SSID//HERE AFTER NOTE()
                                    if(numButton1 == 5 && rectMenuY == 66) {NOTE(3); MyText = ""; if(numButton1 == 2) {return;} WIFI_CONFIG(); PressedButton(0); break;}//TO EDIT PASSWORD//HERE AFTER NOTE()                   
                                                       
                          }//END WHILE 
                          InMenu = 0;
}
void CONNECT_WIFI(int start_device) {
        tft.fillScreen(BackColor); 
        String stringSSID = EEPROM.readString(400); //EEPROM.readString(400).substring(1, EEPROM.readString(400).length()-1);
        String stringPASS = EEPROM.readString(450);          
        startX = 2; startY = 2;  MyText = "Connecting to "; event(); 
        startX = 2; startY = 24; MyText = stringSSID; event();  

const char* ssid =     stringSSID.c_str();//"";
const char* password = stringPASS.c_str();//""; 
        
  if (WiFi.status() != WL_CONNECTED) {WiFi.begin(ssid, password);}
  int dots = 0;
  while (WiFi.status() != WL_CONNECTED) {
      delay(2000);
      if(dots < 7) {MyText =  "."; event();} dots++; if(dots > 4) {break;}
                                        }
   if(WiFi.status() == WL_CONNECTED) {                                       
  startX = 2; startY = 46; 
  MyText = "CONNECTED"; event();
  startX = 2; startY = 68;
  MyText = "[WiFi] IP address: "; event();
  startX = 2; startY = 90;
  MyText = WiFi.localIP().toString(); event();
  tft.drawBitmap(214, 0,  NET_PIC, 25, 21, WHITE);
                                     } else {
                                             MyText = "network not available "; event(); WiFi.disconnect();
                                            }
  
  PressedButton(0);
  if(start_device == 0) {while(numButton1 != 10 && numButton1 != 5) {PressedButton(0); if(numButton1 == 2) {return;}}} //CHECK FOR ESC KEY 
scaleX = 1; scaleY = 1; numButton1 = -1;//BEFORE EXIT
}
////////////////////////////////////////////////////////////////////////////////////////////// 
void DISCONNECT_WIFI() {
  tft.fillScreen(BackColor);
  WiFi.disconnect(); 
  if(WiFi.status() != WL_CONNECTED) {MyText = "disconnect OK"; scaleX = 1; scaleY = 1; startX = 32; startY = 12; event();}
  else {MyText = "please, try again"; scaleX = 1; scaleY = 1; startX = 2; startY = 105; event();}
                                     PressedButton(0);
                                     while(numButton1 != 10 && numButton1 != 5) {PressedButton(0); if(numButton1 == 2) {return;}} //CHECK FOR ESC KEY
                                     numButton1 = -1;
}
//////////////////////////////////////////////////////////////////////////////////////////////                  
void INFORMER() { 
  refresh:
  uint16_t Back = color565(0,0,0);
  tft.fillScreen(Back); 
  //int offsetSave = EEPROM.readInt(808);
  timeClient.setTimeOffset((EEPROM.readInt(808) - 12)*3600);//12 = VIRTUAL 0
  int tempsht30 = 10, tempT = 19; //19 != 10 FOR FIRST DRAW//=-8 
  int humsht30 = 40,  humT = 49;  //40 != 49 FOR FIRST DRAW//=-8  
  tft.drawBitmap(0, 85,  TEMP_PIC, 35, 47, WHITE);
  tft.drawBitmap(0, 156, H_PIC,    35, 47, WHITE);
  double time2 = millis() - 5000;
  int first = 1; String PrevTime = ""; int mno = 0;
  tempFine = EEPROM.readInt(800);
  humFine =  EEPROM.readInt(804);
    if(tempFine < 1 || tempFine > 17) {tempFine = 9;}
    if(humFine < 1  || humFine  > 17) {humFine  = 9;}

startX = 35; startY = 74;  scaleX = 3; scaleY = 3; MyText = " XX"; event(); startY = 87;  scaleX = 2; scaleY = 2; MyText = " C"; event();//DRAW T*
startX = 35; startY = 141; scaleX = 3; scaleY = 3; MyText = " XX"; event(); startY = 151; scaleX = 2; scaleY = 2; MyText = " %"; event();//DRAW HUMIDITY
                                                                                
 if(WiFi.status() == WL_CONNECTED) {timeClient.begin();}

 if(WiFi.status() == WL_CONNECTED) {
              tft.fillRect(0, 210,  240, 30, Back);
              MyText = "====CONNECTED===="; scaleX = 1; scaleY = 1; startX = 9; startY = 210; charColo = BLUE; event(); charColo = WHITE;
                                   }                                                       
 if(WiFi.status() != WL_CONNECTED) {
           if(!timeClient.isTimeSet()) {
              tft.fillRect(0, 210,  240, 30, Back);
              MyText = "==NO CONNECTION=="; scaleX = 1; scaleY = 1; startX = 9; startY = 210; charColo = RED; event(); charColo = WHITE;
             MyText = "--:--"; scaleX = 3; scaleY = 3; startX = 18; startY = 0; event();}
           if(timeClient.isTimeSet())  {
              tft.fillRect(0, 210,  240, 30, Back);
              MyText = "=NO=C="; scaleX = 1; scaleY = 1; startX = 9; startY = 210; charColo = RED; event();
              MyText =        "=TIME=SET=";  scaleX = 1; scaleY = 1; startX = 98; startY = 210; charColo = BLUE; event(); charColo = WHITE; //WIFI NOT CONNECTED, CLOCK STILL GO FROM LOCAL     
             timeClient.update(); MyText = timeClient.getFormattedTime().substring(0, 5); 
             tft.fillRect(0,  0,  215, 68, Back); scaleX = 3; scaleY = 3; startX = 18; startY = 0; event();} //OK, MAYBE                    
                                   }
                                   
                                   if(WiFi.status() == WL_CONNECTED) {tft.drawBitmap(214, 0,  NET_PIC, 25, 21, WHITE);}
 
  PressedButton(0);
  while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;} //CHECK FOR ESC KEY 
  //////////////////////////// WI-FI CONNECT AND DISCONNECT IF 'UP' OR 'DOWN' PRESSED
  if(numButton1 == 0) {String stringSSID = EEPROM.readString(400);
                       String stringPASS = EEPROM.readString(450);
                       const char* ssid =     stringSSID.c_str();
                       const char* password = stringPASS.c_str(); 
                       if(WiFi.status() != WL_CONNECTED) {WiFi.begin(ssid, password); delay(1000); goto refresh;}
                      }
  if(numButton1 == 1) {WiFi.disconnect(); delay(1000); goto refresh;}
  ////////////////////////////
      ////////////SHT30
       Sensor.UpdateData(); tempT = Sensor.GetTemperature() + (tempFine-9);
       if(tempT != tempsht30) {tft.fillRect(77, 74,  80, 65, Back); 
                               startX = 77; startY = 74;  scaleX = 3; scaleY = 3; MyText = tempT; event();
                               tempsht30 = tempT;}
      ////////////  
                            humT = Sensor.GetRelHumidity() + (humFine-9);
       if(humT != humsht30)  {tft.fillRect(77, 141,  80, 65, Back); 
                               startX = 77; startY = 141;  scaleX = 3; scaleY = 3; MyText = humT; event();
                               humsht30 = humT;}
      ////////////                                                       
    if(first == 1 && millis() - time2 > 5000 && WiFi.status() == WL_CONNECTED) {//CHECK FOR FIRST UPDATE EVERY 5S
    startX = 36; startY = 0; scaleX = 3; scaleY = 3; MyText = "WAIT"; event();
    timeClient.update(); time2 = millis(); //CHECK
    if(timeClient.isTimeSet()) {first = 0; 
                                MyText = timeClient.getFormattedTime().substring(0, 5); 
                                tft.fillRect(0,  0,  215, 68, Back); scaleX = 3; scaleY = 3; startX = 18; startY = 0; event();
                                time2 = millis();}                                   
                                                                               }
    if(first == 1 && WiFi.status() != WL_CONNECTED) {first = 0;} 
                                                                                                                                                                                         
    if(first == 0 && millis() - time2 > 60000)  {//REDRAW EVERY 60S 
             mno ++ ; if(mno > 59) {if(timeClient.isTimeSet()) {timeClient.update();} mno = 0;} //CONNECT TO SERVER EVERY 60 MIN 
             if(timeClient.isTimeSet()) {MyText = timeClient.getFormattedTime().substring(0, 5); 
             tft.fillRect(0,  0,  215, 68, Back); scaleX = 3; scaleY = 3; startX = 18; startY = 0; event();}//OK, MAYBE
             time2 = millis(); 
       //if(PrevTime == MyText){tft.fillRect(0,  0,  215, 68, Back); MyText = "NO D"; scaleX = 3; scaleY = 3; startX = 18; startY = 0; event();}//NO DATA FROM INTERNET, CLOCK STOPPED
       //      PrevTime = MyText;        

             if(WiFi.status() != WL_CONNECTED && !timeClient.isTimeSet()) 
                                               {tft.fillRect(0, 210,  240, 30, Back);
                                                MyText = "==NO CONNECTION=="; scaleX = 1; scaleY = 1; startX = 9; startY = 210; charColo = RED; event(); charColo = WHITE;}
             if(WiFi.status() != WL_CONNECTED && timeClient.isTimeSet()) 
                                               {tft.fillRect(0, 210,  240, 30, Back);
                                                MyText = "=NO=C="; scaleX = 1; scaleY = 1; startX = 9; startY = 210; charColo = RED; event();
                                                MyText =        "=TIME=SET="; scaleX = 1; scaleY = 1; startX = 98; startY = 210; charColo = BLUE; event(); charColo = WHITE;} //WIFI NOT CONNECTED, CLOCK STILL GO FROM LOCAL
             if(WiFi.status() == WL_CONNECTED && timeClient.isTimeSet())
                                               {tft.fillRect(0, 210,  240, 30, Back);
                                                MyText = "====CONNECTED===="; scaleX = 1; scaleY = 1; startX = 9; startY = 210; charColo = BLUE; event(); charColo = WHITE;}//ERASE "==NO CONNECTION=="                                                
                                               } 
                          }
      InMenu = 0; scaleX = 1; scaleY = 1; charColo = WHITE; //BEFORE EXIT
}
////////////////////////////////////////////////////////////////////////////////////////////
void NOTE(int i) { 
  uint16_t wall; 
  if(i != 4) {wall = color565(100,0,30); tft.fillScreen(wall);} //IF !UART CHAT && ...
  if(i == 8 || i == 9 || i == 10) {tft.fillRect(0, 148, 240, 92, color565(0,0,255)); 
                                   MyText = "ENT=напечатать[BT]English only. 'layout' сменит раскладку на 2м устр-ве."; scaleX = 1; scaleY = 1; startX = 2; startY = 148; charColo = WHITE; event();
                                   //if(!Keyboard.isConnected()) {Keyboard.begin(); Mouse.begin();}
                                   layout = 1;} //IF EDIT STRING FOR SENDING VIA BLE //ENGLISH ONLY
  uint16_t curCol = color565(0,200,250);//CURSOR
  int curW = 8, curH = 22;

    if(i != 8 && i != 9 && i != 10) {//IF EDIT STRING AND SEND VIA BLE LAYOUT WILL BE CHANGED ON ANOTHER DEVICE
    if(EEPROM.readInt(515) == 2) {layout = 2; analogWrite(3, BRIG_LAYOUT); //RU LED
                                              analogWrite(10,0);  }//ENG LED
                            else {layout = 1; analogWrite(3, 0);   //RU LED
                                              analogWrite(10,BRIG_LAYOUT);}//ENG LED
                                    }
                                                 
  startX = 2; startY = 0; //scaleX = 3; scaleY = 3;
  int add = 0; int prevX, prevY;//ADD = SHIFT Y
  noteStr = "";
  if(i == 1){noteStr = EEPROM.readString(0); //NOTE//LENGTH ?
             startX = 2; startY = 169;   MyText = "ESC или ENTER > сохр., MENU > без сохр. UP+s > UART"; 
             charColo = color565(180,180,250); event(); charColo = WHITE; startX = 2; startY = 0;}
  if(i == 2){noteStr = EEPROM.readString(400);} //SSID
  if(i == 3){noteStr = EEPROM.readString(450);} //PASSWORD 
  if(i == 4){startY = 167; add = 167; wall = BLUE; Serial.begin(115200);}//FOR UART CHAT
  if(i == 5){noteStr = EEPROM.readString(530);} //APICITY
  if(i == 6){noteStr = EEPROM.readString(590);} //APICOUNTRY
  if(i == 7){noteStr = EEPROM.readString(650); //APIKEY  
             startX = 2; startY = 144;   MyText = "please visit      openweathermap.orgto register and   get API key"; 
             charColo = color565(180,180,250); event(); charColo = WHITE; startX = 2; startY = 0;} 
  if(i == 8)  {noteStr = EEPROM.readString(820); } //BLE STRING 1
  if(i == 9)  {noteStr = EEPROM.readString(920); } //BLE STRING 2 
  if(i == 10) {noteStr = EEPROM.readString(1020);} //BLE STRING 3 
   
//  if(i == 2){for(int qa = 0; qa < 50; qa++) {ssid[qa] =     EEPROM.read(200+qa);} noteStr = ssid.c_str();    }//SSID
//  if(i == 3){for(int qb = 0; qb < 50; qb++) {password[qb] = EEPROM.read(250+qb);} noteStr = password.c_str();}//PASSWORD
  MyText = noteStr; event();//DRAW NOTE OR SSID OR PASSWORD OR...
  tft.fillRect(startX, startY+2, 8, 22, curCol);
  numButton1 = -1; 
  timeCursor = millis();
  PressedButton(0);
 
  while(numButton1 != 10 && numButton1 != 5) {  
 
 if(i == 4 && Serial.available()) {serialCheck(); //FOR UART CHAT
                                   //if(stringComplete == true) {
                                   prevX = startX; prevY = startY; 
                                   MyText = InText.substring(0,108);//CUT INCOMING STRING 
                                   tft.fillRoundRect(-5,77,250,73,15,color565(255,200,200));
                                   startX = 0; startY = 77; charColo = BLACK; event(); charColo = WHITE;  
                                   startX = prevX; startY = prevY; InText = ""; //stringComplete = false; }
                                  }    
                                                                          
    PressedButton(1); if(numButton1 == 5 && (i == 8 || i == 9 || i == 10)) {SEND_STRING_BLE(0); delay(300); numButton1 = -1; continue;}//USE 'ENTER'  IF BLE
                      if(numButton1 == 9 && (i == 8 || i == 9 || i == 10)) {SEND_STRING_BLE(1); delay(300); numButton1 = -1; continue;}//USE 'LAYOUT' IF BLE
                      if(numButton1 == 2 && i != 8 && i != 9 && i != 10) {return;} //DISABLE 'MENU' IF BLE FOR SAVING ANYWAY VIA 'ESC'
                                         if(millis() - timeCursor < 150) {continue;}//PAUSE FOR COMFORT
                                         if(numButton0 == -1 &&
                                            numButton1 == -1 && 
                                            numButton2 == -1 &&
                                            numButton3 == -1   ) {continue;} //NOTHING PRESSED
                                         
                                         if((numButton0 != -1 || numButton2 != -1 || numButton3 != -1)) {timeCursor = millis();}//PAUSE ONLY IF SYMBOL KEY PRESSED
 
if((i == 4) && noteStr.length() > 106)                     {goto forfaster;}//LIMIT FOR UART CHAT  //54 RU AND 107 EN
if((i == 8 || i == 9 || i == 10) && noteStr.length() > 97) {goto forfaster;}//LIMIT FOR BLE STRING //49 RU AND 98  EN
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
    if(numButton2 == 9) {MyText = "s"; if(numButton1 == 7) {MyText = "S";} if(numButton1 == 8) {MyText = "/";} if(numButton1 == 0) {Serial.begin(115200); Serial.println(noteStr); goto forfaster;} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
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
    if(numButton2 == 9) {MyText = "ы"; if(numButton1 == 7) {MyText = "Ы";} if(numButton1 == 8) {MyText = "/";} if(numButton1 == 0) {Serial.begin(115200); Serial.println(noteStr); goto forfaster;} noteStr = noteStr + MyText; tft.fillRect(startX,startY+2,curW,curH,wall);  event(); tft.fillRect(startX,startY+2,curW,curH,curCol); goto forfaster;}
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

 if(i != 8 && i != 9 && i != 10) {//IF EDIT STRING AND SEND VIA BLE LAYOUT WILL BE CHANGED ON ANOTHER DEVICE                   
    if(numButton1 == 9 && layout == 1) {analogWrite(3, BRIG_LAYOUT); analogWrite(10, 0);   layout = 2; delay(350); numButton1 = -1;
                                        EEPROM.writeInt(515, 2); EEPROM.commit();} //CHANGE TO RU //IN THE END BECAUSE FURTHER NO CHECK PRESSED BUTTONS
    if(numButton1 == 9 && layout == 2) {analogWrite(3, 0);   analogWrite(10, BRIG_LAYOUT); layout = 1; delay(350); numButton1 = -1;
                                        EEPROM.writeInt(515, 1); EEPROM.commit();} //CHANGE TO ENG//IN THE END BECAUSE FURTHER NO CHECK PRESSED BUTTONS
                                 }
                       
                         }//END WHILE 
                             analogWrite(3,  BRIG_LAYOUT); //RU LED 
                             analogWrite(10, BRIG_LAYOUT); //ENG LED
                             if(i == 1){EEPROM.writeString(0, noteStr);   EEPROM.commit(); InMenu = 0; numButton1 = -1;}//startMenu();
                             if(i == 2){EEPROM.writeString(400, noteStr); EEPROM.commit(); numButton1 = -1;}//SSID//WIFI_CONFIG();
                             if(i == 3){EEPROM.writeString(450, noteStr); EEPROM.commit(); numButton1 = -1;}//PASSWORD//WIFI_CONFIG(); 
                             if(i == 5){EEPROM.writeString(530, noteStr); EEPROM.commit(); numButton1 = -1;}//APICITY
                             if(i == 6){EEPROM.writeString(590, noteStr); EEPROM.commit(); numButton1 = -1;}//APICOUNTRY
                             if(i == 7){EEPROM.writeString(650, noteStr); EEPROM.commit(); numButton1 = -1;}//APIKEY  
                             if(i == 8) {EEPROM.writeString(820,  noteStr); EEPROM.commit(); numButton1 = -1;}//BLE STRING 1
                             if(i == 9) {EEPROM.writeString(920,  noteStr); EEPROM.commit(); numButton1 = -1;}//BLE STRING 2
                             if(i == 10){EEPROM.writeString(1020, noteStr); EEPROM.commit(); numButton1 = -1;}//BLE STRING 3 
                             Serial.end();//FOR UP+S                              
            }//END NOTE();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void serialCheck()   {InText = "";
  while (Serial.available())   {
    char inChar = (char)Serial.read();
    InText += inChar;
    //if (inChar == '\n') {stringComplete = true;}
                               }
                     }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MENURGB() { 
tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));    //TOP
tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
tft.fillRect(0,  210,80, 30, color565(255,0,0));     //R
tft.fillRect(80, 210,80, 30, color565(0,255,0));     //G
tft.fillRect(160,210,80, 30, color565(0,0,255));     //B
 MyText = "R"+String(colorR); //R
   startX = 22;  startY = 185; event();
 MyText = "G"+String(colorG); //G
   startX = 102; startY = 185; event();
 MyText = "B"+String(colorB); //B
   startX = 182; startY = 185; event();

InMenu = 1;

if(selectColorProg == 1) {tft.drawRect(0,  185,80, 25, color565(255,255,255));} //R
if(selectColorProg == 2) {tft.drawRect(80, 185,80, 25, color565(255,255,255));} //G
if(selectColorProg == 3) {tft.drawRect(160,185,80, 25, color565(255,255,255));} //B

PressedButton(0);
while(numButton1 != 10) {PressedButton(0); if(numButton1 == 2) {return;}
////
if(numButton1 == 3 && selectColorProg == 2) {selectColorProg = 3; //RIGHT KEY
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(160,185,80, 25, color565(255,255,255)); //SELECT BLUE
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}
if(numButton1 == 3 && selectColorProg == 1) {selectColorProg = 2; //RIGHT KEY
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(80, 185,80, 25, color565(255,255,255)); //SELECT GREEN
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}
if(numButton1 == 4 && selectColorProg == 2) {selectColorProg = 1; //LEFT KEY
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(0,  185,80, 25, color565(255,255,255)); //SELECT RED
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}
if(numButton1 == 4 && selectColorProg == 3) {selectColorProg = 2; //LEFT KEY    
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(80, 185,80, 25, color565(255,255,255)); //SELECT GREEN
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}                         
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(selectColorProg == 1) {
if((numButton1 == 0) && colorR < 249) {colorR +=10;
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(0,  185,80, 25, color565(255,255,255));
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}    //TOP
if((numButton1 == 1) && colorR > 1)   {colorR -=10;
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(0,  185,80, 25, color565(255,255,255));
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}    //TOP
                         }
if(selectColorProg == 2) {
if((numButton1 == 0) && colorG < 249) {colorG +=10;
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(80, 185,80, 25, color565(255,255,255));
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}    //TOP
if((numButton1 == 1) && colorG > 1)   {colorG -=10;
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(80, 185,80, 25, color565(255,255,255));
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}    //TOP
                         }
if(selectColorProg == 3) {
if((numButton1 == 0) && colorB < 249) {colorB +=10;
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(160,185,80, 25, color565(255,255,255));
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}    //TOP
if((numButton1 == 1) && colorB > 1)   {colorB -=10;
                                     tft.fillRect(0,  185,240,25, color565(40,40,40));    //TEXT
                                      MyText = "R"+String(colorR); //R
                                      startX = 22;  startY = 185; event();
                                      MyText = "G"+String(colorG); //G
                                      startX = 102; startY = 185; event();
                                      MyText = "B"+String(colorB); //B
                                      startX = 182; startY = 185; event();
                                     tft.drawRect(160,185,80, 25, color565(255,255,255));
                                     tft.fillRect(0,  0,  240,185,color565(colorR,colorG,colorB));}    //TOP
                         }
////
                      } InMenu = 0; 
               }
//////////////////////////////////////////////////////////////////////////////////////////// 
void VOLTMETER(){
  tft.fillScreen(BLUE);
  tft.setFont(&FreeMono12pt7b);
  //tft.setCursor(5, 35);
  tft.setTextSize(2, 2);
  tft.setTextColor(WHITE);
  tft.setTextWrap(true);
  MyText = "< 3V3 т. к. после R=300k";
  startX = 2; startY = 150; charColo = color565(0,0,70); event(); charColo = WHITE; //DRAW TEXT
  MyText = "Press any button  to check. Esc=exit";
  startX = 2; startY = 196; event(); //DRAW TEXT
  float Q0, Q1, Q2, Q3;
  int U0, U1, U2, U3;
  resolution=4096.00/32752.00;
  float maxU;
  PressedButton(0);
  while(numButton1 != 10) {PressedButton(0); if(numButton1 == 2) {return;} //(0)TO CHECK ONLY 1 CIRCUIT FOR ESC
    
    U0=ads_read0(); result0 = (U0*resolution)/1000.00;
    U1=ads_read1(); result1 = (U1*resolution)/1000.00;
    U2=ads_read2(); result2 = (U2*resolution)/1000.00;
    U3=ads_read3(); result3 = (U3*resolution)/1000.00; 
    maxU= max(max(result0,result1),max(result2,result3)); 
    if(maxU - result0 < 0.1 && maxU - result1 < 0.1 && maxU - result2 < 0.1 && maxU - result3 < 0.1) {
                                                                     tft.fillRect(150,0,30,148,BLUE);}   
                 
  if(abs(Q0 - result0) > 0.02) {tft.setCursor(5, 35); tft.setTextColor(BLUE);  tft.print(Q0);
                                Q0 = result0; 
                                tft.setCursor(5, 35); tft.setTextColor(WHITE); tft.print(Q0);  
                                 if(abs(result0 - maxU) > 0.1) {startX = 150; startY = 7;   symbolDraw(167);}}//DRAW <=
                               
  if(abs(Q1 - result1) > 0.02) {tft.setCursor(5, 70); tft.setTextColor(BLUE);  tft.print(Q1);
                                Q1 = result1; 
                                tft.setCursor(5, 70); tft.setTextColor(WHITE); tft.print(Q1);  
                                 if(abs(result1 - maxU) > 0.1) {startX = 150; startY = 42;  symbolDraw(167);}}//DRAW <=
  if(abs(Q2 - result2) > 0.02) {tft.setCursor(5, 105); tft.setTextColor(BLUE);  tft.print(Q2);
                                Q2 = result2; 
                                tft.setCursor(5, 105); tft.setTextColor(WHITE); tft.print(Q2); 
                                 if(abs(result2 - maxU) > 0.1) {startX = 150; startY = 77;  symbolDraw(167);}}//DRAW <=
  if(abs(Q3 - result3) > 0.02) {tft.setCursor(5, 140); tft.setTextColor(BLUE);  tft.print(Q3);
                                Q3 = result3; 
                                tft.setCursor(5, 140); tft.setTextColor(WHITE); tft.print(Q3); 
                                 if(abs(result3 - maxU) > 0.1) {startX = 150; startY = 112; symbolDraw(167);}}//DRAW <=                             
                         } InMenu = 0;
                }
//////////////////////////////////////////////////////////////////////////////////////////// 
void CHATS() {
  PressedButton(0); 
        tft.fillScreen(BackColor); 
        tft.fillRect(0,  12, 240, 4,  color565(250,250,250));
        tft.fillRect(0,  122,240, 4,  color565(250,250,250));
        tft.fillRoundRect(30, 2,  180, 24, 5,color565(250,250,250));
        tft.fillRoundRect(30, 112,180, 24, 5,color565(250,250,250));
        charColo = BLACK;
        MyText = "Чаты:"; startX = 89; startY = 0; scaleX = 1; scaleY = 1; event(); 
        MyText = "Управление:"; startX = 50; startY = 112; scaleX = 1; scaleY = 1; event();
        charColo = WHITE;
//Чаты:         
   startX = 19; startY = 24; MyText = "UART"; event();
   startX = 19; startY = 46; MyText = "Bluetooth"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 68; MyText = "Wi-Fi";     charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 90; MyText = "Internet";  charColo = inactElem; event(); charColo = WHITE;
//Управление:
   startX = 19; startY = 134;  MyText = "UART"; event();
   startX = 19; startY = 156; MyText = "Bluetooth"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 178; MyText = "Wi-Fi";     charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 200; MyText = "Internet";  charColo = inactElem; event(); charColo = WHITE;
   rectMenuX = 0; rectMenuY = 22;//CURSOR
    tft.fillRect(rectMenuX, rectMenuY+3, 17, 22,color565(255,255,255));//FIRST 
    double pau = millis();
    PressedButton(0);
   while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;} 
     if((numButton1 == 3 || numButton1 == 1) && rectMenuY < 198 && millis() - pau > 100) {tft.fillRect(0, 24, 17, 90,BackColor); tft.fillRect(0, 134, 17, 90,BackColor); //TO BACKGROUND COLOR 8 ROWS
                             rectMenuY+=22; if(rectMenuY == 110) {rectMenuY+=22;} //SKIPPING Управление:
                             tft.fillRect(rectMenuX, rectMenuY+3, 17, 22,color565(255,255,255));
                             pau = millis();}//NEXT    
     if((numButton1 == 4 || numButton1 == 0) && rectMenuY > 26 && millis() - pau > 100)  {tft.fillRect(0, 24, 17, 90,BackColor); tft.fillRect(0, 134, 17, 90,BackColor); //TO BACKGROUND COLOR 8 ROWS
                             rectMenuY-=22; if(rectMenuY == 110) {rectMenuY-=22;} //SKIPPING Управление:
                             tft.fillRect(rectMenuX, rectMenuY+3, 17, 22,color565(255,255,255));
                             pau = millis();}//PREV 

                                   //IF ENTER KEY PRESSED
                                    if(numButton1 == 5 && rectMenuY == 22)  {UART_CHAT(); if(numButton1 == 2) {return;}  CHATS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 44)  {}
                                    if(numButton1 == 5 && rectMenuY == 66)  {}
                                    if(numButton1 == 5 && rectMenuY == 88)  {} 
                                    if(numButton1 == 5 && rectMenuY == 132) {UART_DRIVE(); if(numButton1 == 2) {return;} CHATS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 154) {}
                                    if(numButton1 == 5 && rectMenuY == 176) {}
                                    if(numButton1 == 5 && rectMenuY == 198) {}                  
                                                       
                          }//END WHILE 
                          InMenu = 0; 
}
//////////////////////////////////////////////////////////////////////////////////////////// 
void BLUETOOTHAPPS() {
  if(!Keyboard.isConnected()) {Keyboard.begin(); Mouse.begin();}
        tft.fillScreen(BackColor); scaleX = 1; scaleY = 1; charColo = WHITE; 
   startX = 19; startY = 2;   MyText = "Keyboard+Mouse"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 24;  MyText = "BLE Keyboard";   event(); charColo = WHITE;
   startX = 19; startY = 46;  MyText = "BLE Mouse";      event(); charColo = WHITE;
   startX = 55; startY = 68;  MyText = "Сохранено:";       event();
   tft.fillRect(0, 75, 52, 10, color565(100,100,210)); tft.fillRect(187, 75, 54, 10, color565(100,100,210));
   
   startX = 19; startY = 90;  MyText = "Строка 1";       event();
   startX = 19; startY = 112; MyText = "Строка 2";       event();
   startX = 19; startY = 134; MyText = "Строка 3";       event();
   //startX = 19; startY = 156; MyText = "Строка 4";       event();
   
   tft.fillRect(0, 190, 240, 53, color565(0,0,155));//BLUE BACKGROUND
   startX = 2; startY = 190; MyText = "При зарядке кнопки   могут сбоить"; charColo = color565(250,170,190); event();//PINK 
   

   rectMenuX = 0; rectMenuY = 0; //CURSOR
    tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));//FIRST 
    double pau = millis();
    PressedButton(0);                   
   while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;}  
     if((numButton1 == 3 || numButton1 == 1) && rectMenuY < 132 && millis() - pau > 100) {tft.fillRect(0, 0,  17, 68,BackColor); //TO BACKGROUND COLOR 3 ROWS
                                                                                          tft.fillRect(0, 90, 17, 88,BackColor); //TO BACKGROUND COLOR 4 ROWS
                             rectMenuY+=22; if(rectMenuY == 66) {rectMenuY+=22;} tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));
                             pau = millis();}//NEXT    
     if((numButton1 == 4 || numButton1 == 0) && rectMenuY > 10 &&  millis() - pau > 100) {tft.fillRect(0, 0,  17, 68,BackColor); //TO BACKGROUND COLOR 3 ROWS
                                                                                          tft.fillRect(0, 90, 17, 88,BackColor); //TO BACKGROUND COLOR 4 ROWS
                             rectMenuY-=22; if(rectMenuY == 66) {rectMenuY-=22;} tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));
                             pau = millis();}//PREV 

                                   //IF ENTER KEY PRESSED
                                    if(numButton1 == 5 && rectMenuY == 0)   {BLE_COMBO();    if(numButton1 == 2) {return;} BLUETOOTHAPPS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 22)  {BLE_KEYBOARD(); if(numButton1 == 2) {return;} BLUETOOTHAPPS(); PressedButton(0); break;}  
                                    if(numButton1 == 5 && rectMenuY == 44)  {BLE_MOUSE();    if(numButton1 == 2) {return;} BLUETOOTHAPPS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 66)  {}
                                    if(numButton1 == 5 && rectMenuY == 88)  {NOTE(8);  MyText = ""; if(numButton1 == 2) {return;} BLUETOOTHAPPS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 110) {NOTE(9);  MyText = ""; if(numButton1 == 2) {return;} BLUETOOTHAPPS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 132) {NOTE(10); MyText = ""; if(numButton1 == 2) {return;} BLUETOOTHAPPS(); PressedButton(0); break;}             
                                                       
                          }//END WHILE 
                          InMenu = 0;  
}
////////////////////////////////////////////////////////////////////////////////////////////
void BLE_COMBO() {
//              if(EEPROM.readInt(515) == 2) {layout = 2; analogWrite(3, BRIG_LAYOUT); //RU LED
//                                                        analogWrite(10,0);  }//ENG LED
//                                      else {layout = 1; analogWrite(3, 0);   //RU LED
//                                                        analogWrite(10,BRIG_LAYOUT);}//ENG LED                                          
//  int no_c = 1;                                         
//////////////////////
//    if(numButton1 == 9 && layout == 1) {analogWrite(3, BRIG_LAYOUT); analogWrite(10, 0);   layout = 2; delay(350); numButton1 = -1;
//                                        EEPROM.writeInt(515, 2); EEPROM.commit();} //CHANGE TO RU//IN THE END BECAUSE FURTHER NO CHECK PRESSED BUTTONS
//    if(numButton1 == 9 && layout == 2) {analogWrite(3, 0);   analogWrite(10, BRIG_LAYOUT); layout = 1; delay(350); numButton1 = -1;
//                                        EEPROM.writeInt(515, 1); EEPROM.commit();} //CHANGE TO ENG//IN THE END BECAUSE FURTHER NO CHECK PRESSED BUTTONS
//////////////////////                       
//   Keyboard.end();
//                             analogWrite(3,  BRIG_LAYOUT); //RU LED  //EXIT
//                             analogWrite(10, BRIG_LAYOUT); //ENG LED //EXIT
}
//////////////////////////////////////////////////////////////////////////////////////////// 
void CHECK_SYMBOL() {                           
    if(numButton0 == 10){StrToBLE = "`"; if(numButton1 == 7) {StrToBLE = "~";} if(numButton1 == 8) {StrToBLE = "1";} return;}
    if(numButton0 == 9) {StrToBLE = "q"; if(numButton1 == 7) {StrToBLE = "Q";} if(numButton1 == 8) {StrToBLE = "2";} return;}
    if(numButton0 == 8) {StrToBLE = "w"; if(numButton1 == 7) {StrToBLE = "W";} if(numButton1 == 8) {StrToBLE = "3";} return;}
    if(numButton0 == 7) {StrToBLE = "e"; if(numButton1 == 7) {StrToBLE = "E";} if(numButton1 == 8) {StrToBLE = "4";} return;}
    if(numButton0 == 6) {StrToBLE = "r"; if(numButton1 == 7) {StrToBLE = "R";} if(numButton1 == 8) {StrToBLE = "5";} return;} 
    if(numButton0 == 5) {StrToBLE = "t"; if(numButton1 == 7) {StrToBLE = "T";} if(numButton1 == 8) {StrToBLE = "6";} return;}
    if(numButton0 == 4) {StrToBLE = "y"; if(numButton1 == 7) {StrToBLE = "Y";} if(numButton1 == 8) {StrToBLE = "7";} return;} 
    if(numButton0 == 3) {StrToBLE = "u"; if(numButton1 == 7) {StrToBLE = "U";} if(numButton1 == 8) {StrToBLE = "8";} return;}
    if(numButton0 == 2) {StrToBLE = "i"; if(numButton1 == 7) {StrToBLE = "I";} if(numButton1 == 8) {StrToBLE = "9";} return;} 
    if(numButton0 == 1) {StrToBLE = "o"; if(numButton1 == 7) {StrToBLE = "O";} if(numButton1 == 8) {StrToBLE = "0";} return;}
    if(numButton0 == 0) {StrToBLE = "p"; if(numButton1 == 7) {StrToBLE = "P";} if(numButton1 == 8) {StrToBLE = "-";} return;} 
        
    if(numButton2 == 10){StrToBLE = "a"; if(numButton1 == 7) {StrToBLE = "A";} if(numButton1 == 8) {StrToBLE = "\134";} return;}
    if(numButton2 == 9) {StrToBLE = "s"; if(numButton1 == 7) {StrToBLE = "S";} if(numButton1 == 8) {StrToBLE = "/";}    return;}
    if(numButton2 == 8) {StrToBLE = "d"; if(numButton1 == 7) {StrToBLE = "D";} if(numButton1 == 8) {StrToBLE = "@";}    return;}
    if(numButton2 == 7) {StrToBLE = "f"; if(numButton1 == 7) {StrToBLE = "F";} if(numButton1 == 8) {StrToBLE = ":";}    return;}
    if(numButton2 == 6) {StrToBLE = "g"; if(numButton1 == 7) {StrToBLE = "G";} if(numButton1 == 8) {StrToBLE = ";";}    return;} 
    if(numButton2 == 5) {StrToBLE = "h"; if(numButton1 == 7) {StrToBLE = "H";} if(numButton1 == 8) {StrToBLE = "%";}    return;}
    if(numButton2 == 4) {StrToBLE = "j"; if(numButton1 == 7) {StrToBLE = "J";} if(numButton1 == 8) {StrToBLE = "\42";}  return;} 
    if(numButton2 == 3) {StrToBLE = "k"; if(numButton1 == 7) {StrToBLE = "K";} if(numButton1 == 8) {StrToBLE = "*";}    return;}
    if(numButton2 == 2) {StrToBLE = "l"; if(numButton1 == 7) {StrToBLE = "L";} if(numButton1 == 8) {StrToBLE = "(";}    return;} 
    if(numButton2 == 1) {StrToBLE = ";"; if(numButton1 == 7) {StrToBLE = ":";} if(numButton1 == 8) {StrToBLE = ")";}    return;}
    if(numButton2 == 0) {StrToBLE = "\47"; if(numButton1 == 7) {StrToBLE = "\42";} if(numButton1 == 8) {StrToBLE = "!";}    return;} //' AND " FROM STANDART KEYBOARD

    if(numButton3 == 11){StrToBLE = "z";   if(numButton1 == 7) {StrToBLE = "Z";}   if(numButton1 == 8) {StrToBLE = "<";} return;}
    if(numButton3 == 10){StrToBLE = "x";   if(numButton1 == 7) {StrToBLE = "X";}   if(numButton1 == 8) {StrToBLE = ">";} return;}
    if(numButton3 == 9) {StrToBLE = "c";   if(numButton1 == 7) {StrToBLE = "C";}   if(numButton1 == 8) {StrToBLE = "[";} return;}
    if(numButton3 == 8) {StrToBLE = "\40"; if(numButton1 == 7) {StrToBLE = "_";}   if(numButton1 == 8) {StrToBLE = "|";} return;}
    if(numButton3 == 7) {StrToBLE = "v";   if(numButton1 == 7) {StrToBLE = "V";}   if(numButton1 == 8) {StrToBLE = "]";} return;} 
    if(numButton3 == 6) {StrToBLE = "b";   if(numButton1 == 7) {StrToBLE = "B";}   if(numButton1 == 8) {StrToBLE = "{";} return;}
    if(numButton3 == 5) {StrToBLE = "n";   if(numButton1 == 7) {StrToBLE = "N";}   if(numButton1 == 8) {StrToBLE = "}";} return;} 
    if(numButton3 == 4) {StrToBLE = "m";   if(numButton1 == 7) {StrToBLE = "M";}   if(numButton1 == 8) {StrToBLE = "?";} return;}
    if(numButton3 == 3) {StrToBLE = ",";   if(numButton1 == 7) {StrToBLE = "<";}   if(numButton1 == 8) {StrToBLE = "?";} return;} //, IN RU = 
    if(numButton3 == 2) {StrToBLE = ".";   if(numButton1 == 7) {StrToBLE = ">";}   if(numButton1 == 8) {StrToBLE = "/";} return;} //. IN RU = 
    if(numButton3 == 1) {StrToBLE = "]";   if(numButton1 == 7) {StrToBLE = "}";}   if(numButton1 == 8) {StrToBLE = "=";} return;}  
    if(numButton3 == 0) {StrToBLE = "[";   if(numButton1 == 7) {StrToBLE = "{";}   if(numButton1 == 8) {StrToBLE = "+";} return;}                             
}
//////////////////////////////////////////////////////////////////////////////////////////// 
void SEND_STRING_BLE(int r) {
  
if(r == 1) {Keyboard.press(KEY_LEFT_GUI); Keyboard.print(" "); delay(100); Keyboard.releaseAll();} //WINDOWS+SPACE
           
if(r == 0) {int xProgressBar = 0;
            tft.fillRect(0, 140, 240, 7, color565(0,0,27)); //PROGRESS BAR BACKGROUND
int charByteStart, charByteEnd, dobByte, strLen = noteStr.length();
for (charByteStart=0; charByteStart<strLen+0; charByteStart+=dobByte) {
                        if(noteStr.charAt(charByteStart) == '\320' ||
                           noteStr.charAt(charByteStart) == '\321') {dobByte=2;}//+BYTE IF RUSSIAN   
                                                               else {dobByte=1;}    
                                                           charByteEnd=charByteStart+dobByte;        
substrSpecific = noteStr.substring(charByteStart, charByteEnd);
                                                           Keyboard.print(substrSpecific); delay(60);
                                                           tft.fillRect(xProgressBar, 140, 2, 7,   color565(0,255,0)); //PROGRESS BAR
                                                           xProgressBar += 2; 
                                                                      }
                                                           tft.fillRect(0, 140, 240, 7, color565(0,255,0)); //PROGRESS BAR FULL
           }
}
//////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////// 
void BLE_KEYBOARD() {
//              if(EEPROM.readInt(515) == 2) {layout = 2; analogWrite(3, BRIG_LAYOUT); //RU  LED
//                                                        analogWrite(10,0);          }//ENG LED
//                                      else {layout = 1; analogWrite(3, 0);           //RU  LED
//                                                        analogWrite(10,BRIG_LAYOUT);}//ENG LED                                          
  int no_c = 1;
  tft.fillScreen(color565(0,0,70)); startX = 37; startY = 0;  MyText = "BLE KEYBOARD";   charColo = WHITE; event();
                                                                            
                                    tft.drawFastHLine(37, 23, 156, color565(250,0,0));
                                    tft.drawFastHLine(37, 24, 156, color565(250,0,0));
                                    startX = 10;  startY = 24; MyText = "==="; charColo = color565(210,0,250); event();  
                                                               MyText = "Just print:"; charColo = WHITE; event();
                                                               MyText = "==="; charColo = color565(210,0,250); event();
   tft.fillRect(0, 168, 240, 75, color565(0,0,155));//BLUE BACKGROUND
   startX = 2; startY = 168; MyText = "Внимание! 'layout' меняет раскладку на ПК, android, .."; charColo = color565(250,170,190); event();//PINK 
                                                               
  //if(!Keyboard.isConnected()) {Keyboard.begin(); Mouse.begin();}
  
  while(!Keyboard.isConnected()) {PressedButton(0); if(numButton1 == 10 || numButton1 == 2) {return;}
                                  startX = 0; startY = 146; MyText = "no_c"; charColo = RED; event(); charColo = WHITE;}
                                  tft.fillRect(0, 146, 56, 22,color565(0,0,70)); //ERASE NO_C
//  int alreadyDrawC = 0;
//  int alreadyDrawRect = 0;

  PressedButton(1);   timeCursor = millis();
//====================================================================================================//                             
  while(numButton1 != 10) { PressedButton(1); //if(numButton1 == 2) {return;} //CHECK ALL KEYS
                                         if(millis() - timeCursor < 90) {continue;}//PAUSE FOR COMFORT                            
                                         if((numButton0 != -1 || numButton1 != -1 || numButton2 != -1 || numButton3 != -1)) {timeCursor = millis();}//PAUSE ONLY IF SYMBOL KEY PRESSED
                                         //if(numButton1 != -1) {timeCursor = millis()-60;}
//  if(!Keyboard.isConnected()) {Keyboard.end(); Keyboard.begin();}
//  if(!Keyboard.isConnected()) {if(alreadyDrawC == 0)    {alreadyDrawC = 1;    alreadyDrawRect = 0; startX = 0; startY = 212; MyText = "no_c"; charColo = RED; event(); charColo = WHITE;}}
//                         else {if(alreadyDrawRect == 0) {alreadyDrawRect = 1; alreadyDrawC = 0;    tft.fillRect(0, 212, 56, 22,color565(0,0,70));}} //ERASE NO_C

     //PressedButton(1); //ALL KEYS
     if(numButton1 == 6) {Keyboard.write(KEY_BACKSPACE);}   //BACKSPASE
     if(numButton1 == 5) {Keyboard.write(KEY_RETURN);}      //ENTER
     if(numButton1 == 0) {Keyboard.write(KEY_UP_ARROW);}    //
     if(numButton1 == 1) {Keyboard.write(KEY_DOWN_ARROW);}  //
     if(numButton1 == 2) {Keyboard.write(KEY_LEFT_GUI);}    //START MENU
     if(numButton1 == 4) {Keyboard.write(KEY_LEFT_ARROW);}  //
     if(numButton1 == 3) {Keyboard.write(KEY_RIGHT_ARROW);} //
      
     if(numButton1 == 9) {Keyboard.press(KEY_LEFT_GUI); Keyboard.print(" "); delay(100); Keyboard.releaseAll();} //WINDOWS+SPACE 
     
      
     if(numButton0 != -1 || numButton2 != -1 || numButton3 != -1) {CHECK_SYMBOL();
                                                                   Keyboard.print(StrToBLE);  
//                                                                   tft.fillRect(90, 50, 62, 78,color565(0,30,120)); //ERASE PREVIOUS
//                                                                   startX = 90; startY = 50; scaleX = 4; scaleY = 3; charColo = color565(100,120,250); event(); //charColo = WHITE;
//                                                                   MyText = "";
                                                                  }                                         
//////////////////////
//    if(numButton1 == 9 && layout == 1) {analogWrite(3, BRIG_LAYOUT); analogWrite(10, 0);   layout = 2; delay(350); numButton1 = -1;
//                                        EEPROM.writeInt(515, 2); EEPROM.commit();} //CHANGE TO RU//IN THE END BECAUSE FURTHER NO CHECK PRESSED BUTTONS
//    if(numButton1 == 9 && layout == 2) {analogWrite(3, 0);   analogWrite(10, BRIG_LAYOUT); layout = 1; delay(350); numButton1 = -1;
//                                        EEPROM.writeInt(515, 1); EEPROM.commit();} //CHANGE TO ENG//IN THE END BECAUSE FURTHER NO CHECK PRESSED BUTTONS
//////////////////////        
                         }//END WHILE                 
   //Keyboard.end(); //Mouse.end();
//                             analogWrite(3,  BRIG_LAYOUT); //RU LED  //EXIT
//                             analogWrite(10, BRIG_LAYOUT); //ENG LED //EXIT
}
//////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////// 
void BLE_MOUSE() {
  int no_c = 1;
  tft.fillScreen(color565(0,0,70)); startX = 58; startY = 0; MyText = "BLE Mouse"; charColo = WHITE; event();
  tft.fillRect(108, 47, 53, 73,color565(0,130,50));
  startX = 120; startY = 57; scaleX = 2; charColo = WHITE; symbolDraw(169); charColo = WHITE; //UP
  startX = 120; startY = 87; scaleX = 2; charColo = WHITE; symbolDraw(170); charColo = WHITE; //DOWN
  scaleX = 1; 
  startX = 200; startY = 130; symbolDraw(167); //LEFT
  startX = 226; startY = 130; symbolDraw(168); //RIGHT
  startX = 0; startY = 191; MyText = "SHIFT"; event();
  startX = 0; startY = 213; MyText = "EXTRA"; event();
  startX = 70; startY = 191; MyText = "L_bu"; event();
  startX = 70; startY = 213; MyText = "R_bu"; event();
  //tft.drawLine(10, 10, 5, 5, WHITE);
  tft.drawFastVLine(126, 190, 50, color565(250,0,0));
  tft.drawFastVLine(127, 190, 50, color565(250,0,0));
  startX = 131; startY = 191; MyText = "ENTER"; event();
  startX = 131; startY = 213; MyText = "BACKS"; event();
  startX = 200; startY = 190; MyText = "SCRL"; scaleY = 2; event(); scaleY = 1;

  //if(!Keyboard.isConnected()) {Keyboard.begin(); Mouse.begin();}
  
  while(!Keyboard.isConnected()) {PressedButton(0); if(numButton1 == 10 || numButton1 == 2) {return;}
                                  startX = 0; startY = 0; MyText = "no_c"; charColo = RED; event(); charColo = WHITE;}
                                  tft.fillRect(0, 0, 56, 22,color565(0,0,70)); //ERASE NO_C
                                  
  while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;} //0=SINGLE CIRCUIT
                                     
     if(numButton1 == 0) {while(numButton1 != -1) {Mouse.move( 0,-10); PressedButton(0);}} //UP  
     if(numButton1 == 1) {while(numButton1 != -1) {Mouse.move( 0, 10); PressedButton(0);}} //DOWN  
     if(numButton1 == 3) {while(numButton1 != -1) {Mouse.move( 10, 0); PressedButton(0);}} //RIGHT  
     if(numButton1 == 4) {while(numButton1 != -1) {Mouse.move(-10, 0); PressedButton(0);}} //LEFT 
     if(numButton1 == 5) {while(numButton1 != -1) {Mouse.move(0,0, 1); PressedButton(0);}} //SCROLL UP  
     if(numButton1 == 6) {while(numButton1 != -1) {Mouse.move(0,0,-1); PressedButton(0);}} //SCROLL DOWN 
     if(numButton1 == 7) {Mouse.click(MOUSE_LEFT);  delay(40);}
     if(numButton1 == 8) {Mouse.click(MOUSE_RIGHT); delay(40);} 
                
                          }//END WHILE  
   //Keyboard.end(); //Mouse.end();
}
//////////////////////////////////////////////////////////////////////////////////////////// 
void UART_CHAT() {
  tft.fillScreen(color565(255,255,255));
  tft.fillRoundRect(-5,165,250,73,15,BLUE); //DRAW ROUNDRECT BOTTOM
  tft.fillRoundRect(-5,2,250,73,15,color565(210,190,255)); //DRAW ROUNDRECT TOP
  tft.fillRoundRect(-5,77,250,73,15,color565(255,180,210)); //DRAW ROUNDRECT MIDDLE
  MyText = "115200"; startX = 158; startY = 210; charColo = color565(200,200,200); event(); charColo = WHITE;
  //Serial.begin(115200);
  while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;}
  noteStr = "";
  NOTE(4); 
  if(noteStr != "" && numButton1 == 5) {Serial.begin(115200); MyText = noteStr; Serial.println(MyText);
                                        //tft.fillRect(0,0,240,75,WHITE); 
                                        tft.fillRoundRect(-5,2,250,73,15,color565(200,200,255)); //CLEAN TOP WINDOW
                                        startX = 2; startY = 2; scaleX = 1; scaleY = 1; charColo = BLACK; event(); charColo = WHITE; 
                                        tft.fillRoundRect(-5,165,250,73,15,BLUE); //CLEAN BOTTOM WINDOW 
                                        noteStr = ""; MyText = "";}
                         }
  Serial.end();
}
//////////////////////////////////////////////////////////////////////////////////////////// 
void UART_DRIVE() {
tft.fillScreen(color565(220,200,250));
  tft.fillRoundRect(-5,26,250,50,15,color565(250,250,250)); //DRAW ROUNDRECT MIDDLE
          MyText = "Клиент должен бытьнастроен на       обработку строк:  UP DOWN LEFT RIGHTENTER MENU BACK   Скорость: 115200";
          charColo = BLACK; startX = 2; startY = 101; scaleX = 1; scaleY = 1; event();
          MyText = "Отправляем в порт:";
          charColo = BLACK; startX = 2; startY = 1;   scaleX = 1; scaleY = 1; event();
//Serial.begin(115200);
startX = 55; startY = 35; scaleX = 2; scaleY = 1;
int last_1 = 0, last0 = 0, last1 = 0, last2 = 0, last3 = 0, last4 = 0, last5 = 0, last6 = 0; String MyTextLast = "EMPTY";
int BU;
while(numButton1 != 10) { PressedButton(0); BU = numButton1; //if(numButton1 == 2) {return;}//"MENU" ENABLE IN FUNCTION
if(BU == -1) {MyText = "NONE";  if(MyTextLast != MyText) {MyTextLast = MyText; goto drawNew;}} 
if(BU == 0)  {MyText = "UP";    if(MyTextLast != MyText) {MyTextLast = MyText; goto drawNew;}} 
if(BU == 1)  {MyText = "DOWN";  if(MyTextLast != MyText) {MyTextLast = MyText; goto drawNew;}} 
if(BU == 2)  {MyText = "MENU";  if(MyTextLast != MyText) {MyTextLast = MyText; goto drawNew;}} 
if(BU == 3)  {MyText = "RIGHT"; if(MyTextLast != MyText) {MyTextLast = MyText; goto drawNew;}} 
if(BU == 4)  {MyText = "LEFT";  if(MyTextLast != MyText) {MyTextLast = MyText; goto drawNew;}} 
if(BU == 5)  {MyText = "ENTER"; if(MyTextLast != MyText) {MyTextLast = MyText; goto drawNew;}}
if(BU == 6)  {MyText = "BACK";  if(MyTextLast != MyText) {MyTextLast = MyText; goto drawNew;}}
continue;
drawNew: //DRAW IF CHANGED ONLY
                                             Serial.begin(115200);
                                             tft.fillRect(55, 35, 130, 27,color565(250,250,250));
                                             Serial.println(MyText);
                                             startX = 55; startY = 35; scaleX = 2; scaleY = 1; event();
                       } Serial.end();
}
//////////////////////////////////////////////////////////////////////////////////////////// 
void SETTINGS() {
        tft.fillScreen(BackColor); scaleX = 1; scaleY = 1; charColo = WHITE; 
   startX = 19; startY = 2;   MyText = "Яркость дисплея";  event();
   startX = 19; startY = 24;  MyText = "Яркость РУ/EN";    event();
   startX = 19; startY = 46;  MyText = "Цвет оформления";  event();
   startX = 19; startY = 68;  MyText = "Wi-Fi при вкл-ии"; event();
   startX = 19; startY = 90;  MyText = "Точность данных";  event();
   startX = 19; startY = 112; MyText = "TimeOffset";       event();
   startX = 19; startY = 134; MyText = "Режим сна";        charColo = inactElem; event(); charColo = WHITE;
   

   rectMenuX = 0; rectMenuY = 0; //CURSOR
    tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));//FIRST 
    double pau = millis();
    PressedButton(0);                   
   while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;}  
     if((numButton1 == 3 || numButton1 == 1) && rectMenuY < 130 && millis() - pau > 100) {tft.fillRect(0, 0, 17, 158,BackColor); //TO BACKGROUND COLOR 7 ROWS
                             rectMenuY+=22; tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));
                             pau = millis();}//NEXT    
     if((numButton1 == 4 || numButton1 == 0) && rectMenuY > 10 &&  millis() - pau > 100) {tft.fillRect(0, 0, 17, 158,BackColor); //TO BACKGROUND COLOR 7 ROWS
                             rectMenuY-=22; tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));
                             pau = millis();}//PREV 

                                   //IF ENTER KEY PRESSED
                                    if(numButton1 == 5 && rectMenuY == 0)   {BRIGHTNESS();        if(numButton1 == 2) {return;} SETTINGS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 22)  {BRIGHTNESS_LAYOUT(); if(numButton1 == 2) {return;} SETTINGS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 44)  {CHANGE_THEME();      if(numButton1 == 2) {return;} SETTINGS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 66)  {WIFI_ON_START();     if(numButton1 == 2) {return;} SETTINGS(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 88)  {FINEDATA();          if(numButton1 == 2) {return;} SETTINGS(); PressedButton(0); break;}  
                                    if(numButton1 == 5 && rectMenuY == 110) {TIMEOFFSETLOC();     if(numButton1 == 2) {return;} SETTINGS(); PressedButton(0); break;} 
                                    if(numButton1 == 5 && rectMenuY == 132) {}              
                                                       
                          }//END WHILE 
                          InMenu = 0;  
}
////////////////////////////////////////////////////////////////////////////////////////////
void TIMEOFFSETLOC() {
  tft.fillScreen(BackColor);
  tft.fillRect(126, 90, 92, 48,color565(180,180,180)); // 
  MyText = "UTC"; startX = 40; startY = 90; scaleX = 2; scaleY = 2; event();
  int SavedUTC = EEPROM.readInt(808);
  if(SavedUTC-12 > 12 || SavedUTC-12 < -12) {SavedUTC == 12; EEPROM.writeInt(808, SavedUTC); EEPROM.commit();} //UTC = +0;  
  if(SavedUTC-12 > -0.5) {MyText = "+" + String(SavedUTC-12);} else {MyText = String(SavedUTC-12);}  
                  startX = 130; startY = 90; scaleX = 2; scaleY = 2; event();
                  
    PressedButton(0);
    while(numButton1 != 10 && numButton1 != 5) { PressedButton(0); if(numButton1 == 2) {EEPROM.writeInt(808, SavedUTC); EEPROM.commit(); 
                                                                                        scaleX = 1; scaleY = 1; return;} 
               if((numButton1 == 3 || numButton1 == 0) && SavedUTC-12 < 11.5)  {SavedUTC += 1; 
                     if(SavedUTC-12 > -0.5) {MyText = "+" + String(SavedUTC-12);} else {MyText = String(SavedUTC-12);} 
                     tft.fillRect(126, 90, 92, 48,color565(180,180,180));
                     startX = 130; startY = 90; scaleX = 2; scaleY = 2; event();}
                     
               if((numButton1 == 4 || numButton1 == 1) && SavedUTC-12 > -11.5)  {SavedUTC -= 1;
                     if(SavedUTC-12 > -0.5) {MyText = "+" + String(SavedUTC-12);} else {MyText = String(SavedUTC-12);} 
                     tft.fillRect(126, 90, 92, 48,color565(180,180,180));
                     startX = 130; startY = 90; scaleX = 2; scaleY = 2; event();}
                                                     }  EEPROM.writeInt(808, SavedUTC); EEPROM.commit(); //numButton1 = -1;
                                                        scaleX = 1; scaleY = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////
void FINEDATA() {
  tempFine = EEPROM.readInt(800); humFine = EEPROM.readInt(804);
  if(tempFine < 1 || tempFine > 17) {tempFine = 9; EEPROM.writeInt(800, tempFine); EEPROM.commit();}
  if(humFine < 1 || humFine > 17)   {humFine = 9;  EEPROM.writeInt(804, humFine);  EEPROM.commit();}

  tft.fillScreen(BackColor); 
  tft.drawBitmap(4, 5,  TEMP_PIC, 35, 47, WHITE);
  tft.drawBitmap(4, 60, H_PIC,    35, 47, WHITE);
               tft.fillRoundRect(170,5, 50,50,10,WHITE); 
               tft.fillRoundRect(170,60,50,50,10,WHITE);
            
            if(tempFine == 9) {startX = 180;} else {startX = 167;} startY = 5;  MyText = String(tempFine-9); if(tempFine > 9) {MyText = "+" + String(tempFine-9);} if(tempFine < 9) {MyText = String(tempFine-9);} scaleX = 2; scaleY = 2; charColo = color565(210,0,0); event(); 
            if(humFine == 9)  {startX = 180;} else {startX = 167;} startY = 60; MyText = String(humFine-9);  if(humFine > 9)  {MyText = "+" + String(humFine-9);}  if(humFine < 9)  {MyText = String(humFine-9);}  scaleX = 2; scaleY = 2; charColo = color565(210,0,0); event();
            startX = 148; startY = 15; scaleX = 1; scaleY = 1; charColo = WHITE; symbolDraw(168);
            
   double pau = millis();            
   while(true) { PressedButton(0); if(numButton1 == 2 || numButton1 == 10) {        
                                       EEPROM.writeInt(800, tempFine); EEPROM.commit();
                                       EEPROM.writeInt(804, humFine);  EEPROM.commit();
                                       if(numButton1 == 2) {return;} if(numButton1 == 10) {break;}
                                                                           }
             if(numButton1 == 0 && startY > 15 && millis() - pau > 100) {tft.fillRect(148, 10, 18, 110,BackColor);
                                                 scaleX = 1; scaleY = 1; startX = 148; startY = 15; charColo = WHITE; symbolDraw(168);
                                                 pau = millis();}
             if(numButton1 == 1 && startY < 70 && millis() - pau > 100) {tft.fillRect(148, 10, 18, 110,BackColor);
                                                 scaleX = 1; scaleY = 1; startX = 148; startY = 70; charColo = WHITE; symbolDraw(168);
                                                 pau = millis();}
             ///////////////////////
             if(numButton1 == 4 && startY == 15 && millis() - pau > 100 && tempFine > 1) {tft.fillRect(171, 10, 48, 40,WHITE); tempFine -= 1;
                                                 if(tempFine == 9) {startX = 180;} else {startX = 167;} startY = 5;  MyText = String(tempFine-9); // ALIGN 
                                                 if(tempFine > 9) {MyText = "+" + String(tempFine-9);}
                                                 if(tempFine < 9) {MyText = String(tempFine-9);}
                                                 scaleX = 2; scaleY = 2; charColo = color565(210,0,0); event();
                                                 pau = millis(); startY = 15;}
             if(numButton1 == 3 && startY == 15 && millis() - pau > 100 && tempFine < 17)  {tft.fillRect(171, 10, 48, 40,WHITE); tempFine += 1;
                                                 if(tempFine == 9) {startX = 180;} else {startX = 167;} startY = 5;  MyText = String(tempFine-9); // ALIGN
                                                 if(tempFine > 9) {MyText = "+" + String(tempFine-9);}
                                                 if(tempFine < 9) {MyText = String(tempFine-9);} 
                                                 scaleX = 2; scaleY = 2; charColo = color565(210,0,0); event(); 
                                                 pau = millis(); startY = 15;}
                                                  
             if(numButton1 == 4 && startY == 70 && millis() - pau > 100 && humFine > 1) {tft.fillRect(171, 65, 48, 40,WHITE); humFine -= 1;
                                                 if(humFine == 9) {startX = 180;} else {startX = 167;} startY = 60;  MyText = String(humFine-9); // ALIGN 
                                                 if(humFine > 9) {MyText = "+" + String(humFine-9);}
                                                 if(humFine < 9) {MyText = String(humFine-9);}
                                                 scaleX = 2; scaleY = 2; charColo = color565(210,0,0); event();
                                                 pau = millis(); startY = 70;}
             if(numButton1 == 3 && startY == 70 && millis() - pau > 100 && humFine < 17)  {tft.fillRect(171, 65, 48, 40,WHITE); humFine += 1;
                                                 if(humFine == 9) {startX = 180;} else {startX = 167;} startY = 60;  MyText = String(humFine-9); // ALIGN 
                                                 if(humFine > 9) {MyText = "+" + String(humFine-9);}
                                                 if(humFine < 9) {MyText = String(humFine-9);}
                                                 scaleX = 2; scaleY = 2; charColo = color565(210,0,0); event(); 
                                                 pau = millis(); startY = 70;}                       
                           }           
}
////////////////////////////////////////////////////////////////////////////////////////////
void SMART_HOME() {
        tft.fillScreen(BackColor); 
   startX = 19; startY = 2;   MyText = "Инф-я с датчиков"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 24;  MyText = "Освещение"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 46;  MyText = "Шторы"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 68;  MyText = "Отопление"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 90;  MyText = "Музыка"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 112; MyText = "Фото с камеры"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 134; MyText = "Ворота"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 156; MyText = "Генератор"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 178; MyText = "Канал связи"; event();
   startX = 196; startY = 213; scaleX = 3; scaleY = 1; symbolDraw(171); scaleX = 1; scaleY = 1; //HOME PIC
     
   rectMenuX = 0; rectMenuY = 0;//CURSOR
    tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));//FIRST 
    double pau = millis();
    PressedButton(0);                   
   while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;}
     if((numButton1 == 3 || numButton1 == 1) && rectMenuY < 175 && millis() - pau > 100) {tft.fillRect(0, 0, 17, 202,BackColor); //TO BACKGROUND COLOR 9 ROWS
                             rectMenuY+=22; tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));
                             pau = millis();}//NEXT    
     if((numButton1 == 4 || numButton1 == 0) && rectMenuY > 10  && millis() - pau > 100) {tft.fillRect(0, 0, 17, 202,BackColor); //TO BACKGROUND COLOR 9 ROWS
                             rectMenuY-=22; tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));
                             pau = millis();}//PREV 

                                   //IF ENTER KEY PRESSED
                                    if(numButton1 == 5 && rectMenuY == 0)   {  PressedButton(0);}
                                    if(numButton1 == 5 && rectMenuY == 22)  {  PressedButton(0);}
                                    if(numButton1 == 5 && rectMenuY == 44)  {  PressedButton(0);}
                                    if(numButton1 == 5 && rectMenuY == 66)  {  PressedButton(0);}
                                    if(numButton1 == 5 && rectMenuY == 88)  {  PressedButton(0);}    
                                    if(numButton1 == 5 && rectMenuY == 110) {  PressedButton(0);}
                                    if(numButton1 == 5 && rectMenuY == 132) {  PressedButton(0);}
                                    if(numButton1 == 5 && rectMenuY == 154) {  PressedButton(0);}
                                    if(numButton1 == 5 && rectMenuY == 176) {SMARTHOMECHANNEL(); if(numButton1 == 2) {return;} SMART_HOME(); PressedButton(0); break;}             
                                                       
                          }//END WHILE 
                          InMenu = 0;   
}
////////////////////////////////////////////////////////////////////////////////////////////
void SMARTHOMECHANNEL() {
        tft.fillScreen(BackColor); tft.fillRect(0, 90, 240, 22,color565(170,40,240));
   startX = 19; startY = 2;  MyText = "Internet"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 24; MyText = "Wi-Fi"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 46; MyText = "Bluetooth"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 68; MyText = "UART"; charColo = inactElem; event(); charColo = WHITE;
   
   startX = 57; startY = 90; MyText = "ПАРАМЕТРЫ"; charColo = BLACK; event(); charColo = WHITE;
   startX = 19; startY = 112; MyText = "Internet"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 134; MyText = "Wi-Fi"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 156; MyText = "Bluetooth"; charColo = inactElem; event(); charColo = WHITE;
   startX = 19; startY = 178; MyText = "UART"; charColo = inactElem; event(); charColo = WHITE;
   startX = 196; startY = 213; scaleX = 3; scaleY = 1; symbolDraw(171); scaleX = 1; scaleY = 1; //HOME PIC
   
    if(EEPROM.readInt(520) == 0) {startX = 223; startY = 0;  symbolDraw(167);}
    if(EEPROM.readInt(520) == 1) {startX = 223; startY = 22; symbolDraw(167);}
    if(EEPROM.readInt(520) == 2) {startX = 223; startY = 44; symbolDraw(167);}
    if(EEPROM.readInt(520) == 3) {startX = 223; startY = 66; symbolDraw(167);}

   rectMenuX = 0; rectMenuY = 1;//CURSOR
    tft.fillRect(rectMenuX, rectMenuY+1, 17, 22,color565(255,255,255));//FIRST 
    double pau = millis();
    PressedButton(0);
   while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;}
     if((numButton1 == 3 || numButton1 == 1) && rectMenuY < 175 && millis() - pau > 100) {tft.fillRect(0, 0, 17, 90,BackColor); tft.fillRect(0, 112, 17, 100,BackColor); //TO BACKGROUND COLOR 
                             rectMenuY+=22; if (rectMenuY == 89) {rectMenuY += 22;} tft.fillRect(rectMenuX, rectMenuY+1, 17, 22,color565(255,255,255));
                             pau = millis();}//NEXT    
     if((numButton1 == 4 || numButton1 == 0) && rectMenuY > 10 && millis() - pau > 100)  {tft.fillRect(0, 0, 17, 90,BackColor); tft.fillRect(0, 112, 17, 100,BackColor); //TO BACKGROUND COLOR 
                             rectMenuY-=22; if (rectMenuY == 89) {rectMenuY -= 22;} tft.fillRect(rectMenuX, rectMenuY+1, 17, 22,color565(255,255,255));
                             pau = millis();}//PREV 

                                   if(numButton1 == 5) {//IF ENTER KEY PRESSED
                                    if(rectMenuY == 1)  {EEPROM.writeInt(520, 0); EEPROM.commit();
                                                         tft.fillRect(223, 0, 17, 90,BackColor);
                                                         startX = 223; startY = 0;  symbolDraw(167);}
                                    if(rectMenuY == 23) {EEPROM.writeInt(520, 1); EEPROM.commit();
                                                         tft.fillRect(223, 0, 17, 90,BackColor);
                                                         startX = 223; startY = 22; symbolDraw(167);} 
                                    if(rectMenuY == 45) {EEPROM.writeInt(520, 2); EEPROM.commit();
                                                         tft.fillRect(223, 0, 17, 90,BackColor);
                                                         startX = 223; startY = 44; symbolDraw(167);}
                                    if(rectMenuY == 67) {EEPROM.writeInt(520, 3); EEPROM.commit();
                                                         tft.fillRect(223, 0, 17, 90,BackColor);
                                                         startX = 223; startY = 66; symbolDraw(167);}              
                                                       }
                          }//END WHILE     
}
////////////////////////////////////////////////////////////////////////////////////////////
void BRIGHTNESS() {
                   tft.fillRect(0, 0,   240, 60,BackColor);
                   tft.fillRect(0, 60,  240, 60,color565(255,255,255));
                   tft.fillRect(0, 120, 240, 60,color565(0,0,0));
                   tft.fillRect(0, 180, 240, 60,color565(0,255,0));
       int BR0 = BRIG;
                   tft.setFont(&FreeMono12pt7b);
                   //tft.setCursor(5, 35);
                   tft.setTextSize(1, 1);
                   tft.setTextColor(WHITE);
                   tft.setTextWrap(true);
                   tft.setCursor(99, 35); tft.setTextColor(WHITE); tft.print(BR0);
                   PressedButton(0);//NECESSARY
    while(numButton1 != 10 && numButton1 != 5) { PressedButton(0); if(numButton1 == 2) {return;} 
                   if((numButton1 == 3 || numButton1 == 0) && BRIG < 249) {BRIG += 5; analogWrite(7, BRIG); //BL++ MAX 250
                                                      tft.setCursor(99, 35); tft.setTextColor(BackColor);  tft.print(BR0);
                                                      BR0 = BRIG; 
                                                      tft.setCursor(99, 35); tft.setTextColor(WHITE); tft.print(BR0); 
                                                     }
                   if((numButton1 == 4 || numButton1 == 1) && BRIG > 14)  {BRIG -= 5; analogWrite(7, BRIG); //BL-- MIN 10
                                                      tft.setCursor(99, 35); tft.setTextColor(BackColor);  tft.print(BR0);
                                                      BR0 = BRIG; 
                                                      tft.setCursor(99, 35); tft.setTextColor(WHITE); tft.print(BR0);
                                                     }                           
                                              } EEPROM.writeInt(490, BRIG); EEPROM.commit(); //numButton1 = -1; 
                  }
//////////////////////////////////////////////////////////////////////////////////////////// 
void BRIGHTNESS_LAYOUT() {tft.fillScreen(BLACK);
                   startX = 0;   startY = 218;  symbolDraw(170);
                   startX = 226; startY = 218;  symbolDraw(170);
       int BR0 = BRIG_LAYOUT;
                   tft.setFont(&FreeMono12pt7b);
                   //tft.setCursor(5, 35);
                   tft.setTextSize(1, 1);
                   tft.setTextColor(WHITE);
                   tft.setTextWrap(true);
                   tft.setCursor(99, 237); tft.setTextColor(WHITE); tft.print(BR0);
                   PressedButton(0);//NECESSARY
     while(numButton1 != 10 && numButton1 != 5) { PressedButton(0); if(numButton1 == 2) {return;} 
            if((numButton1 == 3 || numButton1 == 0) && BRIG_LAYOUT < 250) {BRIG_LAYOUT += 5; analogWrite(3,  BRIG_LAYOUT); //MAX 250
                                                                        analogWrite(10, BRIG_LAYOUT); //++
                                                      tft.setCursor(99, 237); tft.setTextColor(BLACK); tft.print(BR0);
                                                      BR0 = BRIG_LAYOUT; 
                                                      tft.setCursor(99, 237); tft.setTextColor(WHITE); tft.print(BR0); 
                                                     }
            if((numButton1 == 4 || numButton1 == 1) && BRIG_LAYOUT > 14)  {BRIG_LAYOUT -= 5; analogWrite(3,  BRIG_LAYOUT); //MIN 10
                                                                        analogWrite(10, BRIG_LAYOUT); //--
                                                      tft.setCursor(99, 237); tft.setTextColor(BLACK); tft.print(BR0);
                                                      BR0 = BRIG_LAYOUT; 
                                                      tft.setCursor(99, 237); tft.setTextColor(WHITE); tft.print(BR0);
                                                     }                           
                                               } EEPROM.writeInt(494, BRIG_LAYOUT); EEPROM.commit(); 
                        analogWrite(3,  BRIG_LAYOUT); //RU
                        analogWrite(10, BRIG_LAYOUT); //ENG 
                  }
//////////////////////////////////////////////////////////////////////////////////////////// 
void WIFI_ON_START() {
        tft.fillScreen(BackColor); 
   startX = 19; startY = 2;  MyText = "Автома-е подкл."; event();
   startX = 19; startY = 24; MyText = "Без автоподкл."; event();
    if(EEPROM.readInt(525) == 1) {startX = 223; startY = 2;  symbolDraw(167);}
    if(EEPROM.readInt(525) == 0) {startX = 223; startY = 24; symbolDraw(167);}

   rectMenuX = 0; rectMenuY = 0;//CURSOR
    tft.fillRect(rectMenuX, rectMenuY+1, 17, 22,color565(255,255,255));//FIRST 
    double pau = millis();
    PressedButton(0);
   while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;}
     if((numButton1 == 3 || numButton1 == 1) && rectMenuY < 20 && millis() - pau > 100) {tft.fillRect(0, 0, 17, 52,BackColor); //TO BACKGROUND COLOR 2 ROWS
                             rectMenuY+=22; tft.fillRect(rectMenuX, rectMenuY+1, 17, 22,color565(255,255,255));
                             pau = millis();}//NEXT    
     if((numButton1 == 4 || numButton1 == 0) && rectMenuY > 10 && millis() - pau > 100) {tft.fillRect(0, 0, 17, 52,BackColor); //TO BACKGROUND COLOR 2 ROWS
                             rectMenuY-=22; tft.fillRect(rectMenuX, rectMenuY+1, 17, 22,color565(255,255,255));
                             pau = millis();}//PREV 

                                   if(numButton1 == 5) {//IF ENTER KEY PRESSED
                                    if(rectMenuY == 0)  {EEPROM.writeInt(525, 1); EEPROM.commit();
                                                         tft.fillRect(223, 0, 17, 52,BackColor);
                                                         startX = 223; startY = 2;  symbolDraw(167);}
                                    if(rectMenuY == 22) {EEPROM.writeInt(525, 0); EEPROM.commit();
                                                         tft.fillRect(223, 0, 17, 52,BackColor);
                                                         startX = 223; startY = 24; symbolDraw(167);}                 
                                                       }
                          }//END WHILE   
}
//////////////////////////////////////////////////////////////////////////////////////////// 
void CHANGE_THEME() {

  int colorR[] = {60, 180,0,  0,  0,  50, 90, 0  };
  int colorG[] = {0,  0,  0,  0,  0,  0,  0,  80 };
  int colorB[] = {70, 250,0,  70, 200,20, 0,  0  };

  int flag = 100;
  int wi = 0;
  int he = 0;
  int inte = 0;
  MyText = "Текст";
  while(inte < 8) {
  tft.fillRect(wi,he,120,60,color565(colorR[inte],colorG[inte],colorB[inte])); //DRAW BACKGROUNDS 
  startX = wi + 27; startY = he + 17; event(); 
  wi += 120; if(wi > 239) {wi = 0; he += 60;}
  inte ++;
                  }
  wi = 0; he = 0; inte = 0;    
  while(inte < 8) {
  tft.drawRect(wi,he,120,60,BLACK); //DRAW GRID
  startX = wi + 27; startY = he + 17; event(); 
  wi += 120; if(wi > 239) {wi = 0; he += 60;}
  inte ++;
                  }        

tft.drawRect(0, 0, 120, 60, WHITE);
  wi = 0; he = 0; inte = 0;
  double pau = millis();
while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;}
  if(inte < 7 && (numButton1 == 3 || numButton1 == 1) && millis() - pau > 100) {inte++; //NEXT RECT
         tft.drawRect(wi, he, 120, 60, BLACK);
         wi += 120; if(wi > 239) {wi = 0; he += 60;}
         tft.drawRect(wi, he, 120, 60, WHITE);
         pau = millis();
                      }
  if(inte > 0 && (numButton1 == 4 || numButton1 == 0) && millis() - pau > 100) {inte--; //PREVIOUS RECT
         tft.drawRect(wi, he, 120, 60, BLACK);
         wi -= 120; if(wi < 0) {wi = 120; he -= 60;}
         tft.drawRect(wi, he, 120, 60, WHITE);
         pau = millis();
                      }
  if(numButton1 == 5) {//IF ENTER
         EEPROM.writeInt(500, flag); EEPROM.commit();//FLAG FOR NOT DEFAULT
         EEPROM.writeInt(505, colorR[inte]); EEPROM.commit();
         EEPROM.writeInt(506, colorG[inte]); EEPROM.commit();
         EEPROM.writeInt(507, colorB[inte]); EEPROM.commit();
         //BackColor = color565(colorR[inte],colorG[inte],colorB[inte]);

  if(EEPROM.readInt(500) == 100) {BackColor = color565(EEPROM.readInt(505),EEPROM.readInt(506),EEPROM.readInt(507));} //SET NEW THEME
                            else {BackColor = color565(60,0,70);} //IF FAIL
                            break;
                      }
}
}
////////////////////////////////////////////////////////////////////////////////////////////
void WEATHER() {
        tft.fillScreen(BackColor); 
   startX = 0; startY = 210;  MyText = "openweathermap.org"; charColo = color565(180,180,250); event(); charColo = WHITE;     
   startX = 19; startY = 2;   MyText = "Получить погоду"; charColo = WHITE; event(); charColo = WHITE;
   startX = 19; startY = 24;  MyText = "Город"; charColo = WHITE; event(); charColo = WHITE;
   startX = 19; startY = 46;  MyText = "Страна"; charColo = WHITE; event(); charColo = WHITE;
   startX = 19; startY = 68;  MyText = "API key"; charColo = WHITE; event(); charColo = WHITE;
     
   rectMenuX = 0; rectMenuY = 0;//CURSOR
    tft.fillRect(rectMenuX, rectMenuY+2, 17, 22, color565(255,255,255));//FIRST 
    if(WiFi.status() == WL_CONNECTED) {tft.drawBitmap(214, 0,  NET_PIC, 25, 21, WHITE);}
    double pau = millis();
    PressedButton(0);                   
   while(numButton1 != 10) { PressedButton(0); if(numButton1 == 2) {return;}
     if((numButton1 == 3 || numButton1 == 1) && rectMenuY < 65 && millis() - pau > 100) {tft.fillRect(0, 0, 17, 94,BackColor); //TO BACKGROUND COLOR 4 ROWS
                             rectMenuY+=22; tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));
                             pau = millis();}//NEXT    
     if((numButton1 == 4 || numButton1 == 0) && rectMenuY > 10 && millis() - pau > 100) {tft.fillRect(0, 0, 17, 94,BackColor); //TO BACKGROUND COLOR 4 ROWS
                             rectMenuY-=22; tft.fillRect(rectMenuX, rectMenuY+2, 17, 22,color565(255,255,255));
                             pau = millis();}//PREV 

                                   //IF ENTER KEY PRESSED
                                    if(numButton1 == 5 && rectMenuY == 0)   {GET_WEATHER(); if(numButton1 == 2) {return;} PressedButton(0); WEATHER(); PressedButton(0); break;}
                                    if(numButton1 == 5 && rectMenuY == 22)  {NOTE(5); if(numButton1 == 2) {return;} PressedButton(0); WEATHER(); PressedButton(0); break;} //APICITY
                                    if(numButton1 == 5 && rectMenuY == 44)  {NOTE(6); if(numButton1 == 2) {return;} PressedButton(0); WEATHER(); PressedButton(0); break;} //APICOUNTRY
                                    if(numButton1 == 5 && rectMenuY == 66)  {NOTE(7); if(numButton1 == 2) {return;} PressedButton(0); WEATHER(); PressedButton(0); break;} //APIKEY                                                                
                          }//END WHILE 
                          InMenu = 0;   
}
////////////////////////////////////////////////////////////////////////////////////////////
void GET_WEATHER() { tft.fillScreen(BackColor); //Serial.begin(9600);
                     startX = 73; startY = 110; MyText = "WAIT..."; event();
 apiC =       EEPROM.readString(530);
 apiCountry = EEPROM.readString(590);
 apiKey =     EEPROM.readString(650);
 String apiW = "http://api.openweathermap.org/data/2.5/weather?q=" + apiC + "," + apiCountry + ",pt&APPID="; 
 HTTPClient http;
    http.begin(apiW + apiKey);
    int httpCode = http.GET();  
    if (httpCode > 0) {
      String payload = http.getString();
      //Serial.println(httpCode);
      //Serial.println(payload);
      handleReceivedMessage(payload);
  http.end();                                  
}
}
////////////////////////////////////////////////////////////////////////////////////////////
void handleReceivedMessage(String message) {
  StaticJsonDocument<1500> doc;   
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    //Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.c_str());
    //return;
  }
  const char* name = doc["name"];
  ////////////////////////
    if(name != "") {tft.fillScreen(BackColor);} //ERASE "WAIT..."
  ////////////////////////
    startX = 0; startY = 0; MyText = " "; event(); MyText = name; event();
    
  float tempK = doc["main"]["temp"];                   
  float tempC = tempK - 273.15;   
    startX = 0; startY = 48; MyText = "Темп-а: "; event(); MyText = tempC; event(); MyText = " C"; event();

  float tempKFeel = doc["main"]["feels_like"];                   
  float tempCFeel = tempKFeel - 273.15;   
    startX = 0; startY = 72; MyText = "По ощ-ям: "; event(); MyText = tempCFeel; event(); MyText = " C"; event();

  int humidity = doc["main"]["humidity"];
    startX = 0; startY = 96; MyText = "Влаж-ть: "; event(); MyText = humidity; event(); MyText = " %"; event();

  String weather = doc["clouds"]["all"]; 
    startX = 0; startY = 120; MyText = "Облач-ть: "; event(); MyText = weather; event(); MyText = " %"; event();;

  int pressurehPa = doc["main"]["pressure"]; 
  float pressure = pressurehPa/1.333;
    startX = 0; startY = 144; MyText = "Дав-е: "; event(); MyText = pressure; event(); MyText = " mmHc"; event();

  float windspeed = doc["wind"]["speed"]; 
    startX = 0; startY = 168; MyText = "Ветер: "; event(); MyText = windspeed; event(); MyText = " m/s"; event();
      
//  int timezone = doc["timezone"];
//    startX = 0; startY = 26; MyText = "Timezone: "; event(); MyText = timezone; event();
    
//  int winddeg = doc["wind"]["deg"]; 

   PressedButton(0);
   while(numButton1 != 10 && numButton1 != 5) {PressedButton(0); if(numButton1 == 2) {return;}}
}
////////////////////////////////////////////////////////////////////////////////////////////
void event() {
int charByteStart, charByteEnd, dobByte, strLen = MyText.length();
for (charByteStart=0; charByteStart<strLen+0; charByteStart+=dobByte) {
                        if(MyText.charAt(charByteStart) == '\320' ||
                           MyText.charAt(charByteStart) == '\321') {dobByte=2;}//+BYTE IF RUSSIAN   
                                                              else {dobByte=1;}    
                                                           charByteEnd=charByteStart+dobByte;        
substrSpecific = MyText.substring(charByteStart, charByteEnd);
//if(substrSpecific == "\40") {if(startX == 2) {continue;} startX+=14; continue;}//IF SPACE. CANCEL IF START STRING
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void RAFT() {
tft.fillScreen(BackColor);
  MyText = "Just for example"; startX = 14; startY = 0; charColo = WHITE; event();
int cooX = 108, cooY = 235, plotW = 25, gap = 7;
tft.fillRect(cooX,cooY,plotW,4,WHITE);
tft.fillRect(120,120,2,2,WHITE);

PressedButton(0);
    while(numButton1 != 10 && numButton1 != 5) { PressedButton(0); if(numButton1 == 2) {return;} 
                   if(numButton1 == 3) {if(cooX + plotW > 240) {} else {cooX += gap; //CHECK RIGHT WALL
                                        tft.fillRect(cooX-gap,cooY,gap,4,BackColor);            
                                        tft.fillRect(cooX+(plotW-gap),cooY,gap,4,WHITE);
                                                                        }
                                       }
                   if(numButton1 == 4) {if(cooX < 0) {}           else {cooX -= gap; //CHECK LEFT WALL
                                        tft.fillRect(cooX+(plotW),cooY,gap,4,BackColor); 
                                        tft.fillRect(cooX,cooY,gap,4,WHITE);
                                                                        }
                                       }                          
                                               }
            }
