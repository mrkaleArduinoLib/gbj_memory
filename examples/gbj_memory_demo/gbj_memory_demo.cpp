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
const unsigned int MEMORY_POSITION_MAX = 0x3F; // Real maximal position
const unsigned int MEMORY_POSITION_MIN = 0x08; // Real minimal position
const unsigned int MEMORY_POSITION_TEST = 0x00; // Logical test position

gbj_memory device = gbj_memory();
// gbj_memory device = gbj_memory(device.CLOCK_400KHZ);
// gbj_memory device = gbj_memory(device.CLOCK_100KHZ);
// gbj_memory device = gbj_memory(device.CLOCK_100KHZ, D2, D1);
int valueInt = 0xAA55; // 1010101001010101
float valueFloat = 123.45;

void errorHandler(String location)
{
  if (device.isSuccess())
  {
    return;
  }
  Serial.print(location);
  Serial.print(" - Error: ");
  Serial.print(device.getLastResult());
  Serial.print(" - ");
  switch (device.getLastResult())
  {
    // General
    case device.ERROR_ADDRESS:
      Serial.println("ERROR_ADDRESS");
      break;

    case device.ERROR_PINS:
      Serial.println("ERROR_PINS");
      break;

      // Arduino, Esspressif specific
#if defined(__AVR__) || defined(ESP8266) || defined(ESP32)
    case device.ERROR_BUFFER:
      Serial.println("ERROR_BUFFER");
      break;

    case device.ERROR_NACK_DATA:
      Serial.println("ERROR_NACK_DATA");
      break;

    case device.ERROR_NACK_OTHER:
      Serial.println("ERROR_NACK_OTHER");
      break;

      // Particle specific
#elif defined(PARTICLE)
    case device.ERROR_BUSY:
      Serial.println("ERROR_BUSY");
      break;

    case device.ERROR_END:
      Serial.println("ERROR_END");
      break;

    case device.ERROR_TRANSFER:
      Serial.println("ERROR_TRANSFER");
      break;

    case device.ERROR_TIMEOUT:
      Serial.println("ERROR_TIMEOUT");
      break;
#endif
    case device.ERROR_POSITION:
      Serial.println("ERROR_POSITION");
      break;

    default:
      Serial.println("Uknown error");
      break;
  }
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
  device.setPositionInBytes();
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
