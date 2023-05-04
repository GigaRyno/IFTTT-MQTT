#pragma once

#include <string>

using namespace std;

// WiFi credentials
const char * SSID = "HOME-E9E2";
const char * PASSWORD = "blank";

// MQTT settings
const string ID = "ryan";
const string BROKER = "broker.hivemq.com";
const string CLIENT_NAME = ID + "remote_info";

const string CLIENT_TELEMETRY_TOPIC =  "champ_iot/ryan";