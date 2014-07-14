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
#include <WifiUartHelper.h>

// EXAMPLE: Connect an LED to this pin to show the state of the wifi module (if it is in command
//          mode or not)
#define COMMAND_LED 13

// EXAMPLE: Connect the TX pin on the wifi module to this pin (the rx pin on the software serial
//          port will connect to the tx pin on the wifi module)
#define WIFI_RX 10

// EXAMPLE: Connect the RX pin on the wifi module to this pin (the tx pin on the software serial
//          port will connect to the rx pin on the wifi module)
#define WIFI_TX 11

// EXAMPLE: Connect the reset pin on the wifi module to this pin
#define RESET_PIN 12

// EXAMPLE: Connect an LED to this pin to show how an LED can be turned on or off via the
//          transparent connection
#define TEST_LED 6

// EXAMPLE: Fill in these bits so the initArduino() method can initialise the wifi card for your
//          network. Alternativly modify the initArduino() method to setup your card the way you
//          want.
#define SSID "YOUR_SSID_HERE"
#define SECURITY_PASSWORD "YOUR_WPA2_PASSWORD_HERE"
#define WEB_ADMIN_ENABLED false
#define WEB_ADMIN_PORT 80
// NOTE: Passcode must be 6 characters, no more, no less
#define WEB_ADMIN_PASSCODE "789789"
#define TCP_SERVER true // Setup as a server, set to false to setup as a client
#define TCP_PORT 7777 // client and server option
#define TCP_SERVERNAME "YOUR_SERVER_IP_HERE" // client option

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
//#define USER1 '1'
//#define USER2 '2'
//#define USER3 '3'

// Error Blink Codes
#define ERR_UNKNOWN 1
#define ERR_INIT_COMMAND 2
#define ERR_INIT_WIRELESS 3
#define ERR_INIT_DHCP 4
#define ERR_INIT_WEB 5
#define ERR_INIT_CLIENTSERVER 6

SoftwareSerial wifiSerial(WIFI_RX, WIFI_TX); // RX TX
WifiUart wifi = WifiUart(&wifiSerial);
bool writeToWifi = false;
bool isTestLedOn = false;

/**
 * Put the arduino in an error state.
 *
 * This will blink the LED's constantly to give a visual display that the program is erroring out
 * and will not be processing any further commands
 */
void errorState(int errorCode) {
    while (true) {
        Serial.print("Error ");
        Serial.print(errorCode);
        Serial.println("!");
        for (int x = 0; x < errorCode; x++) {
            digitalWrite(COMMAND_LED, HIGH);
            digitalWrite(TEST_LED, HIGH);
            delay(100);
            digitalWrite(COMMAND_LED, LOW);
            digitalWrite(TEST_LED, LOW);
            delay(100);
        }
        delay(900);
    }
}
void errorState() {
    errorState(ERR_UNKNOWN);
}

/**
 * This gets called at the end of the setup() function. I am using this method as a way to
 * initialize the wifi card on boot (showing the use of the helper init methods)
 */
