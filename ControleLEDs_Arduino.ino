/* 
 *  
 *  Software Developer
 *    Alexandre Pansan Junior
 *  Software created for controlling leds exinting on the FastLED library, on the ESP8266 module.
 *  It includes an App for Android, disponible on Github, Please not sell, or distribute without asking..
 *  
 */


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include <efeitos.c>

//Definitions

//LEDs definifions
  //Leds Pins
    #define LED_PIN      01
    #define LED_PIN2     02
    #define LED_PIN3     03
    #define LED_PIN4     04
  //quantities of led on each strip
    #define NUM_LEDS     20
    #define NUM_LEDS2    20
    #define NUM_LEDS3    20
    #define NUM_LEDS4    20
  //some useless counts  
    #define HALF_NUM_LEDS NUM_LEDS/2

  //LED configurations
    #define BRIGHTNESS  50
    #define LED_TYPE    UCS1903
    #define COLOR_ORDER RGB
    #define FRAMES_PER_SECOND 100
//End of the LED definitions

//
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS2];
CRGB leds3[NUM_LEDS3];
CRGB leds4[NUM_LEDS4];

//Thread FuncLED;

String string, funcao;
int red, green,blue,redB,greenB,blueB, externa, interna, controller = 0, directions = 0;


// config WIFI
  IPAddress    apIP(42, 42, 42, 42);
  char *ssid = "ControleLEDs-Teste";
  const char *password = "ESP8266Test";

// Define a web server at port 80 for HTTP
  ESP8266WebServer server(80);

//config FastLED library
  bool gReverseDirection = false;

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;



/*
 * 
 * Begining of SETUP loop
 * 
 */
void setup() {
  
  delay( 3000 ); // power-up safety delay

  // Set the led Strip comfiguration

  
  //Set LED strip 1
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );

  //Set LED strip 2
    FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS2).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );

  //Set LED strip 3
    FastLED.addLeds<LED_TYPE, LED_PIN3, COLOR_ORDER>(leds3, NUM_LEDS3).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );

  //Set LED strip 4
    FastLED.addLeds<LED_TYPE, LED_PIN4, COLOR_ORDER>(leds4, NUM_LEDS4).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );

  //End of the Strip configuration

  //Set an Startup effect

  
  //wifi hotspot configuration
    Serial.begin(115200);
  //Serial.println();
  //Serial.println("Configuring access point...");

  //set-up the custom IP address
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00  

  //Creating Wifi SoftAP Network
    WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  server.on ( "/", handleRoot );
  server.on ( "/funcao", handleRoot);
  server.on ( "/fita", handleRoot); 
  server.on ( "/testConnect", handleRoot);
  /*server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );*/
  server.onNotFound ( handleNotFound );
  
  server.begin();
  //Serial.println("HTTP server started");

}


static uint8_t startIndex = 0;
uint8_t gHue = 0;

/*
 * 
 * Begining of LOOP loop?!?!
 *                    Vai entender.. loop inception
 * 
 */
void loop() {
  
  //wifi
  server.handleClient();

  //Serial.println(funcao);
  //GoBack("1","1",255,255,255,255,255,255);

  //Starts on 
  Fire("Teste","Teste");

  //leds
  if( funcao == "AllStrip"){ AllStrip((String) interna,(String) externa, red, green, blue);}
  if( funcao == "GoBack"){GoBack((String) interna,(String) externa, red, green, blue, redB, greenB, blueB, controller, directions); controller++;}
  if( funcao == "Fire"){ Fire((String) interna,(String) externa); }
  
}



/*
 * 
 * Wifi configuration functions
 * 
 */
 
void handleRoot() {

  
    Serial.println(server.arg("funcao"));
    Serial.println(server.arg("fita"));
    if(server.arg("testeConnect")){
      server.send ( 200, "text/plain", "i'm here" );
    }
   
    char buf[100];
    char *p = buf;
    string = server.arg("funcao");
    string.toCharArray(buf, sizeof(buf));
    funcao = strtok_r(p, ",", &p);
    red = atoi(strtok_r(p, ",", &p));
    green = atoi(strtok_r(p, ",", &p));
    blue = atoi(strtok_r(p, ",", &p));
    redB = atoi(strtok_r(p, ",", &p));
    greenB = atoi(strtok_r(p, ",", &p));
    blueB = atoi(strtok_r(p, ",", &p));
    interna = atoi(strtok_r(p, ",", &p));
    externa = atoi(strtok_r(p, ",", &p));
}



