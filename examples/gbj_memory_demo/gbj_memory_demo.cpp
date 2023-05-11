/*
  NAME:
  Demonstration usage of gbjMemory library.

  DESCRIPTION:
  The sketch sets address of a device and writes and read data from it.
  - The memory parameters are set for real time clock chip DS1307 with own
    memory. Change them for another experimental device.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_memory.h"

// Change address for connected experimental memory device
const byte ADDRESS_DEVICE = 0x68; // DS1307
// const byte ADDRESS_DEVICE = 0x50; // AT24Cxx
const unsigned int MEMORY_POSITION_MAX = 0x3F; // Real maximal position
const unsigned int MEMORY_POSITION_MIN = 0x08; // Real minimal position
const unsigned int MEMORY_POSITION_TEST = 0x00; // Logical test position

gbj_memory device = gbj_memory();
// gbj_memory device = gbj_memory(device.CLOCK_400KHZ);
// gbj_memory device = gbj_memory(device.CLOCK_100KHZ);
// gbj_memory device = gbj_memory(device.CLOCK_100KHZ, D2, D1);
byte valueByte = 0xA5; // 10100101
int valueInt = 0xAA55; // 1010101001010101
float valueFloat = 123.45;

void errorHandler(String location)
{
  Serial.println(device.getLastErrorTxt(location));
  Serial.println("---");
  return;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("---");
  // Test constructor success
  if (device.isError())
  {
    errorHandler("Constructor");
    return;
  }
  // Initial two-wire bus (parameters for DS1307)
  if (device.isError(device.begin(MEMORY_POSITION_MAX,
                                  MEMORY_POSITION_MAX - MEMORY_POSITION_MIN + 1,
                                  MEMORY_POSITION_MIN)))
  {
    errorHandler("Begin");
    return;
  }
  device.setPositionInBytes(); // Comment for EEPROM AT24Cxx
  // device.setDelaySend(10); // Uncomment for EEPROM AT24Cxx
  // Set and test address
  if (device.isError(device.setAddress(ADDRESS_DEVICE)))
  {
    errorHandler("Address");
    return;
  }
  Serial.print("Address: 0x");
  Serial.println(device.getAddress(), HEX);
  Serial.print("Bus Clock: ");
  Serial.print(device.getBusClock() / 1000);
  Serial.println(" kHz");
  Serial.print("Pin SDA: ");
  Serial.println(device.getPinSDA());
  Serial.print("Pin SCL: ");
  Serial.println(device.getPinSCL());
  Serial.println("---");
  // Write and read byte
  Serial.println("Stored byte: 0x" + String(valueByte, HEX));
  if (device.isError(device.store(MEMORY_POSITION_TEST, valueByte)))
  {
    errorHandler("Store byte");
    return;
  }
  valueByte = 0;
  if (device.isError(device.retrieve(MEMORY_POSITION_TEST, valueByte)))
  {
    errorHandler("Retrieved byte");
    return;
  }
  Serial.println("Retrieved byte: 0x" + String(valueByte, HEX));
  Serial.println("---");
  // Write and read integer
  Serial.println("Stored integer: 0x" + String(valueInt, HEX));
  if (device.isError(device.store(MEMORY_POSITION_TEST, valueInt)))
  {
    errorHandler("Store integer");
    return;
  }
  valueInt = 0;
  if (device.isError(device.retrieve(MEMORY_POSITION_TEST, valueInt)))
  {
    errorHandler("Retrieved integer");
    return;
  }
  Serial.println("Retrieved integer: 0x" + String(valueInt, HEX));
  Serial.println("---");
  // Write and read float
  Serial.println("Stored float: " + String(valueFloat));
  if (device.isError(device.store(MEMORY_POSITION_TEST, valueFloat)))
  {
    errorHandler("Store float");
    return;
  }
  valueFloat = 0.0;
  if (device.isError(device.retrieve(MEMORY_POSITION_TEST, valueFloat)))
  {
    errorHandler("Retrieved float");
    return;
  }
  Serial.println("Retrieved float: " + String(valueFloat));
  Serial.println("---");
}

void loop() {}
