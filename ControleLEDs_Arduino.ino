#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>


//Definitions

#define LED_PIN     02
#define LED_PIN2     04
#define NUM_LEDS    50
#define NUM_LEDS2    50
#define HALF_NUM_LEDS NUM_LEDS/2
#define BRIGHTNESS  50
#define LED_TYPE    UCS1903
#define COLOR_ORDER RGB
#define FRAMES_PER_SECOND 100
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];

String string, funcao;
int red, green,blue,redB,greenB,blueB, externa, interna;


// config WIFI
IPAddress    apIP(42, 42, 42, 42);

const char *ssid = "ESP8266";
const char *password = "ESP8266Test";

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

//config FastLED library
bool gReverseDirection = false;


CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
  
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  //Set LED strip 2
  FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS2).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

    //wifi
  
  Serial.begin(115200);
  //Serial.println();
  //Serial.println("Configuring access point...");

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  server.on ( "/", handleRoot );
  server.on ( "/funcao", handleRoot);
  server.on ( "/fita", handleRoot); 
  //server.on ( "/main", handleRoot);
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
void loop() {



  
  //wifi
  server.handleClient();

  //leds
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}
//================================================================================


//Wifi Funcions

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
  
  /*
   * 
   * Debugs arduinenses
   * 
  Serial.println("===========Dados Separados===========");
  Serial.println("Cor principal | Cor secundaria");
  Serial.print(red);
  Serial.print(",");
  Serial.print(green);
  Serial.print(",");
  Serial.print(blue);
  Serial.print("|");
  Serial.print(redB);
  Serial.print(",");
  Serial.print(greenB);
  Serial.print(",");
  Serial.println(blueB);
  Serial.println("Fita");
  Serial.print(interna);
  Serial.print("|");
  Serial.println(externa);
  
  */
  
  
  
        //seleciona de acordo com o que foi enviado pelo cliente

        if( funcao == "AllStrip"){ AllStrip((String) interna,(String) externa, red, green, blue, redB, greenB, blueB); }
        if( funcao == "Fire"){ Fire((String) interna,(String) externa); }
        
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

//LED functions

void AllStrip(String Strip1, String Strip2, int r, int g, int b, int rb, int gb, int bb){

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
  for(int i = 25;i< NUM_LEDS; i++){
    
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
    }
}

