/**
 * @file headphone.ino
 * @author Phil Schatzmann
 * @brief Headphone Detection Demo
 * @date 2021-12-10
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include "AudioKitHAL.h"
#include "SineWaveGenerator.h"
#include "credentials.h"

#define LED1 GPIO_NUM_19
#define LED2 GPIO_NUM_22

AudioKit kit;
SineWaveGenerator wave;
const int BUFFER_SIZE = 1024;
uint8_t buffer[BUFFER_SIZE];

void setupWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(MY_SSID, MY_SSAUTH);

  ArduinoOTA.setHostname("esp32");
  ArduinoOTA
      .onStart([]()
               {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "firmware";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Received: %7d of %7d\r", progress, total); })
      .onError([](ota_error_t error)
               {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

  ArduinoOTA.begin();
}

void setup()
{
  Serial.begin(115200);

  LOGLEVEL_AUDIOKIT = AudioKitError;
  auto cfg = kit.defaultConfig(AudioOutput);
  kit.begin(cfg);

  // 1000 hz
  wave.setFrequency(1);
  wave.setSampleRate(cfg.sampleRate());
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);

  setupWiFi();
}

void loop()
{
  size_t l = wave.read(buffer, BUFFER_SIZE);
  kit.write(buffer, l);
  int16_t *ptr = (int16_t *)buffer;
  Serial.print(">signal:");
  Serial.println(*ptr);

  ArduinoOTA.handle();
  yield();
}