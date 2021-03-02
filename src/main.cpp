#include "M5Atom.h"
//#include <Adafruit_NeoPixel.h>
#include "FastLED.h"
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;


#define NUMPIXELS      9
#define PIN            22
CRGB leds[NUMPIXELS];

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const uint8_t Srv0 = 23, Srv1 = 33; //GPIO No.
const uint8_t srv_CH0 = 7, srv_CH1 = 6;//チャンネル
const double PWM_Hz = 50;   //PWM周波数
const uint8_t PWM_level = 16; //PWM 16bit(0～65535)


extern const unsigned char AtomImageData[375 + 2];

uint8_t DisBuff[2 + 5 * 5 * 3];

void setBuff(uint8_t Rdata, uint8_t Gdata, uint8_t Bdata)
{
    DisBuff[0] = 0x05;
    DisBuff[1] = 0x05;
    for (int i = 0; i < 25; i++)
    {
        DisBuff[2 + i * 3 + 0] = Rdata;
        DisBuff[2 + i * 3 + 1] = Gdata;
        DisBuff[2 + i * 3 + 2] = Bdata;
    }
}

void setup()
{
    M5.begin(true, false, true);

    SerialBT.begin("ESP32");
    SerialBT.println("Hello");
    SerialBT.println("nekosuki");


    delay(10);
    setBuff(0xff, 0x00, 0x00);
    M5.dis.displaybuff(DisBuff);
    pinMode(21,OUTPUT);
    FastLED.addLeds<SK6812, PIN, GRB>(leds, NUMPIXELS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(100);

    //pixels.begin(); // This initializ\es the NeoPixel library.

    pinMode(Srv0, OUTPUT);
    pinMode(Srv1, OUTPUT);
    
    //モータのPWMのチャンネル、周波数の設定
    ledcSetup(srv_CH0, PWM_Hz, PWM_level);
    ledcSetup(srv_CH1, PWM_Hz, PWM_level);
    
    //モータのピンとチャンネルの設定
    ledcAttachPin(Srv0, srv_CH0);
    ledcAttachPin(Srv1, srv_CH1);

    //ledcWrite(srv_CH0, 5000);
    //ledcWrite(srv_CH1, 4500);

}

uint8_t FSM = 0;
int led_speed=10;
float gain_r=1.0;
float gain_g=1.0;
float gain_b=1.0;
int mode=1;
void loop()
{

  for(int t=0;t<2*314;t+=led_speed){
    for(int n=0;n<NUMPIXELS;n++){
      if(mode==1){
        int r=gain_r*(110*sin(t*1.0/100.0         +1/NUMPIXELS*n)+110);
        int g=gain_g*(110*sin(t*1.0/100.0+2*3.14/3+1/NUMPIXELS*n)+110);
        int b=gain_b*(110*sin(t*1.0/100.0+4*3.14/3+1/NUMPIXELS*n)+110);
        leds[n]=CRGB(r,g,b);

      }else if(mode==2){
        int r=gain_r*225;
        int g=gain_g*225;
        int b=gain_b*225;
        leds[n]=CRGB(r,g,b);
      }

      FastLED.show();
      delay(5); // Delay for a period of time (in milliseconds)

    }
  
    if (SerialBT.available()) {
    int inByte = SerialBT.read();
    int cw=5000;
    int ccw=4500;
    switch(inByte){
      case 'a':
        ledcWrite(srv_CH0, cw);
        ledcWrite(srv_CH1, cw);
      SerialBT.println("a");

        break;
      case 'w':
        ledcWrite(srv_CH0, cw);
        ledcWrite(srv_CH1, ccw);
        SerialBT.println("w");

        break;
      case 'd':
        ledcWrite(srv_CH0, ccw);
        ledcWrite(srv_CH1, ccw);
        SerialBT.println("d");
        break;
      case 's':
        ledcWrite(srv_CH0, ccw);
        ledcWrite(srv_CH1, cw);
        SerialBT.println("s");
        break;
      case 'q':
        ledcWrite(srv_CH0, 8000);
        ledcWrite(srv_CH1, 6000);
        SerialBT.println("q");
        break;
      case 'e':
        ledcWrite(srv_CH0, 2000);
        ledcWrite(srv_CH1, 8000);
        SerialBT.println("e");
        break;
      case 'o':
        led_speed++;
        SerialBT.println(led_speed);
        break;
      case 'l':
        if(led_speed>1)led_speed--;
        SerialBT.println(led_speed);
        break;
      case 't':
        gain_r+=0.1;
        if(gain_r>=1.1)gain_r=0.0;
        SerialBT.println(gain_r);
        break;
      case 'h':
        gain_g+=0.1;
        if(gain_g>=1.1)gain_g=0.0;
        SerialBT.println(gain_g);
        break;
      case 'n':
        gain_b+=0.1;
        if(gain_b>=1.1)gain_b=0.0;
        SerialBT.println(gain_b);
        break;
      case 'r':
        gain_r=1;
        gain_g=0;
        gain_b=0;
        break;
      case 'g':
        gain_r=0;
        gain_g=1;
        gain_b=0;
        break;
      case 'b':
        gain_r=0;
        gain_b=0;
        gain_b=1;
        break;
      case 'f':
        gain_r=0;
        gain_g=0;
        gain_b=0;
        break;
      case 'v':
        gain_r=1;
        gain_g=1;
        gain_b=1;
        break;
      case 'z':
        if(mode==1){mode=2;
        }else if(mode==2){mode=1;}
        SerialBT.print("mode:");
        SerialBT.println(mode);
        break;
      default:
          ledcWrite(srv_CH0, 0);
          ledcWrite(srv_CH1, 0);
          break;
          
      }
    //  SerialBT.println(inByte);
    }
  }
//*/
  if (M5.Btn.wasPressed()){
  SerialBT.println("nekosuki");
  switch (FSM){
    case 0:
      digitalWrite(21,LOW);
      break;
    case 1:
      digitalWrite(21,LOW);
      break;
    default:
      break;
    }
        //M5.dis.displaybuff(DisBuff);

        FSM++;
        if (FSM >= 1)
        {
            FSM = 0;
        }
    }

    M5.update();
}