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

#include <ArduinoUnit.h>
#include <WifiUart.h>
#include <WifiUartHelper.h>
#include <WifiUartMock.h>

test (initWireless) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // WPRT, infrastructure mode
    stream.nextBytes("+OK=0\r\n\r\n");
    // SSID, ssid
    stream.nextBytes("+OK=\"hello\"\r\n\r\n");
    // ENCRY, wpa2
    stream.nextBytes("+OK=5\r\n\r\n");
    // KEY, ascii, wep index, password
    stream.nextBytes("+OK=1,0,\"secret\"\r\n\r\n");
    assertEqual(
    WIFIU_HELPER_RESPONSE_OK,
    WifiUartHelper::initWireless(
        &wifi,
        WIFIU_NET_MODE_INFRASTRUCTURE,
        "hello",
        WIFIU_NET_ENC_WPA2,
        "secret",
        WIFIU_NET_KEY_FORMAT_ASCII,
        0));
    assertEqual("AT+WPRT\rAT+SSID\rAT+ENCRY\rAT+KEY\r", stream.bytesWritten());
}
test (initWirelessModifiedMode) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // WPRT, adhoc (expect infrastructure mode)
    stream.nextBytes("+OK=1\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    // SSID, ssid
    stream.nextBytes("+OK=\"hello\"\r\n\r\n");
    // ENCRY, wpa2
    stream.nextBytes("+OK=5\r\n\r\n");
    // KEY, ascii, wep index, password
    stream.nextBytes("+OK=1,0,\"secret\"\r\n\r\n");
    assertEqual(
    WIFIU_HELPER_RESPONSE_MODIFIED,
    WifiUartHelper::initWireless(
        &wifi,
        WIFIU_NET_MODE_INFRASTRUCTURE,
        "hello",
        WIFIU_NET_ENC_WPA2,
        "secret",
        WIFIU_NET_KEY_FORMAT_ASCII,
        0));
    assertEqual("AT+WPRT\rAT+WPRT=!0\rAT+SSID\rAT+ENCRY\rAT+KEY\r", stream.bytesWritten());
}
test (initWirelessModifiedSSID) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // WPRT, infrastructure mode
    stream.nextBytes("+OK=0\r\n\r\n");
    // SSID, ssid (expect hello)
    stream.nextBytes("+OK=\"hellothere\"\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    // ENCRY, wpa2
    stream.nextBytes("+OK=5\r\n\r\n");
    // KEY, ascii, wep index, password
    stream.nextBytes("+OK=1,0,\"secret\"\r\n\r\n");
    assertEqual(
    WIFIU_HELPER_RESPONSE_MODIFIED,
    WifiUartHelper::initWireless(
        &wifi,
        WIFIU_NET_MODE_INFRASTRUCTURE,
        "hello",
        WIFIU_NET_ENC_WPA2,
        "secret",
        WIFIU_NET_KEY_FORMAT_ASCII,
        0));
    assertEqual("AT+WPRT\rAT+SSID\rAT+SSID=!hello\rAT+ENCRY\rAT+KEY\r", stream.bytesWritten());
}
test (initWirelessModifiedEncryption) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // WPRT, infrastructure mode
    stream.nextBytes("+OK=0\r\n\r\n");
    // SSID, hello
    stream.nextBytes("+OK=\"hello\"\r\n\r\n");
    // ENCRY, aes (expect wpa2)
    stream.nextBytes("+OK=6\r\n\r\n");
    // KEY, ascii, wep index, password
    stream.nextBytes("+OK=1,0,\"secret\"\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    assertEqual(
    WIFIU_HELPER_RESPONSE_MODIFIED,
    WifiUartHelper::initWireless(
        &wifi,
        WIFIU_NET_MODE_INFRASTRUCTURE,
        "hello",
        WIFIU_NET_ENC_WPA2,
        "secret",
        WIFIU_NET_KEY_FORMAT_ASCII,
        0));
    assertEqual("AT+WPRT\rAT+SSID\rAT+ENCRY\rAT+KEY\rAT+ENCRY=!5\rAT+KEY=!1,0,secret\r", stream.bytesWritten());
}
test (initWirelessModifiedKey) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // WPRT, infrastructure mode
    stream.nextBytes("+OK=0\r\n\r\n");
    // SSID, hello
    stream.nextBytes("+OK=\"hello\"\r\n\r\n");
    // ENCRY, wpa2
    stream.nextBytes("+OK=5\r\n\r\n");
    // KEY, ascii, wep index, password
    stream.nextBytes("+OK=1,0,\"secretsquirril\"\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    assertEqual(
    WIFIU_HELPER_RESPONSE_MODIFIED,
    WifiUartHelper::initWireless(
        &wifi,
        WIFIU_NET_MODE_INFRASTRUCTURE,
        "hello",
        WIFIU_NET_ENC_WPA2,
        "secret",
        WIFIU_NET_KEY_FORMAT_ASCII,
        0));
    assertEqual("AT+WPRT\rAT+SSID\rAT+ENCRY\rAT+KEY\rAT+ENCRY=!5\rAT+KEY=!1,0,secret\r", stream.bytesWritten());
}

test (initNetworkDHCP) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // NIP, dhcp, ip, mask, gateway, dns
    stream.nextBytes("+OK=0,\"127.0.0.1\",\"255.255.255.255\",\"0.0.0.0\",\"0.0.0.0\"\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_OK, WifiUartHelper::initNetworkDHCP(&wifi));
    assertEqual("AT+NIP\r", stream.bytesWritten());
}
test (initNetworkDHCPModified) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // NIP, static ip (expecting dhcp), ip, mask, gateway, dns
    stream.nextBytes("+OK=1,\"127.0.0.1\",\"255.255.255.255\",\"0.0.0.0\",\"0.0.0.0\"\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_MODIFIED, WifiUartHelper::initNetworkDHCP(&wifi));
    assertEqual("AT+NIP\rAT+NIP=!0\r", stream.bytesWritten());
}

void setup() {
    Serial.begin(115200);
    Serial.println("");
}

void loop() {
    Test::run();
}
