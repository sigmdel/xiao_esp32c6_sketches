// Main module of wifi_blackhole
// Copyright: see notice in wifi_blackhole.ino

#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"

unsigned long connect_time = 0;
unsigned long etime = 0;
unsigned long itime = 0;

void setup() {
  Serial.begin();
  delay(2000);
  Serial.println("\nAttempting to connect to the Wi-Fi network");
  WiFi.mode(WIFI_STA);
  connect_time = millis();
  WiFi.begin(ssid, password);
  while (!WiFi.isConnected()) {
    // do nothing;
  }  
  etime = millis() - connect_time;
  while ((uint32_t) WiFi.localIP() == (uint32_t) 0) {
    // do nothing
  };
  itime = millis() - connect_time;
  Serial.print("\nWiFi is connected with IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("Time to connect: %lu ms\n", etime);
  Serial.printf("Time to valid IP local address: %lu ms\n", itime);
  Serial.printf("Difference: %lu ms\n", itime - etime);
}

void loop() {
  // do nothing
}
