#include <IRremote.h>
#include <FastLED.h>

//für IR
#define UP 0x1039789F
#define RIGHT 0xF0B4BB43
#define DOWN 0xBB8B3E9E
#define LEFT 0x45481702
#define OK 0xB8E7B4FE
uint8_t RECV_PIN = 11;    //IR-Sensor Pin
IRrecv irrecv(RECV_PIN);
decode_results results;

//für RGB-LEDs
#define DATA_PIN 5
#define HELLIGKEIT 64
#define HOEHE 12
#define BREITE 12
#define NUM_LEDS (HOEHE*BREITE)
CRGB leds[NUM_LEDS];
uint8_t matrix[BREITE][HOEHE];

uint8_t helligkeit=HELLIGKEIT;
//--------------------------------Setup-----------------------

void setup() {
  Serial.begin(9600);
  
  //für LEDs
  
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(HELLIGKEIT);
  int8_t i;
  int8_t j;
  for(i=0;i<HOEHE;i++){
    for(j=0;j<BREITE;j++){
        matrix[i][j]=0;
    }  
  }
  FastLED.show();
  //für IR
  
  irrecv.enableIRIn(); // Start the receiver

  
}

//-----------------------------Loop------------------------------------

void loop() {
  uint8_t rueckgabe=checkIR();
  switch(rueckgabe){
    case 1: SinusFarbverlauf();break;
    case 2: Snake_einfach();break;
    case 3: Snake_schwer();break;
    case 4: TTT();break;
    case 9: Helligkeit();
    default: ;  
  }
}

//--------------------------------Unterprogramme--------------------------------------

void AusgebenMatrix(){
  int8_t i;
  int8_t j;
  for(i=0;i<HOEHE;i++){
      if(i%2==0){
        for(j=0;j<BREITE;j++){
            switch(matrix[i][j]){
              case 0: leds[i*BREITE+j]=CRGB::Black;break;
              case 1: leds[i*BREITE+j]=CRGB::Red;break;
              case 2: leds[i*BREITE+j]=CRGB::Blue;break;
              case 3: leds[i*BREITE+j]=CRGB::Yellow;break;
              case 4: leds[i*BREITE+j]=CRGB::Green;break;
              case 5: leds[i*BREITE+j]=CRGB::White;break;
              case 6: leds[i*BREITE+j]=CRGB::Orange;break;
              case 99: leds[i*BREITE+j]=CRGB::Red;break;
              case 100: leds[i*BREITE+j]=CRGB::Yellow;break;
              default: leds[i*BREITE+j]=CRGB::Orange;break;
            }
        }
      }
      else{
        for(j=BREITE-1;j>=0;j--){
            switch(matrix[i][j]){
              case 0: leds[((i+1)*BREITE)-1-j]=CRGB::Black;break;
              case 1: leds[((i+1)*BREITE)-1-j]=CRGB::Red;break;
              case 2: leds[((i+1)*BREITE)-1-j]=CRGB::Blue;break;
              case 3: leds[((i+1)*BREITE)-1-j]=CRGB::Yellow;break;
              case 4: leds[((i+1)*BREITE)-1-j]=CRGB::Green;break;
              case 5: leds[((i+1)*BREITE)-1-j]=CRGB::White;break;
              case 6: leds[((i+1)*BREITE)-1-j]=CRGB::Orange;break;
              case 99: leds[((i+1)*BREITE)-1-j]=CRGB::Red;break;
              case 100: leds[((i+1)*BREITE)-1-j]=CRGB::Yellow;break;
              default: leds[((i+1)*BREITE)-1-j]=CRGB::Orange;break;
            }
        } 
      }
  }
  FastLED.show();  
}
uint8_t checkIR(){
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    uint32_t wert=results.value;
    irrecv.resume(); 
    if (wert == 0x4ED5545A)return 1; //Rote Taste
    if (wert == 0x2B77163A)return 2; //Grüne Taste
    if (wert == 0x9774845A)return 3; //Gelbe Taste
    if (wert == 0xA084B2A2)return 4; //Blaue Taste
    if (wert == 0xCEBE3B7A)return 9; //helligkeit verändern
    if (wert == 0xFFFFFFFF)return 254; //Taste dauerhalt gedrückt
    if (wert != 0)return 255;       //Signal aufgenommen
  } 
  return 0;
}

