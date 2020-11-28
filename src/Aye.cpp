//
//  Copyright (C) 2020 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#include "WiFiConfiguration.hpp"

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <ArduinoOTA.h>

namespace {

    constexpr auto pirIn = D1;
    constexpr auto myId = "01";

    WiFiClient espClient;
    PubSubClient client(espClient);

    void connectToAccessPoint()
    {
        WiFi.mode(WIFI_STA);
        // Connect WiFi
        WiFi.hostname( myName);
        WiFi.begin( networkSID, networkPassword);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            digitalWrite( LED_BUILTIN, not digitalRead( LED_BUILTIN));
        }
        digitalWrite( LED_BUILTIN, LOW);
    }

    void reconnect()
    {

        if (WiFi.status() != WL_CONNECTED)
        {
            connectToAccessPoint();
        }

        // Loop until we're reconnected
        // Create a random client ID
        client.setServer(mqttServer, mqttPort);
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        while ( not client.connected())
        {
            if (client.connect(clientId.c_str(), nullptr, nullptr, "Aye/01/connected", 0, false, "0"))
            {
                client.publish("Aye/01/connected", "1");
            }
            else
            {
                digitalWrite( LED_BUILTIN, not digitalRead( LED_BUILTIN));
                delay(5000);
            }
        }
    }

    void setupOTA()
    {
        ArduinoOTA.setHostname( myName);
        ArduinoOTA.onStart( []()
        {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
            {
                type = "sketch";
            }
            else
            { // U_SPIFFS
                    type = "filesystem";
            }

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
        });

        ArduinoOTA.onEnd( []()
        {
            Serial.println("\nEnd");
        });

        ArduinoOTA.onProgress( [](unsigned int progress, unsigned int total)
        {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });

        ArduinoOTA.onError( [](ota_error_t error)
        {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
            {
                Serial.println("Auth Failed");
            }
            else if (error == OTA_BEGIN_ERROR)
            {
                Serial.println("Begin Failed");
            }
            else if (error == OTA_CONNECT_ERROR)
            {
                Serial.println("Connect Failed");
            }
            else if (error == OTA_RECEIVE_ERROR)
            {
                Serial.println("Receive Failed");
            }
            else if (error == OTA_END_ERROR)
            {
                Serial.println("End Failed");
            }
        });

        ArduinoOTA.begin();
    }
}
void setup()
{
    pinMode( LED_BUILTIN, OUTPUT);
    pinMode( pirIn, INPUT);

    connectToAccessPoint();
    reconnect();
    setupOTA();
    digitalWrite( LED_BUILTIN, HIGH); // switch off LED.
    delay( 10000); // wait 10s for the PIR to settle
    digitalWrite( LED_BUILTIN, HIGH); // switch off LED.
}

void loop()
{
    static auto previousPirState = not digitalRead( pirIn);
    static const String topic = String{"Aye/"} + String( myId) + String("/onOff");

    auto pirState = digitalRead( pirIn);
    if (pirState != previousPirState)
    {
        previousPirState = pirState;
        while (not client.publish( topic.c_str(), pirState?"1":"0"))
        {
            delay(500);
            reconnect();
            digitalWrite( LED_BUILTIN, not digitalRead( LED_BUILTIN));
        }
        digitalWrite( LED_BUILTIN, HIGH);
    }
    client.loop();
    ArduinoOTA.handle();
}

