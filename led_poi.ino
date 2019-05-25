#include <IRremote.h>
#include "FastLED.h"

//Button Codes - Modify to match your remote control

#define BTN_REPEAT 0xFFFFFFFF

#define BTN_ON 0xFFA25D
#define BTN_OFF 0xFFE21D

#define BTN_FASTER 0xFF02FD
#define BTN_SLOWER 0xFFC23D
#define BTN_BRIGHTER 0xFFA857
#define BTN_DARKER 0xFF906F

#define BTN_FLASH 0xFF22DD
#define BTN_CONSTANT 0xFFE01F
#define BTN_STROBE 0xFF629D

#define BTN_RAINBOW 0xFF30CF
#define BTN_FIRE 0xFF6897
#define BTN_WATER 0xFFB04F
#define BTN_FOREST 0xFF9867



//Modify these or wire like in diagram
#define IRPIN 11
#define LEDDAT 6
#define LEDCLK 5
#define NUMLED 10




//keep all these as is
#define MODEconst   0
#define MODEstrobe  1
#define MODEflash   2
#define StepSize    51 // 255/5

int mode = MODEconst;
uint8_t brightness = 3*StepSize;   // step 2 of 4 ... 51 per step, 255 max, 51 min
uint8_t currentStep = 0;
bool isOn = false;
int FPS = 30;
unsigned int currentSpeed = 40;
unsigned long lastButton = 0xFFFFFF;
CRGBPalette16 currentPalette = RainbowColors_p;
CRGB leds[NUMLED];
IRrecv irrecv(IRPIN);
decode_results results;

void setup() {
  Serial.begin(9600);
      Serial.print("go");
  //Setup our LED strip
  FastLED.addLeds<APA102, LEDDAT, LEDCLK>(leds, NUMLED).setCorrection( TypicalLEDStrip );
  // Start the receiver
  irrecv.enableIRIn(); 

}

void loop() {
  //if we get signal from remote 
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX); 
   
    //Serial.println(results.value, HEX); 
    buttonMapping(results.value); //handle signal
    irrecv.resume(); // Receive the next value
  }
  animate();
}

void animate(){
  if(isOn == false){
    showColor(CRGB::Black, 0);
  }else{
    showColor(getColor(), getBrightness());
    currentStep++;
  }
  delayMicroseconds(1000/FPS);
}

CRGB getColor(){
  return ColorFromPalette( currentPalette, currentStep, 255, LINEARBLEND);
}
int getBrightness(){
  switch(mode){
    default:
    case MODEconst:
      return 255;
    break;
    case MODEstrobe:
      return 255*((currentStep / 32) % 2);
    break;
    case MODEflash: 
      return cubicwave8((uint8_t)currentStep*5);
    break;
  }
}

void showColor(CRGB color, int tBrightness){
  FastLED.setBrightness( (int)floor(tBrightness*((float)brightness/255)) );
  for(int i=0; i<NUMLED; i++){
    leds[i] = color;
  }
  FastLED.show();
}


void buttonMapping(unsigned long button){
  if(button != BTN_REPEAT){      //Handle holding down remote button
    lastButton = button;   
  }
  else{
    buttonMapping(lastButton);
  }

  
  switch(button){                 //add leading 0x to all remote-codes
    //ON-Button
    case BTN_ON:          //0xFFC03F:  
      isOn = true;              
    break;
    //OFF-Button
    case BTN_OFF:          //0xFF40BF:
      isOn = false;
    break;

//Brightness
    //Brigter
    case BTN_BRIGHTER:        //0xFF00FF:
    if(brightness < 255) brightness+=StepSize;
    Serial.print(brightness);
    break;
    //Darker
    case BTN_DARKER:        //0xFF807F:
      if(brightness > StepSize) brightness-=StepSize;
      Serial.print(brightness);
    break;

//Animation Speed
    //Faster
    case BTN_FASTER:      //0xFFC837:
      FPS+=10;
    break;
    //Slower
    case BTN_SLOWER:      //0xFFE817:
    if(FPS > 10){
      FPS-=10;
       }
    break;
      

//Brightness Transformation

    //Flash
    case BTN_FLASH:    
      mode = MODEflash;
    break;

    //Constant
    case BTN_CONSTANT:   
      mode = MODEconst;
    break;

    //Strobe
    case BTN_STROBE:
      mode = MODEstrobe;
    break;

//Color Transformation

    //Rainbow
    case BTN_RAINBOW:  
    currentPalette = RainbowColors_p;
    break;
    
    //Fire
    case BTN_FIRE:     
    currentPalette =HeatColors_p;
    break;

    //Water
    case BTN_WATER:     
    currentPalette = OceanColors_p;
    break;

    //Forest
    case BTN_FOREST:   
    currentPalette = ForestColors_p;
    break;

    default:
    break;

    
  }


  

  
}