//-----------------------------------Snake-------------------------------------------------------
void Snake_einfach(){
  int8_t y;
  int8_t x;
  for(y=0;y<HOEHE;y++){
    for(x=0;x<BREITE;x++){
        matrix[x][y]=0;
    }  
  }
  matrix[random(BREITE)][random(HOEHE/2)]=100;
  Snake(1);
}
void Snake_schwer(){
  int8_t y;
  int8_t x;
  for(y=0;y<HOEHE;y++){
    for(x=0;x<BREITE;x++){
        matrix[x][y]=0;
    }  
  }
  for(x=0;x<BREITE;x++)matrix[x][0]=4;
  for(x=0;x<BREITE;x++)matrix[x][HOEHE-1]=4;
  for(y=0;y<HOEHE;y++)matrix[0][y]=4;
  for(y=0;y<HOEHE;y++)matrix[BREITE-1][y]=4;
  matrix[random(1,BREITE-1)][random(1,HOEHE/2)]=100;
  Snake(2);
}

void Snake(uint8_t sl){ //schwierigkeitslevel übergeben (1=leicht, 2=schwer)
  int8_t spiel=1;
  int8_t laenge=1;
  int8_t rueckgabe;
  int8_t richtung=4; //1=rechts,2=links,3=up,4=down 
  s_setzeBlock(sl);
  while(spiel){
    
      rueckgabe=s_checkIR();
      if(rueckgabe!=0)richtung=rueckgabe;
      
      rueckgabe=s_bewegen(laenge, richtung);
      if(rueckgabe==2){
        laenge++;
        s_setzeBlock(sl);
      }
      else if(rueckgabe==1)spiel=0;

      AusgebenMatrix();
      
      rueckgabe=s_checkIR();
      if(rueckgabe!=0)richtung=rueckgabe;
      
      delay(300);
  }
  delay(1000);
  int8_t y;
  int8_t x;
  for(y=0;y<HOEHE;y++){
    for(x=0;x<BREITE;x++){
        matrix[x][y]=0;
    }  
  }
  GameOver();
}

//--------------------------------------------Unterprogramme Snake---------------------------------------

void s_setzeBlock(uint8_t sl){
  if(sl=1){
    uint8_t a=random(BREITE);
    uint8_t b=random(HOEHE);
    while(matrix[a][b]!=0){
      a=random(BREITE);
      b=random(HOEHE);  
    }
    matrix[a][b]=99; 
  }
  else if(sl=2){
    uint8_t a=random(1,BREITE-1);
    uint8_t b=random(1-HOEHE-1);
    while(matrix[a][b]!=0){
      a=random(1,BREITE-1);
      b=random(1,HOEHE-1);  
    }
    matrix[a][b]=99; 
  } 
}

int8_t s_bewegen(int8_t laenge, int8_t richtung){
  int8_t y;
  int8_t x;
  int8_t plus1=0;
  for(y=0;y<HOEHE;y++){
    for(x=0;x<BREITE;x++){
        if(matrix[x][y]==100+laenge)matrix[x][y]=0;
        else if(matrix[x][y]>100)matrix[x][y]+=1;
        else if(matrix[x][y]==98)matrix[x][y]=100;
        else if(matrix[x][y]==100){
            switch(richtung){
              case 1:  if(x+1<BREITE){
                        if(matrix[x+1][y]!=0&&matrix[x+1][y]!=99)return 1;
                        else{
                          if(matrix[x+1][y]==99)plus1=1;
                          matrix[x+1][y]=98;
                          matrix[x][y]=101;
                        }  
                      }
                      else if(matrix[0][y]==0||matrix[0][y]==99){
                        if(matrix[0][y]==99)plus1=1;
                        matrix[0][y]=100;
                        matrix[x][y]=101;  
                      }
                      else return 1; 
                      break;
              case 2:  if(x-1>=0){
                        if(matrix[x-1][y]!=0&&matrix[x-1][y]!=99)return 1;
                        else{ 
                          if(matrix[x-1][y]==99)plus1=true;
                          matrix[x-1][y]=100;
                          matrix[x][y]=101;     
                        }
                      }
                      else if(matrix[BREITE-1][y]==0||matrix[BREITE-1][y]==99){
                        if(matrix[BREITE-1][y]==99)plus1=1;
                        matrix[BREITE-1][y]=98;
                        matrix[x][y]=101;  
                      }
                      else return 1; 
                      break;
              case 3:  if(y-1>=0){
                        if(matrix[x][y-1]!=0&&matrix[x][y-1]!=99)return 1;
                        else{
                          if(matrix[x][y-1]==99)plus1=true;
                          matrix[x][y-1]=100;
                          matrix[x][y]=101;  
                        } 
                      }
                      else if(matrix[x][HOEHE-1]==0||matrix[x][HOEHE-1]==99){
                        if(matrix[x][HOEHE-1]==99)plus1=1;
                        matrix[x][HOEHE-1]=98;
                        matrix[x][y]=101;  
                      }
                      else return 1; 
                      break;
             case 4:  if(y+1<HOEHE){
                        if(matrix[x][y+1]!=0&&matrix[x][y+1]!=99)return 1;
                        else{ 
                          if(matrix[x][y+1]==99)plus1=true;
                          matrix[x][y+1]=98;
                          matrix[x][y]=101;  
                          
                        }  
                      }
                      else if(matrix[x][0]==0||matrix[x][0]==99){
                        if(matrix[x][0]==99)plus1=1;
                        matrix[x][0]=100;
                        matrix[x][y]=101;  
                      }
                      else return 1; 
                      break;
              default: return 1;
            }
        }
    }  
  }
  if(plus1)return 2;
  return 0;
}
uint8_t s_checkIR(){
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    uint32_t wert=results.value;
    irrecv.resume(); 
    if (wert == UP)return 3;
    if (wert == RIGHT)return 1;
    if (wert == DOWN)return 4;
    if (wert == LEFT)return 2;
  } 
  return 0;
}

