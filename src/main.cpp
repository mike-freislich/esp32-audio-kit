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
#include "TelnetStream.h"


AudioKit kit;
SineWaveGenerator wave;
const int BUFFER_SIZE = 1024;
uint8_t buffer[BUFFER_SIZE];
uint8_t pinGreenLed;
bool blink = true;

void setupWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin("IvoryTower", "Butterfly1977");

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

  LOGLEVEL_AUDIOKIT = AudioKitInfo;
  
  // open in write mode
  auto cfg = kit.defaultConfig(AudioOutput);

  kit.setSampleRate(AUDIO_HAL_48K_SAMPLES);
  kit.setVolume(10);
  kit.begin(cfg);

  // 1000 hz
  wave.setFrequency(1000);
  wave.setSampleRate(cfg.sampleRate());

  // pinGreenLed = kit.pinGreenLed();
  pinGreenLed = GPIO_NUM_22;
  pinMode(pinGreenLed, OUTPUT);
  digitalWrite(pinGreenLed, LOW);

  delay(1000);
  Serial.println("Sample Rate: " + cfg.sampleRate());
  Serial.println("bpS: " + cfg.bitsPerSample());

  setupWiFi();
}



void loop()
{
  size_t l = wave.read(buffer, BUFFER_SIZE);
  kit.write(buffer, l);
  delay(100);
  Serial.print(int(buffer[1]));
  // Serial.print(" ");
  /*
  blink = !blink;
  if (blink)
    digitalWrite(pinGreenLed, LOW);
  else
    digitalWrite(pinGreenLed, HIGH);
    */
  ArduinoOTA.handle();
  yield();
}