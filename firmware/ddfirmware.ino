#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebSrv.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "Preferences.h"
#include <U8g2lib.h>
#include <typeinfo>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


U8G2_ST7567_ENH_DG128064I_F_HW_I2C u8g2(U8G2_R0, 22, 21, U8X8_PIN_NONE); 
AsyncWebServer server(5000);
IPAddress local_ip(192,168,0,24);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

const char* serverName = "http://192.168.0.24:5000/sources";
String ssid = "blank";
String password = "blank";
String sources = "000";
u8g2_uint_t offset;			// current offset for the scrolling text
u8g2_uint_t width;
String text = "Bienvenido";

Preferences preferences;
String sourcesData;

String httpGETRequest(const char*);
void notFound(AsyncWebServerRequest *request)

{
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}
void setup()
{
  u8g2.setI2CAddress(0x3F * 2);
  u8g2.begin();  
  
  u8g2.setFont(u8g2_font_logisoso58_tr);	// set the target font to calculate the pixel width
  width = u8g2.getUTF8Width(text.c_str());		// calculate the pixel width of the text
  
  u8g2.setFontMode(0);		// enable transparent mode, which is faster
  preferences.begin("settings", false);
  String name = preferences.getString("ssid", "");
  String pwd = preferences.getString("pwd", "");
  String src = preferences.getString("src", "");
  if (name == "")
  {
    Serial.begin(115200);

    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/config", [](AsyncWebServerRequest *request, JsonVariant &json) {
      StaticJsonDocument<200> data;
      if (json.is<JsonArray>())
      {
        data = json.as<JsonArray>();
      }
      else if (json.is<JsonObject>())
      {
        data = json.as<JsonObject>();
      }
      String response;
      serializeJson(data, response);
      request->send(200, "application/json", response);
      Serial.println(response);

      auto SSID = data["networkSSID"].as<const char*>();
      auto passkey = data["networkPassword"].as<const char*>();
      auto Sources = data["sources"].as<const char*>();
      
      Serial.println(SSID);
      preferences.putString("ssid", SSID);
      Serial.println(passkey);
      preferences.putString("pwd", passkey);
      Serial.println(Sources);
      preferences.putString("src", Sources);

      Serial.println("Received credentials, rebooting");
      delay(3000);

      ESP.restart();

      
    });
    server.addHandler(handler);
    server.onNotFound(notFound);
    server.begin();
  }
  else {
    Serial.begin(115200);
    
    ssid = name;
    password = pwd;
    sources = src;

    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
  }
}void loop()
{
  u8g2_uint_t x;
  
  u8g2.firstPage();

  do {
  
    // draw the scrolling text at current offset
    x = offset;
    u8g2.setFont(u8g2_font_logisoso58_tr);		// set the target font
    do {								// repeated drawing of the scrolling text...
      u8g2.drawUTF8(x, 63, text.c_str());			// draw the scolling text
      x += width;						// add the pixel width of the scrolling text
    } while( x < u8g2.getDisplayWidth() );		// draw again until the complete display is filled
    
    // u8g2.setFont(u8g2_font_inb16_mr);		// draw the current pixel width
    // u8g2.setCursor(0, 58);
    // u8g2.print(width);					// this value must be lesser than 128 unless U8G2_16BIT is set
    
  } while ( u8g2.nextPage() );
  
  offset-=3;							// scroll by one pixel
  if ( (u8g2_uint_t)offset < (u8g2_uint_t)-width )
    if(WiFi.status()== WL_CONNECTED){
    sourcesData = httpGETRequest(serverName);
    Serial.println(sourcesData);
    StaticJsonDocument<400> rates;
    DeserializationError error = deserializeJson(rates, sourcesData);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    String rate1 = rates[0]["tasa"];
    String rate2 = rates[1]["tasa"];
    String rate3 = rates[2]["tasa"];

    String name1 = rates[0]["name"];
    String name2 = rates[1]["name"];
    String name3 = rates[2]["name"];

    String names [3] = {name1,name2,name3};
    String tasas [3] = {rate1,rate2,rate3};

    text = "";
    Serial.println(sources);
    Serial.println(names[0]);
    Serial.println(tasas[0]);
    Serial.println(sources.c_str()[0]);

    for (int i = 0; i < 3; i++) {
      if (sources.c_str()[i]=='1') {
        Serial.println("vuelta");
        Serial.println(text);
        text = text + names[i] + ":" + tasas[i] + " ";
      }
    }  
    Serial.println(text);
    Serial.println(names[1]);
    width = u8g2.getUTF8Width(text.c_str());		// recalculate the pixel width of the text
    offset = 0;							// start over again
    
  delay(10);							// do some small delay

  

    

    delay(5000);
  }
}
String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // If you need Node-RED/server authentication, insert user and password below
  //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}