//-------------------------Sinus-Farbverlauf------------------------------
void SinusFarbverlauf()
{
    int8_t count=0;
    while(1){
      uint32_t ms = millis();
      int32_t yHueDelta32 = ((int32_t)cos16( ms * 27 ) * (350 / BREITE));
      int32_t xHueDelta32 = ((int32_t)cos16( ms * 39 ) * (310 / HOEHE));
      DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
      FastLED.show();

      if(count>20){
        uint8_t rueckgabe=checkIR();
        if(rueckgabe!=0&&rueckgabe!=254)break;
      }
      else count++;
    }
}
 
void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for( byte y = 0; y < HOEHE; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;      
    for( byte x = 0; x < BREITE; x++) {
      pixelHue += xHueDelta8;
      leds[ XY(x, y)]  = CHSV( pixelHue, 255, 255);
    }
  }
}
// Helper function that translates from x, y into an index into the LED array
// Handles both 'row order' and 'serpentine' pixel layouts.
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (BREITE - 1) - x;
      i = (y * BREITE) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * BREITE) + x;
    }
 
  return i;
}
//---------------------------TicTacToe--------------------------------------
void TTT(){
  uint8_t rueckgabe;   //für s_checkIR()
  int8_t c_wert=0;      //was ist unterhalb vom Cursor?
  int8_t c_position=4;  //von 0-8 von oben links nach rechts unten 
  int8_t y;
  int8_t x;
  int8_t game=1;
  int8_t spieler=0; //toggle 0&1
  int8_t farbe;
  for(y=0;y<HOEHE;y++){
    for(x=0;x<BREITE;x++){
        matrix[x][y]=0;
    }  
  }
  for(x=0;x<BREITE-1;x++){
    matrix[x][3]=4;
    matrix[x][7]=4;
  }
  for(y=0;y<HOEHE-1;y++){
    matrix[3][y]=4;
    matrix[7][y]=4;
  }
  matrix[5][5]=2;
  AusgebenMatrix();
  
  while(game){
    if(spieler==1)farbe=1;
    else farbe=6;
    for(x=0;x<BREITE;x++)matrix[x][HOEHE-1]=farbe;
    for(y=0;y<HOEHE;y++)matrix[BREITE-1][y]=farbe;
    rueckgabe=t_checkIR();
    switch(rueckgabe){
      case 1: if(c_position%3!=2){
                loesche_c(c_position,c_wert);
                c_position++;
                c_wert=setze_c(c_position);
             }break;
      case 2: if(c_position%3!=0){
                loesche_c(c_position,c_wert);
                c_position--;
                c_wert=setze_c(c_position);
             }break;
      case 4: if(c_position<=5){
                loesche_c(c_position,c_wert);
                c_position+=3;
                c_wert=setze_c(c_position);
             }break;
      case 3: if(c_position>=3){
                loesche_c(c_position,c_wert);
                c_position-=3;
                c_wert=setze_c(c_position);
             }break;
      case 5: rueckgabe=setzen(spieler, c_position);
              if(rueckgabe>0)spieler=(spieler+1)&1;
              if(rueckgabe==2)c_wert=1;break;
      default:;
    }
    AusgebenMatrix();
    
    rueckgabe=checkGewonnenT();
    if(rueckgabe){
      if(rueckgabe==1)Gewonnen(6);
      if(rueckgabe==2)Gewonnen(1);
      break;
    }
    if(checkUnentschiedenT()){
      for(y=0;y<HOEHE;y++){
        for(x=0;x<BREITE;x++){
          matrix[x][y]=0;
        }  
      }
      delay(150);
      GameOver();
      break;
    }
    delay(150);
  }
  
}

