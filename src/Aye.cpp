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
    constexpr auto myId = "02";

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
        static const String connectedTopic = String("Aye/") + String( myId) + String("/connected");
        while ( not client.connected())
        {
            if (client.connect(clientId.c_str(), nullptr, nullptr, connectedTopic.c_str() , 0, false, "0"))
            {
                client.publish( connectedTopic.c_str(), "1");
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

