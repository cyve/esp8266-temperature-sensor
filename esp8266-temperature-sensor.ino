#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include "config.h"

ESP8266WebServer server(80);
DHT dht(2, DHT11);

void handleHttpRequest() {
  digitalWrite(LED_BUILTIN, HIGH);
  int statusCode = 200;
  String content = "{";

  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
      Serial.println("Failed to read temperature from DHT sensor.");
      statusCode = 503;
      content += "\"status\":\"error\"";
      content += ",\"message\":\"Failed to read from DHT sensor!\"";
  } else {
      Serial.printf("T: %s\n", temperature);
      content += "\"status\":\"ok\"";
      content += ",\"temperature\":";
      content += (float) temperature;
  }

  content += ",\"id\":";
  content += ESP.getChipId();
  content += "}";
  
  server.send(statusCode, "application/json", content);
  digitalWrite(LED_BUILTIN, LOW);
}

void handleNotFound() {
  digitalWrite(LED_BUILTIN, HIGH);
  server.send(404, "text/html", "<html><body><h1>Not Found</h1></body></html>");
  digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, INPUT);
  dht.begin();
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFI with IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleHttpRequest);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