void initArduino() {
    Serial.println("Init Arduino - start with a hardware reset");
    wifi.hardwareReset();
    bool needsReset = false;
    if (!wifi.enterCommandMode()) {
        errorState(ERR_INIT_COMMAND);
        return; // calling errorState should never return
    }

    // Init the wireless settings
    bool success = WifiUartHelper::initWireless(
            &wifi,
            WIFIU_NET_MODE_INFRASTRUCTURE,
            SSID,
            WIFIU_NET_ENC_WPA2,
            SECURITY_PASSWORD,
            WIFIU_NET_KEY_FORMAT_ASCII,
            0);
    if (success == WIFIU_HELPER_RESPONSE_ERR) {
        errorState(ERR_INIT_WIRELESS);
        return; // calling errorState should never return
    }
    else if (success == WIFIU_HELPER_RESPONSE_MODIFIED) {
        Serial.println("Modified the wireless settings");
        needsReset = true;
        success = WIFIU_HELPER_RESPONSE_OK;
    }

    // Init DHCP/StaticIP settings
    success = WifiUartHelper::initNetworkDHCP(&wifi);
    if (success == WIFIU_HELPER_RESPONSE_ERR) {
        errorState(ERR_INIT_DHCP);
        return; // calling errorState should never return
    }
    else if (success == WIFIU_HELPER_RESPONSE_MODIFIED) {
        Serial.println("Modified the dhcp settings");
        needsReset = true;
        success = WIFIU_HELPER_RESPONSE_OK;
    }

    // Init web service settings
    success = WifiUartHelper::initMisc(&wifi, WEB_ADMIN_ENABLED, WEB_ADMIN_PORT, WEB_ADMIN_PASSCODE);
    if (success == WIFIU_HELPER_RESPONSE_ERR) {
        errorState(ERR_INIT_WEB);
        return; // calling errorState should never return
    }
    else if (success == WIFIU_HELPER_RESPONSE_MODIFIED) {
        Serial.println("Modified the misc settings");
        needsReset = true;
        success = WIFIU_HELPER_RESPONSE_OK;
    }

    // Initialise to connect to a sever, or act as a client
    if (TCP_SERVER) {
        success = WifiUartHelper::initServer(&wifi, TCP_PORT, WIFIU_NET_ATRM_TCP, WIFIU_TRANS_DEFAULT_TIMEOUT);
    }
    else {
        success = WifiUartHelper::initClient(&wifi, TCP_SERVERNAME, TCP_PORT, WIFIU_NET_ATRM_TCP);
    }
    if (success == WIFIU_HELPER_RESPONSE_ERR) {
        errorState(ERR_INIT_CLIENTSERVER);
        return; // calling errorState should never return
    }
    else if (success == WIFIU_HELPER_RESPONSE_MODIFIED) {
        Serial.println("Modified the client/server settings");
        needsReset = true;
        success = WIFIU_HELPER_RESPONSE_OK;
    }

    if (needsReset) {
        Serial.println("Wifi card settings have been corrected. We need to reset the module before we continue...");
        reportSuccess (wifi.reset());
    }
    else {
        Serial.println("Wifi card settings correct");
    }
    Serial.println("Done");
}

void userOne() {
    //// set module to connect to home wifi
    //if (!WifiUartNetwork::setWirelessMode(&wifi, WIFIU_NET_MODE_INFRASTRUCTURE)) {
    //    Serial.println("Could not set wireless mode");
    //}
    //if (!WifiUartNetwork::setSSID(&wifi, SSID)) {
    //    Serial.println("Could not set SSID");
    //}
    //if (!WifiUartNetwork::setSecurity(&wifi, WIFIU_NET_ENC_WPA, SECURITY_PASSWORD)) {
    //    Serial.println("Could not set security");
    //}
    //if (!WifiUartNetwork::setNetworkSettingsToDHCP(&wifi)) {
    //    Serial.println("Could not set dhcp");
    //}
    //Serial.print("Done (Don't forget to reset the module ");
    //Serial.println("for the changes to take effect)");
}

void userTwo() {
    //// Set module to listen to a TCP port
    //// Connect to this port over TCP using telnet and send either 1 or 0
    //// characters to turn the TEST_LED on or off
    //if (!WifiUartTransparent::setServer(&wifi, TCP_LISTEN_PORT)) {
    //    Serial.println("Could not set server mode");
    //}
    //Serial.print("Done (Don't forget to reset the module ");
    //Serial.println("for the changes to take effect)");
}

void userThree() {
    //// Try running the nodejs script in examples/server.js to test. You will need
    //// enter write to wifi mode (^) and then use the T (note case) character to
    //// toggle the led on the TEST_LED pin
    //if (!WifiUartTransparent::setClient(&wifi, TCP_CONNECT_SERVER, TCP_CONNECT_PORT)) {
    //    Serial.println("Could not set client mode");
    //}
    //Serial.print("Done (Don't forget to reset the module ");
    //Serial.println("for the changes to take effect)");
}

/**
 * Helper method to easily report success on the serial based on a boolean value
 *
 * @param bool success
 * @return bool
 */
bool reportSuccess(bool success) {
    if (success) {
        Serial.println("Success");
    }
    else {
        Serial.println("Failed");
    }
    return success;
}

/**
 * Sets the COMMAND_LED to the correct state based on the value of the wifi module
 */
void updateCommandLed () {
    if (wifi.isInCommandMode()) {
        digitalWrite(COMMAND_LED, HIGH);
    }
    else {
        digitalWrite(COMMAND_LED, LOW);
    }
}

