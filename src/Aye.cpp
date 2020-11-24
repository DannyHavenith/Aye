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

    void reconnect() {

        // Loop until we're reconnected
        // Create a random client ID
        client.setServer(mqttServer, mqttPort);
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        while ( not client.connected())
        {
            if (client.connect(clientId.c_str()))
            {
                digitalWrite( LED_BUILTIN, LOW);
                client.publish("Aye/version", "1.0");
            }
            else
            {
                digitalWrite( LED_BUILTIN, not digitalRead( LED_BUILTIN));
                delay(5000);
            }
        }
    }
}
void setup()
{
    pinMode( LED_BUILTIN, OUTPUT);
    pinMode( pirIn, INPUT);

    connectToAccessPoint();
    reconnect();
}

void loop()
{
    static auto previousPirState = not digitalRead( pirIn);
    static const String topic = String{"Aye/"} + String( myId) + String("/onOff");

    auto pirState = digitalRead( pirIn);
    if (pirState != previousPirState)
    {
        previousPirState = pirState;
        client.publish( topic.c_str(), previousPirState?"1":"0");
    }
}