//----------------------------------TicTacToe Unterprogramme--------------------------------------
void loesche_c(int8_t c_position, int8_t c_wert){
    switch(c_position){
      case 0: matrix[1][1]=c_wert;break;  
      case 1: matrix[5][1]=c_wert;break;  
      case 2: matrix[9][1]=c_wert;break;  
      case 3: matrix[1][5]=c_wert;break;  
      case 4: matrix[5][5]=c_wert;break;  
      case 5: matrix[9][5]=c_wert;break;  
      case 6: matrix[1][9]=c_wert;break;  
      case 7: matrix[5][9]=c_wert;break;  
      case 8: matrix[9][9]=c_wert;break;  
      default:;
    }
}
int8_t setze_c(int8_t c_position){
    int8_t c_wert;
    switch(c_position){
      case 0: c_wert=matrix[1][1];
              matrix[1][1]=2;break;  
      case 1: c_wert=matrix[5][1];
              matrix[5][1]=2;break; 
      case 2: c_wert=matrix[9][1];
              matrix[9][1]=2;break;  
      case 3: c_wert=matrix[1][5];
              matrix[1][5]=2;break;  
      case 4: c_wert=matrix[5][5];
              matrix[5][5]=2;break;  
      case 5: c_wert=matrix[9][5];
              matrix[9][5]=2;break;  
      case 6: c_wert=matrix[1][9];
              matrix[1][9]=2;break;  
      case 7: c_wert=matrix[5][9];
              matrix[5][9]=2;break;  
      case 8: c_wert=matrix[9][9];
              matrix[9][9]=2;break;
      default:;  
    }
    return c_wert;
}
int8_t setzen(int8_t spieler,int8_t c_position){  //gibt 1 zurück wenn erfolgreich
  int8_t x=(c_position%3)*4;
  int8_t y=(c_position/3)*4;
    if(matrix[x][y]==0){
      if(spieler==0){
          matrix[x][y]=6;
          matrix[x+1][y]=6;
          matrix[x+2][y]=6;
          matrix[x][y+1]=6;
          matrix[x+2][y+1]=6;
          matrix[x][y+2]=6;
          matrix[x+1][y+2]=6;
          matrix[x+2][y+2]=6;
          return 1;
      }
      else{
        matrix[x][y]=1;
        matrix[x+2][y]=1;
        matrix[x][y+2]=1;
        matrix[x+2][y+2]=1;
        return 2;
      }
    }
    return 0;
}
uint8_t checkGewonnenT(){
 
  if(matrix[0][0]==6&&matrix[4][0]==6&&matrix[8][0]==6)return 1;
  if(matrix[0][4]==6&&matrix[4][4]==6&&matrix[8][4]==6)return 1;
  if(matrix[0][8]==6&&matrix[4][8]==6&&matrix[8][8]==6)return 1;
  if(matrix[0][0]==6&&matrix[0][4]==6&&matrix[0][8]==6)return 1;
  if(matrix[4][0]==6&&matrix[4][4]==6&&matrix[4][8]==6)return 1;
  if(matrix[8][0]==6&&matrix[8][4]==6&&matrix[8][8]==6)return 1;
  if(matrix[0][0]==6&&matrix[4][4]==6&&matrix[8][8]==6)return 1;
  if(matrix[8][0]==6&&matrix[4][4]==6&&matrix[0][8]==6)return 1;
  
  if(matrix[0][0]==1&&matrix[4][0]==1&&matrix[8][0]==1)return 2;
  if(matrix[0][4]==1&&matrix[4][4]==1&&matrix[8][4]==1)return 2;
  if(matrix[0][8]==1&&matrix[4][8]==1&&matrix[8][8]==1)return 2;
  if(matrix[0][0]==1&&matrix[0][4]==1&&matrix[0][8]==1)return 2;
  if(matrix[4][0]==1&&matrix[4][4]==1&&matrix[4][8]==1)return 2;
  if(matrix[8][0]==1&&matrix[8][4]==1&&matrix[8][8]==1)return 2;
  if(matrix[0][0]==1&&matrix[4][4]==1&&matrix[8][8]==1)return 2;
  if(matrix[8][0]==1&&matrix[4][4]==1&&matrix[0][8]==1)return 2;
  else return 0;
}
int8_t checkUnentschiedenT(){
    int8_t n;                         
    int8_t wert=0;
    for(n=0;n<=8;n++){
      int8_t x=(wert%3)*4;
      int8_t y=(wert/3)*4;
      if(matrix[x][y]!=0)wert++;
    }
    if(wert==9)return 1;
    else return 0;
}
uint8_t t_checkIR(){
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    uint32_t wert=results.value;
    irrecv.resume(); 
    if (wert == UP)return 1;
    if (wert == RIGHT)return 2;
    if (wert == DOWN)return 4;
    if (wert == LEFT)return 3;
    if (wert == OK)return 5;
  } 
  return 0;
}

