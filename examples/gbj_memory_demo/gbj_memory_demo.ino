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
#define SKETCH "GBJ_MEMORY_DEMO 1.0.0"

#include "gbj_memory.h"

// Change address for connected experimental memory device
const byte ADDRESS_DEVICE = 0x68;  // DS1307
gbj_memory Device = gbj_memory();
// gbj_memory Device = gbj_memory(gbj_memory::CLOCK_400KHZ);
// gbj_memory Device = gbj_memory(gbj_memory::CLOCK_100KHZ);
// gbj_memory Device = gbj_memory(gbj_memory::CLOCK_100KHZ, D2, D1);
const unsigned int MEMORY_POSITION = 0;
int valueInt = 0xAA55;
float valueFloat = 123.45;


void errorHandler(String location)
{
  if (Device.isSuccess()) return;
  Serial.print(location);
  Serial.print(" - Error: ");
  Serial.print(Device.getLastResult());
  Serial.print(" - ");
  switch (Device.getLastResult())
  {
    // General
    case gbj_memory::ERROR_ADDRESS:
      Serial.println("ERROR_ADDRESS");
      break;

    case gbj_memory::ERROR_PINS:
      Serial.println("ERROR_PINS");
      break;

    // Arduino, Esspressif specific
#if defined(__AVR__) || defined(ESP8266) || defined(ESP32)
    case gbj_memory::ERROR_BUFFER:
      Serial.println("ERROR_BUFFER");
      break;

    case gbj_memory::ERROR_NACK_DATA:
      Serial.println("ERROR_NACK_DATA");
      break;

    case gbj_memory::ERROR_NACK_OTHER:
      Serial.println("ERROR_NACK_OTHER");
      break;

    // Particle specific
#elif defined(PARTICLE)
    case gbj_memory::ERROR_BUSY:
      Serial.println("ERROR_BUSY");
      break;

    case gbj_memory::ERROR_END:
      Serial.println("ERROR_END");
      break;

    case gbj_memory::ERROR_TRANSFER:
      Serial.println("ERROR_TRANSFER");
      break;

    case gbj_memory::ERROR_TIMEOUT:
      Serial.println("ERROR_TIMEOUT");
      break;
#endif
    case gbj_memory::ERROR_POSITION:
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
  // Test constructor success
  if (Device.isError())
  {
    errorHandler("Constructor");
    return;
  }
  // Initial two-wire bus
  if (Device.begin(56, 56, 8))  // Parameters for DS1307
  {
    errorHandler("Begin");
    return;
  }
  Device.setPositionInBytes();
  // Set and test address
  Device.setAddress(ADDRESS_DEVICE);
  if (Device.isError())
  {
    errorHandler("Address");
    return;
  }
  Serial.print("Address: 0x");
  Serial.println(Device.getAddress(), HEX);
  Serial.print("Bus Clock: ");
  Serial.print(Device.getBusClock() / 1000);
  Serial.println(" kHz");
  Serial.print("Pin SDA: ");
  Serial.println(Device.getPinSDA());
  Serial.print("Pin SCL: ");
  Serial.println(Device.getPinSCL());
  Serial.println("---");
  // Write and read integer
  Serial.println("Stored integer: 0x" + String(valueInt, HEX));
  if (Device.store(MEMORY_POSITION, valueInt))
  {
    errorHandler("Store integer");
    return;
  }
  valueInt = 0;
  if (Device.retrieve(MEMORY_POSITION, valueInt))
  {
    errorHandler("Retrieved integer");
    return;
  }
  Serial.println("Retrieved integer: 0x" + String(valueInt, HEX));
  Serial.println("---");
  // Write and read float
  Serial.println("Stored float: " + String(valueFloat));
  if (Device.store(MEMORY_POSITION, valueFloat))
  {
    errorHandler("Store float");
    return;
  }
  valueFloat = 0.0;
  if (Device.retrieve(MEMORY_POSITION, valueFloat))
  {
    errorHandler("Retrieved float");
    return;
  }
  Serial.println("Retrieved float: " + String(valueFloat));
  Serial.println("---");
  Serial.println("END");
}

void loop() {}
