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
#include <WifiUartTransparent.h>
#include <SoftwareSerial.h>

// pin for the led to show if the module is in command mode(high) or not (low)
#define COMMAND_LED 13
// RX pin the wifi module is connected to (rx pin goes to tx on wifi module)
#define WIFI_RX 10
// TX pins the wifi module is connected to (tx pin goes to rx on wifi module)
#define WIFI_TX 11
// The pin connected to the hardware reset pin on wifi module (optional)
#define RESET_PIN 12
// Used to show how an LED can be turned on or off via the transparent connection
#define TEST_LED 6

// userOne() uses these bits of information to setup the wifi card to use wpa
// encryption, using the ssid and password below, setting up dns.
#define SSID "YOUR_SSID_HERE"
#define SECURITY_PASSWORD "YOUR_WPA_PASSWORD_HERE"

// userTwo() and userThree() uses these bits to listen/connect to a TCP port
#define TCP_LISTEN_PORT 7777
#define TCP_CONNECT_PORT 7777
#define TCP_CONNECT_SERVER "YOUR_IP_HERE"

// Commands
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
  Serial.print("Done (Don't forget to reset the module ");
  Serial.println("for the changes to take effect)");
}

void userTwo() {
    // Set module to listen to a TCP port
    // Connect to this port over TCP using telnet and send either 1 or 0
    // characters to turn the TEST_LED on or off
    if (!WifiUartTransparent::setServer(&wifi, TCP_LISTEN_PORT)) {
        Serial.println("Could not set server mode");
    }
    Serial.print("Done (Don't forget to reset the module ");
    Serial.println("for the changes to take effect)");
}

void userThree() {
    // Try running the nodejs script in examples/server.js to test. You will need
    // enter write to wifi mode (^) and then use the T (note case) character to
    // toggle the led on the TEST_LED pin
    if (!WifiUartTransparent::setClient(&wifi, TCP_CONNECT_SERVER, TCP_CONNECT_PORT)) {
        Serial.println("Could not set client mode");
    }
    Serial.print("Done (Don't forget to reset the module ");
    Serial.println("for the changes to take effect)");
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

void updateCommandLed () {
    if (!wifi.isInCommandMode()) {
        digitalWrite(COMMAND_LED, LOW);
    }
    else {
        digitalWrite(COMMAND_LED, HIGH);
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
  updateCommandLed();
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

bool isTestLedOn = false;
void receiveChar(char c) {
    if (!wifi.isInCommandMode()) {
        switch (c) {
            case '0':
                // Turn the test led off
                Serial.println("TURNING THE LED OFF");
                digitalWrite(TEST_LED, LOW);
                wifiSerial.write("OFF\n");
                isTestLedOn = false;
                break;
            case '1':
                // Turn the test led on
                Serial.println("TURNING THE LED ON");
                digitalWrite(TEST_LED, HIGH);
                wifiSerial.write("ON\n");
                isTestLedOn = true;
                break;
            case '2':
                Serial.println("STATUS REQUESTED");
                wifiSerial.write(isTestLedOn ? "ON\n" : "OFF\n");
                break;
        }
    }
    else {
        Serial.write(c);
    }

}

void setup() {
  Serial.begin(115200);
  wifiSerial.begin(9600);
  help();
  pinMode(COMMAND_LED, OUTPUT);
  pinMode(TEST_LED, OUTPUT);
  wifi.setHardwareResetPin(RESET_PIN, true);
}

void loop() {
  if (wifiSerial.available()) {
    while (wifiSerial.available()) {
      receiveChar(wifiSerial.read());
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
      if (r == '\r') {
          wifiSerial.print('\n');
      }
    }
    else {
      switch (r) {
        case HELP:
          help();
          break;
        case ENTER_COMMAND:
          Serial.println("Enter Command Mode");
          reportSuccess (wifi.enterCommandMode());
          updateCommandLed();
          break;
        case EXIT_COMMAND:
          Serial.println("Exit Command Mode");
          reportSuccess (wifi.exitCommandMode());
          updateCommandLed();
          break;
        case FACTORY_RESET:
          Serial.println("Factory Reset");
          reportSuccess (wifi.factoryReset());
          updateCommandLed();
          break;
        case RESET:
          Serial.println("Reset");
          reportSuccess (wifi.reset());
          updateCommandLed();
          break;
        case HARDWARE_RESET:
          Serial.println("Hardware Reset");
          reportSuccess (wifi.hardwareReset());
          updateCommandLed();
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
