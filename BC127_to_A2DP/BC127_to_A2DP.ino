/*****************************/
/* 16.4.2018  VERSION 1.0    */
/*****************************/

//#define DEBUG
#define seconds() (millis()/1000)
#include "LowPower.h"
#include <SparkFunbc127.h>
#include <EEPROM.h>
#include "StRingBuffer.h"

#ifdef DEBUG
#include <SoftwareSerial.h>
// Create a software serial port.
SoftwareSerial swPort(11, 10); // RX, TX
int ledBlue = A0;
int ledRed = A1;
int ledGreen = A2;
bool LED_on = true;
#else
#define swPort  Serial
int ledBlue = A3;
int ledRed = A4;
int ledGreen = A5;
bool LED_on = false;
#endif
int state = 0;
byte ADDRESS[13];
StRingBuffer ringBuffer = StRingBuffer(16);
int lastAction = 0;
String VERSION = "";
bool isHardcodedMac = false;

BC127 BTModu(&swPort);

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
  while (!Serial);
#endif
  swPort.begin(9600);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  debug("Starting...");
  red();
  //LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  //BTModu.exitDataMode();
  //BTModu.restore();
  //BTModu.stdSetParam("ENABLE_SPP", "on");
  BTModu.writeConfig();
  BTModu.reset();
  delay(500);
  //BTModu.stdCmd("DISCOVERABLE ON");
  swPort.print("VERSION");
  swPort.print("\r");
  swPort.flush();
  unsigned long startTime = millis();
  while ((startTime + 500) > millis())
  {
    while (swPort.available() > 0) 
    {
      char temp = swPort.read();
      VERSION += temp;
      if (temp = '\r') break;
    }    
  }
  isHardcodedMac = false;
  for (int kk = 0; kk < 12; kk ++)
  {
    ADDRESS[kk] = EEPROM.read(kk);
    if (ADDRESS[kk] == 0xFF || ADDRESS[kk] == 0x00)
      isHardcodedMac = true;
  }
  if (isHardcodedMac)
    String("CCFA00163796").getBytes(ADDRESS, 13);
  ADDRESS[12] = '\0';
}

//if (BTModu.stdGetParam("BATTERY_STATUS", &param) == 1)
//  debug("BATTERY_STATUS:" + param);

void loop()
{
  if (state == 0)
  {
    LedsOff();
    if (waitForSPP() == true)
      state = 1;
    else
      state = 2;
  }
  else if (state == 1)
  {
    if (swPort.available())
    {
      char in = swPort.read();
      String value = ringBuffer.addChar(in);
      swPort.print(in);
      lastAction = seconds();
      value.toUpperCase();
      if (value.substring(value.length() - 5) == "ERROR")
      {
        debug("Connection lost...");
        state = 0;
      }
      else if (value.substring(value.length() - 7) == "VERSION")
      {
        swPort.println(" ");
        swPort.println(VERSION);
        ringBuffer.clear();
      }
      else if (value.substring(0, 4) == "MAC=")
      {
        if (value.indexOf(":") >= 0)
        {
          swPort.println(" ");
          swPort.println("input MAC without :-Symbol");
        }
        else
        {
          value.substring(value.length() - 12).getBytes(ADDRESS, 13);
          for (int kk = 0; kk < 12; kk ++)
            EEPROM.update(kk, ADDRESS[kk]);
          ADDRESS[12] = '\0';
          String Address = String((char *)ADDRESS);
          Address.trim();
          swPort.println(" ");
          swPort.print("new MAC='");
          swPort.print(Address);
          swPort.println("'");
        }
        ringBuffer.clear();
      }
      else if (value.substring(value.length() - 4) == "MAC?")
      {
        swPort.println(" ");
        swPort.print("MAC='");
        swPort.print(String((char *)ADDRESS));
        swPort.println("'");
        ringBuffer.clear();
      }
      else if (value.substring(value.length() - 4) == "QUIT")
      {
        swPort.println(" ");
        swPort.println("Quit Data Mode");
        swPort.println(" ");
        swPort.println("********************************");
        swPort.println("* PLEASE DISCONNECT BLUETOOTH! *");
        swPort.println("********************************");
        swPort.println(" ");
        LedsOff();
        delay(10000);
        BTModu.exitDataMode();
        BTModu.restore();
        BTModu.writeConfig();
        BTModu.reset();
        delay(500);
        state = 2;
      }
    }
    /*
      if (state == 1 && seconds() - lastAction > 60)
      {
      swPort.println("Quit Data Mode");
      state = 2;
      }
    */
  }
  else if (state == 2)
  {
    BC127::opResult connectionState = BTModu.connectionState();
    if (connectionState == BC127::CONNECT_ERROR)
    {
      String Address = String((char *)ADDRESS);
      Address.trim();
      debug("Connecting...");
      blue();
      //BTModu.restore();
      //BTModu.stdSetParam("ENABLE_HFP", "off");
      //BTModu.stdSetParam("ENABLE_HFP_WBS", "off");
      //BTModu.stdSetParam("ENABLE_LED", "off");
      //BTModu.stdSetParam("ENABLE_MAP", "off");
      //BTModu.stdSetParam("ENABLE_PBAP", "off");
      //BTModu.stdSetParam("ENABLE_SPP", "off");
      BTModu.setClassicSource();
      BTModu.writeConfig();
      BTModu.reset();
      delay(400);
      //BTModu.stdCmd("DISCOVERABLE OFF");
      if (BTModu.connect(Address, BC127::A2DP) < 1)
      {
        red();
        debug("Error connecting Bluetooth Device (A2DP)");
      }
      else
      {
        if (BTModu.connect(Address, BC127::AVRCP) < 1)
        {
          red();
          digitalWrite(ledBlue, LED_on);
          debug("Error connecting Bluetooth Device (AVRCP)");
        }
        else
        {
          if (BTModu.musicCommands(BC127::PLAY) == 1)
          {
            green();
            debug("A2DP connected successfully, sending music");
          }
          else
          {
            red();
            digitalWrite(ledGreen, LED_on);
            debug("MusicCommand Error");
          }
        }
      }
      LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
      LedsOff();
    }
    else if (isHardcodedMac == true && connectionState != BC127::CONNECT_ERROR)
      green();
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}

void red()
{
  LedsOff();
  digitalWrite(ledRed, LED_on);
}

void blue()
{
  LedsOff();
  digitalWrite(ledBlue, LED_on);
}

void green()
{
  LedsOff();
  digitalWrite(ledGreen, LED_on);
}

void LedsOff()
{
  digitalWrite(ledBlue, !LED_on);
  digitalWrite(ledRed, !LED_on);
  digitalWrite(ledGreen, !LED_on);
}

void debug(String strMessage)
{
#ifdef DEBUG
  Serial.println(strMessage);
  delay(100);
#endif
}

bool waitForSPP()
{
  String value = ringBuffer.addChar(' ');
  blue();
  debug("waiting for SPP...");
  while (seconds() < 30)
  {
    value = "";
    BTModu.stdGetCommand("STATUS", "SPP", &value);
    if (value.indexOf("CONNECTED SPP") >= 0)
    {
      debug("SPP connected!");
      debug("ENTER_DATA_MODE...");
      BTModu.enterDataMode();
      delay(1000);
      //BTModu.stdCmd("ENTER_DATA_MODE 15");
      swPort.flush();
      while (swPort.available())
        swPort.read();
      ringBuffer.clear();
      digitalWrite(ledGreen, LED_on);
      return true;
    }
  }
  return false;
}