void handleNotFound() {
  digitalWrite ( D0, 0 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( D0, 1 ); //turn the built in LED on pin D0 of NodeMCU off
}



/*
 * 
 * Efeitos LED padrao... mas funcional
 * 
*/

void AllStripHalf(String Strip1, String Strip2, int r, int g, int b, int rb, int gb, int bb){
  Serial.println("Rodando");

  for(int i = 0; i < HALF_NUM_LEDS; i++){
    if(Strip1 == "1"){
     leds[i].setRGB( r, g, b);
    }else{
      leds[i].setRGB(0,0,0);
    }
    
    if(Strip2 == "1"){
     leds2[i].setRGB( r, g, b);
    }else{
      leds2[i].setRGB(0,0,0);
    }
  }
  for(int i = 25;i< NUM_LEDS2; i++){
    
    if(Strip1 == "1"){
     leds[i].setRGB(rb,gb,bb);
    }else{
      leds[i].setRGB(0,0,0);
    }
    if(Strip2 == "1"){
     leds2[i].setRGB(rb,gb,bb);
    }else{
      leds2[i].setRGB(0,0,0);
    }
     
  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}



void AllStrip(String Strip1, String Strip2, int r, int g, int b){
  CRGB rgbval(r,g,b);
  if(Strip1 == "1"){
    fill_solid(leds,NUM_LEDS,rgbval);
  }
  if(Strip2 == "1"){
    fill_solid(leds2,NUM_LEDS2,rgbval);
  }
}



void GoBack(String Strip1, String Strip2, int r, int g, int b, int rb, int gb, int bb, int i, int direction_roll){
  int numleds = NUM_LEDS + 6;
    if(direction_roll == 0){
      if(Strip1 == "1"){
        //Serial.println(i);
        leds[i].setRGB(r,g,b);
        leds[i-1].setRGB(r/2,g/2,b/2);
        leds[i-2].setRGB(r/4,g/4,b/4);
        leds[i-3].setRGB(r/8,g/8,b/8);
        leds[i-4].setRGB(r/16,g/16,b/16);
        leds[i-5].setRGB(r/32,g/32,b/32);
        leds[i-6].setRGB(0,0,0);
      }
      if(Strip2 == "1"){
        //Serial.println(i);
        leds2[i].setRGB(r,g,b);
        leds2[i-1].setRGB(r/2,g/2,b/2);
        leds2[i-2].setRGB(r/4,g/4,b/4);
        leds2[i-3].setRGB(r/8,g/8,b/8);
        leds2[i-4].setRGB(r/16,g/16,b/16);
        leds2[i-5].setRGB(r/32,g/32,b/32);
        leds2[i-6].setRGB(0,0,0);
      }
      FastLED.show();
      //Serial.print("Indo ");
      //Serial.println(i);
    }
    
    if(direction_roll == 1){
      if(Strip1 == "1"){
        //Serial.println(i);
        leds[i].setRGB(r,g,b);
        leds[i+1].setRGB(r/2,g/2,b/2);
        leds[i+2].setRGB(r/4,g/4,b/4);
        leds[i+3].setRGB(r/8,g/8,b/8);
        leds[i+4].setRGB(r/16,g/16,b/16);
        leds[i+5].setRGB(r/32,g/32,b/32);
        leds[i+6].setRGB(0,0,0);
      }
      if(Strip2 == "1"){
        //Serial.println(i);
        leds2[i].setRGB(r,g,b);
        leds2[i+1].setRGB(r/2,g/2,b/2);
        leds2[i+2].setRGB(r/4,g/4,b/4);
        leds2[i+3].setRGB(r/8,g/8,b/8);
        leds2[i+4].setRGB(r/16,g/16,b/16);
        leds2[i+5].setRGB(r/32,g/32,b/32);
        leds2[i+6].setRGB(0,0,0);
      }
      FastLED.show(); 
      //Serial.print("Vindo ");
      //Serial.println(i);
    }
    //Serial.println("passou GoBack");
}



#define COOLING  55
#define SPARKING 120
void Fire(String Strip1, String Strip2)
{
// Array of temperature readings at each simulation cell

  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      if(Strip1 == "1"){
          leds[pixelnumber] = color;
      }else{
          leds[pixelnumber].setRGB(0,0,0);
      }
      if(Strip2 == "1"){
          leds2[pixelnumber] = color;
      }else{
          leds2[pixelnumber].setRGB(0,0,0);
      }
      if(Strip1 == "Teste"){
          leds[pixelnumber] = color;
          leds2[pixelnumber] = color;
          leds3[pixelnumber] = color;
          leds4[pixelnumber] = color;
      }
    }
    FastLED.show();
    FastLED.delay(1000 / FRAMES_PER_SECOND);
}


