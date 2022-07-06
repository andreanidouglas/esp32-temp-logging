#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "<YOUR SSID HERE>";
const char* password = "<YOUR SSID PASSWORD HERE>";

String serverName = "http://<SERVER IP ADDRESS>/temp";
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;



#define DHTPIN 14

#define DHTTYPE DHT22


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wifi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  dht.begin();

  delay(2000);
}


void loop() {
  

  float h = dht.readHumidity();
  float t = dht.readTemperature(false);

  if ( isnan(h) || isnan(t) ) {
    Serial.println(F("Failed to read from DHT sensor."));
    Serial.println(h);
    Serial.println(t);
    
    return;
  }

  float hic = dht.computeHeatIndex(t, h, false);


  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("content-type", "application/json");
    http.addHeader("agent-user", "esp32-sensor");
    Serial.println("{\"temperature_c\":" + String(t,2) + ",\n\"humidity_p\":" + String(h,2) + " }\n\n");
    int httpResponse = http.POST("{\"temperature_c\":" + String(t,2) + ",\n\"humidity_p\":" + String(h,2) + " }");
    Serial.print("HTTP Response: ");
    Serial.println(httpResponse); 
    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }
  delay (5000);
}
