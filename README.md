arduinoWifiUart
===============

Library for the arduino to interface with the elechouse wifi uart device.

Usage
-----

For how to use the library, see the provided example (examples/SerialControl/SerialControl.ino), as well as the files in the tests directory.

The code for doing different commands to the wifi module is separated in different code files. 

- WifiUart.h: As a minimum you will need to include the WifiUart.h file. This in turn uses WifiUartCommand.h and WifiUartResponse.h. These three code modules implement the minimum set of code required to communicate with the wifi module, as well as provide some basic functionality, like being able to do soft/factory/hardware reset and entering/exiting command mode.
  - enterCommandMode()
  - exitCommandMode()
  - enterManualMode()
  - factoryReset()
  - reset()
  - hardwareReset()
- WifiUartCommand.h: Encapsulates all the different AT+ commands available for the module, and implements the protocol for sending those commands to the module.
  - send()
- WifiUartResponse.h: Gathers the response after a command has been sent and provides some helper methods to interpret that response
  - receive()
  - getCode()
  - getResponse()
  - getResponseLength()
  - getResponseField()
- WifiUartNetwork.h: Commands to setup the network on the module
  - Wireless Mode can be either ADHOC (sets up as an access point that can be joined), or INFRASTRUCTURE (the module will attempt to join an existing wifi network)
    - getWirelessMode()
    - setWirlessMode()
  - getSSID()
  - setSSID()
  - Setting the security for the encryption of the wifi network (WPA/WEP/etc) and the encryption key
    - getSecurity()
    - setSecurity()
  - Connection state will return if the module is connected to the network, and provide the IP address, NetMask, etc
    - getConnectionState()
  -  Network settings will only return/set information on the wifi module. I.e. the Connection State above will return the actual ip address provided by the DHCP server (if applicable), when the getNetworkSettings() might return an ip address that was set before setting the card to DHCP.
    - getNetworkSettings()
    - setNetworkSettingsToDHCP()
    - setNetworkSettingsToStaticIP()
- WifiUartSocket.h: Commands to help with socket communication
  - (no methods implemented yet)
- WifiUartTransparent.h: Commands that deal with transparent mode of the module
  - The module may be set into client mode (the module connects to a port on another server on the network), or server mode (the module will listen to a port)
    - getConnectionDetails()
    - setClient()
    - setServer()
- WifiUartMisc.h: Other commands that didn't fit anywhere else
  - These methods are for working with the built-in webserver for administering the wifi module over the wifi network. (By default they are turned on, I recommend to turn them off by default, or at least set a decent password.) (Note the password length is 6 characters exactly, No more. No less.)
    - getSystemPassword()
    - setSystemPassword()
    - getWebServiceStatus()
    - setWebServiceStatus()

Disclaimer
----------

This is my first attempt to write for the arduino.

License
-------
All code released for this project is under the MIT license.

About
-----

This libaray may be used with the [UART WiFi Network Server/Client Module Kits (TLG10UA03)] (http://www.elechouse.com/elechouse/index.php?main_page=product_info&cPath=90_186&products_id=1007) from elechouse. Essentially the wifi module has two modes... 
- command mode which takes 'AT+' commands to either fetch or set parameters of the module. E.g. you can set the SSID of the network you are trying to connect to, or setup the module as a wifi access point.
- transparent mode which sends any data on the serial interface through to the wifi network, and any data arriving on the wifi network through to the serial interface.

The scope of this library is to help with the management of sending AT+ commands to the TLG10UA03.



