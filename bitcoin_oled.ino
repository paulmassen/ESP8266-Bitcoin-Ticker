
/*
The MIT License (MIT)

Copyright (c) 2014 - 2016 Jakob Flierl <jakob.flierl at gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <SFE_MicroOLED.h>
#include <ArduinoJson.h>

const char* ssid = "YOURSSID";
const char* pass = "YOURPASSWORD";
#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D
String id;
String value;
String json;
//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////

MicroOLED oled(PIN_RESET, DC_JUMPER);  // I2C Example


WiFiClient client;

// String server = "api.coindesk.com";

// delay between updates, in msec
const unsigned long postingInterval = 60L * 1000L;

// last time you connected to the server, in msec
unsigned long lastConnectionTime = 0;



void setup() {
  delay(100);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to ");


  oled.begin();
  oled.clear(ALL);
  oled.setCursor(0,0);
  oled.display();
  oled.clear(PAGE); // Clear the display's internal memory
  oled.clear(ALL);  // Clear the library's display buffer
 
  oled.print("CoinDesk Bitcoin");
  oled.setCursor(0,1);
  oled.print("loading..");
   oled.display();   // Display what's in the buffer (splashscreen)


  Serial.begin(115200);
  delay(50);

}

int check_connect = 0;

void httpRequest() {
   client.stop();

  // if there's a successful connection:
  if (client.connect("api.coindesk.com", 80)) {
    Serial.println("connecting...");
    client.println("GET /v1/bpi/currentprice.json HTTP/1.1");
    client.println("Host: api.coindesk.com");
    client.println("User-Agent: ESP8266/1.1");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

void loop() {
  int cnt;
  
  if (cnt++ == 10000) {
    cnt = 0;
    if (check_connect++ == 50) {
      check_connect = 0;
      if (WiFi.status() != WL_CONNECTED) {
      
      }
    }
  }
  
  /*
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }*/

  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
    unsigned int i = 0; //timeout counter
    int n = 1; // char counter
    char json[500] ="{";
  
   while (!client.find("\"EUR\":{")){}

    while (i<20000) {
      if (client.available()) {
        char c = client.read();
        json[n]=c;
        if (c=='}') break;
        n++;
        i=0;
      }
      i++;
    }
  
    // Serial.println(json);
    
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);

    String newjson = root["code"];
    String value = root["rate"];
    id = newjson.substring(9,12);
    
    // value = newjson.substring(41,51);
    oled.display();
  oled.clear(PAGE); // Clear the display's internal memory
  oled.clear(ALL);  // Clear the library's display buffer
 
    Serial.print("id =");

    Serial.print(id);
    
    Serial.println("value");
    Serial.println(value);
    oled.setCursor(0,1);
    oled.print(value);

    oled.display();

    id="";
    value="";
  }
}
