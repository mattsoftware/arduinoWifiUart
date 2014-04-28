/*
The MIT License (MIT)

Copyright (c) 2014 Matt Paine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <WifiUart.h>
#include <WifiUartMisc.h>
#include <WifiUartNetwork.h>
#include <SoftwareSerial.h>

#define LED 13
#define WIFI_RX 10
#define WIFI_TX 11
#define RESET_PIN 12

// userOne() uses these bits of information to setup the wifi card to use wpa
// encryption, using the ssid and password below, setting up dns.
#define SSID "YOUR_SSID_HERE"
#define SECURITY_PASSWORD "YOUR_WPA_PASSWORD_HERE"

#define TOGGLE_INPUT '^'
#define HELP 'h'
#define ENTER_COMMAND 'c'
#define EXIT_COMMAND 'x'
#define FACTORY_RESET 'F'
#define RESET 'r'
#define HARDWARE_RESET 'H'
#define NETWORK_STATE 'n'
#define NETWORK_SETTINGS 'N'
#define WEB_STATE 'w'
#define WEB_TOGGLE 'W'
#define USER1 '1'
#define USER2 '2'
#define USER3 '3'

SoftwareSerial wifiSerial(WIFI_RX, WIFI_TX); // RX TX
WifiUart wifi = WifiUart(&wifiSerial);
bool writeToWifi = false;


void userOne() {
  // set module to connect to home wifi
  if (!WifiUartNetwork::setWirelessMode(&wifi, WIFIU_NET_MODE_INFRASTRUCTURE)) {
    Serial.println("Could not set wireless mode");
  }
  if (!WifiUartNetwork::setSSID(&wifi, SSID)) {
    Serial.println("Could not set SSID");
  }
  if (!WifiUartNetwork::setSecurity(&wifi, WIFIU_NET_ENC_WPA, SECURITY_PASSWORD)) {
    Serial.println("Could not set security");
  }
  if (!WifiUartNetwork::setNetworkSettingsToDHCP(&wifi)) {
    Serial.println("Could not set dhcp");
  }
  Serial.println("Done");
}

void userTwo() {
    // TODO: Your code here
}

void userThree() {
    // TODO: Your code here
}


bool reportSuccess(bool success) {
  if (success) {
    Serial.println("Success");
  }
  else {
    Serial.println("Failed");
  }
  return success;
}

void updateLed () {
    if (!wifi.isInCommandMode()) {
        digitalWrite(LED, LOW);
    }
    else {
        digitalWrite(LED, HIGH);
    }
}

void helpLine(char shortCut, char* description) {
  Serial.print("    ");
  Serial.print(shortCut);
  Serial.print(": ");
  Serial.println(description);
}

void help()
{
  Serial.println("Commands available:");
  helpLine(TOGGLE_INPUT, "Toggle input to wifimodule");
  helpLine(HELP, "This help message");
  helpLine(ENTER_COMMAND, "Enter command mode");
  helpLine(EXIT_COMMAND, "Exit command mode");
  helpLine(FACTORY_RESET, "Factory Reset");
  helpLine(RESET, "Reset");
  helpLine(HARDWARE_RESET, "Hardware Reset");
  helpLine(NETWORK_STATE, "Network State");
  helpLine(NETWORK_SETTINGS, "Network Settings");
  Serial.print("Wifi module is currently ");
  Serial.print(wifi.isInCommandMode() ? "" : "not ");
  Serial.println("in command mode");
  updateLed();
}

void networkState() {
  Serial.println("-----");
  WifiUartNetModeResponse mode = WifiUartNetwork::getWirelessMode(&wifi);
  if (mode.code == WIFIU_NET_RESPONSE_OK && mode.mode == WIFIU_NET_MODE_ADHOC) {
    Serial.println("AdHoc Mode");
  }
  else if (mode.code == WIFIU_NET_RESPONSE_OK && mode.mode == WIFIU_NET_MODE_INFRASTRUCTURE) {
    Serial.println("Infrastructure Mode");
  }
  else {
    Serial.println("Error getting mode of wireless");
  }

  WifiUartNetSSIDResponse ssid = WifiUartNetwork::getSSID(&wifi);
  if (ssid.code == WIFIU_NET_RESPONSE_OK) {
    Serial.print("SSID: "); Serial.println(ssid.ssid);
  }
  else {
    Serial.println("Error getting ssid");
  }

  WifiUartNetStateResponse state = WifiUartNetwork::getConnectionState(&wifi);
  if (state.code == WIFIU_NET_RESPONSE_OK && state.isConnected) {
    Serial.println("Connected!");
    Serial.print("IP: "); Serial.println(state.ip);
    Serial.print("Network Mask: "); Serial.println(state.netmask);
    Serial.print("Gateway: "); Serial.println(state.gateway);
    Serial.print("DNS: "); Serial.println(state.dns);
    Serial.print("Mac: "); Serial.println(state.physical);
  }
  else if (state.code == WIFIU_NET_RESPONSE_OK) {
    Serial.println("Not Connected");
  }
  else {
    Serial.println("Error getting connection status");
  }
  Serial.println("-----");
}

void networkSettings() {
  Serial.println("-----");
  WifiUartNetSettingsResponse state = WifiUartNetwork::getNetworkSettings(&wifi);
  if (state.code == WIFIU_NET_RESPONSE_OK && state.staticIp) {
    Serial.println("Static Ip Settings:");
    Serial.print("IP: "); Serial.println(state.ip);
    Serial.print("Network Mask: "); Serial.println(state.netmask);
    Serial.print("Gateway: "); Serial.println(state.gateway);
    Serial.print("DNS: "); Serial.println(state.dns);
  }
  else if (state.code == WIFIU_NET_RESPONSE_OK) {
    Serial.println("DHCP");
  }
  else {
    Serial.println("Error getting settings");
  }
  Serial.println("-----");
}

bool currentWebState () {
  bool ret = true; // always assume the worst!
  WifiUartMiscWebServiceResponse web = WifiUartMisc::getWebServiceStatus(&wifi);
  if (web.code == WIFIU_MISC_RESPONSE_OK) {
    ret = web.isEnabled;
  }
  return ret;
}
void webState() {
  Serial.print("Webservice state: ");
  Serial.println(currentWebState() ? "Enabled" : "Disabled");
}

void webToggle() {
  bool newState = !currentWebState();
  Serial.print("Setting state to: ");
  Serial.println(newState ? "Enabled" : "Disabled");
  reportSuccess(WifiUartMisc::setWebServiceStatus(&wifi, newState, 80));
}

void setup() {
  Serial.begin(115200);
  wifiSerial.begin(9600);
  help();
  pinMode(LED, OUTPUT);
  wifi.setHardwareResetPin(RESET_PIN, true);
}

void loop() {
  if (wifiSerial.available()) {
    while (wifiSerial.available()) {
      Serial.write(wifiSerial.read());
    }
  }
  if (Serial.available()) {
    char r = Serial.read();
    if (r == TOGGLE_INPUT) {
      writeToWifi = !writeToWifi;
      Serial.print("Write to wifi is now ");
      Serial.println(writeToWifi ? "on" : "off");
    }
    else if (writeToWifi) {
      wifiSerial.print(r);
    }
    else {
      switch (r) {
        case HELP:
          help();
          break;
        case ENTER_COMMAND:
          Serial.println("Enter Command Mode");
          reportSuccess (wifi.enterCommandMode());
          updateLed();
          break;
        case EXIT_COMMAND:
          Serial.println("Exit Command Mode");
          reportSuccess (wifi.exitCommandMode());
          updateLed();
          break;
        case FACTORY_RESET:
          Serial.println("Factory Reset");
          reportSuccess (wifi.factoryReset());
          updateLed();
          break;
        case RESET:
          Serial.println("Reset");
          reportSuccess (wifi.reset());
          updateLed();
          break;
        case HARDWARE_RESET:
          Serial.println("Hardware Reset");
          reportSuccess (wifi.hardwareReset());
          updateLed();
          break;
        case NETWORK_STATE:
          Serial.println("Network State");
          networkState();
          break;
        case NETWORK_SETTINGS:
          Serial.println("Network Settings");
          networkSettings();
          break;
        case WEB_STATE:
          webState();
          break;
        case WEB_TOGGLE:
          webToggle();
          break;
        case USER1:
          userOne();
          break;
        case USER2:
          userTwo();
          break;
        case USER3:
          userThree();
          break;
      }
    }
  }
}