//----------------------Helligkeit---------------------------------------
void Helligkeit(){
  int8_t i;
  int8_t j;
  switch(helligkeit){
    case 32: helligkeit=64;break;
    case 64: helligkeit=128;break;
    case 128: helligkeit=255;break;
    case 255: helligkeit=32;break;
    default: helligkeit=HELLIGKEIT;  
  }
  FastLED.setBrightness(helligkeit);
  for(i=2;i<BREITE-2;i++){
    for(j=2;j<HOEHE-2;j++){
      matrix[i][j]=5;
    }
  }
  AusgebenMatrix();
}

//---------------------------------GameOver------------------------------
void GameOver(){
  matrix[0][1]=6;
  matrix[0][2]=6;
  matrix[0][6]=6;
  matrix[0][8]=6;
  matrix[0][4]=1;
  matrix[0][9]=1;
  matrix[0][10]=1;
  matrix[0][11]=1;
  
  matrix[1][0]=6;
  matrix[1][3]=1;
  matrix[1][5]=1;
  matrix[1][6]=6;
  matrix[1][7]=6;
  matrix[1][8]=6;
  matrix[1][9]=1;
  
  matrix[2][0]=6;
  matrix[2][3]=1;
  matrix[2][4]=1;
  matrix[2][5]=1;
  matrix[2][6]=6;
  matrix[2][8]=6;
  matrix[2][9]=1;
  matrix[2][10]=1;
  matrix[2][11]=1;
  
  matrix[3][0]=6;
  matrix[3][2]=6;
  matrix[3][3]=1;
  matrix[3][5]=1;
  matrix[3][6]=6;
  matrix[3][8]=6;
  matrix[3][9]=1;
  
  matrix[4][0]=6;
  matrix[4][1]=6;
  matrix[4][2]=6;
  matrix[4][3]=1;
  matrix[4][5]=1;
  matrix[4][6]=6;
  matrix[4][8]=6;
  matrix[4][9]=1;
  matrix[4][10]=1;
  matrix[4][11]=1;


  matrix[6][1]=1;
  matrix[6][3]=6;
  matrix[6][5]=6;
  matrix[6][6]=1;
  matrix[6][7]=1;
  matrix[6][8]=1;
  matrix[6][9]=6;
  matrix[6][10]=6;

  matrix[7][0]=1;
  matrix[7][2]=1;
  matrix[7][3]=6;
  matrix[7][5]=6;
  matrix[7][6]=1;
  matrix[7][9]=6;
  matrix[7][11]=6;

  matrix[8][0]=1;
  matrix[8][2]=1;
  matrix[8][3]=6;
  matrix[8][5]=6;
  matrix[8][6]=1;
  matrix[8][7]=1;
  matrix[8][8]=1;
  matrix[8][9]=6;
  matrix[8][10]=6;

  matrix[9][0]=1;
  matrix[9][2]=1;
  matrix[9][4]=6;
  matrix[9][6]=1;
  matrix[9][9]=6;
  matrix[9][11]=6;

  matrix[10][1]=1;
  matrix[10][4]=6;
  matrix[10][6]=1;
  matrix[10][7]=1;
  matrix[10][8]=1;
  matrix[10][9]=6;
  matrix[10][11]=6;
  
  AusgebenMatrix();
}
void Gewonnen(int8_t farbe){
  int n;
  for(n=0;n<(BREITE*HOEHE);n++){
    matrix[n%12][n/12]=farbe;
    AusgebenMatrix();
  }
}