/**
 * Helper method to print out a line in the help menu
 *
 * @param char shortCut
 * @param char* description
 * @return void
 */
void helpLine(char shortCut, char* description) {
    Serial.print("    ");
    Serial.print(shortCut);
    Serial.print(": ");
    Serial.println(description);
}

/**
 * Print out a list of commands that may be used when using the example
 */
void help() {
    Serial.println("Commands available:");
    helpLine(TOGGLE_INPUT, "Toggle input to wifimodule");
    helpLine(HELP, "This help message");
    helpLine(ENTER_COMMAND, "Enter command mode");
    helpLine(EXIT_COMMAND, "Exit command mode");
    helpLine(FACTORY_RESET, "Factory Reset");
    helpLine(RESET, "Reset");
    helpLine(HARDWARE_RESET, "Hardware Reset");
    helpLine(NETWORK_STATE, "Print Network State");
    helpLine(NETWORK_SETTINGS, "Print Network Settings");
    Serial.print("Wifi module is currently ");
    Serial.print(wifi.isInCommandMode() ? "" : "not ");
    Serial.println("in command mode");
}

/**
 * Print the network state
 *
 * TODO: Move this to the helper module
 */
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

/**
 * Prints the netowrk settings
 *
 * TODO: Move this to the helper module
 */
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

// Web State Methods
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

/**
 * Takes a character (from the wifi card) and does something with it
 */
void receiveChar(char c) {
    if (!wifi.isInCommandMode()) {
        // We only want to do commands on input coming in through the transparent interface
        // EXAMPLE: You can modify what happens here for any character input. For more advaned
        //          functionality you may want to implement a state machine.
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
        // We want to see on the console what the wifi card is giving us while we are in command mode
        Serial.write(c);
    }
}

/**
 * Takes a character and runs the associated command
 *
 * @param char r
 * @returns void
 */
void runMenuCommand (char r) {
    switch (r) {
        case HELP:
            help();
            break;
        case ENTER_COMMAND:
            Serial.println("Enter Command Mode");
            reportSuccess (wifi.enterCommandMode());
            break;
        case EXIT_COMMAND:
            Serial.println("Exit Command Mode");
            reportSuccess (wifi.exitCommandMode());
            break;
        case FACTORY_RESET:
            Serial.println("Factory Reset");
            reportSuccess (wifi.factoryReset());
            break;
        case RESET:
            Serial.println("Reset");
            reportSuccess (wifi.reset());
            break;
        case HARDWARE_RESET:
            Serial.println("Hardware Reset");
            reportSuccess (wifi.hardwareReset());
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
        //case USER1:
        //    userOne();
        //    break;
        //case USER2:
        //    userTwo();
        //    break;
        //case USER3:
        //    userThree();
        //    break;
    }
}

// The arduino setup method
void setup() {
    Serial.begin(115200);
    wifiSerial.begin(9600);
    pinMode(COMMAND_LED, OUTPUT);
    pinMode(TEST_LED, OUTPUT);
    wifi.setHardwareResetPin(RESET_PIN, true);
    initArduino();
    help();
}

// The arduino run loop
void loop() {
    if (wifiSerial.available()) {
        // Any character received from the wifi serial, at the moment just gets output to the
        // arduino console
        while (wifiSerial.available()) {
            receiveChar(wifiSerial.read());
        }
    }
    if (Serial.available()) {
        // We got a character.
        char r = Serial.read();
        if (r == TOGGLE_INPUT) {
            // It is the shortcut to toggle input between the wifi module and arduino
            writeToWifi = !writeToWifi;
            Serial.print("Write to wifi is now ");
            Serial.println(writeToWifi ? "on" : "off");
        }
        else if (writeToWifi) {
            // We are writing to the wifi
            wifiSerial.print(r);
            if (r == '\r') {
                wifiSerial.print('\n');
            }
        }
        else {
            // We are not writing to wifi, and its not the command to toggle between. Enter our menu
            // and pick the right one for the job being asked.
            runMenuCommand(r);
            // One of the above commands may have changed the state of the wifi module, so we want
            // to update the command LED just in case
            updateCommandLed();
        }
    }
}

