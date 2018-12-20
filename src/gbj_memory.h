/*
  NAME:
  gbjMemory

  DESCRIPTION:
  Library for a generic memory on two-wire (I2C) bus as a parent class for
  specific memory chips.
  Library embraces and provides common methods used at every application
  working with random access memory chips, e.g., memorys, RTC RAM, etc.
  - Library does not deal with hardware specialties, e.g., addressing, delays,
    etc.
  - Library specifies (inherits from) the system TwoWire library.
  - Library implements extended error handling for memories.
  - Library provides some general system methods implemented differently for
    various platforms, especially Arduino, ESP8266, ESP32, and Particle.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
  (related to original code) and MIT License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkaleArduinoLib/gbj_memory.git
*/
#ifndef GBJ_MEMORY_H
#define GBJ_MEMORY_H

#include "gbj_twowire.h"


class gbj_memory : public gbj_twowire
{
public:
//------------------------------------------------------------------------------
// Public constants
//------------------------------------------------------------------------------
static const String VERSION;
enum ErrorCodes
{
  ERROR_POSITION = 12,  // Wrong position in memory; either 0 or no sufficient
                        // space for data storing or retrieving
};


//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------
/*
  Constructor taken from parent class.
*/
gbj_memory(uint32_t clockSpeed = CLOCK_100KHZ, uint8_t pinSDA = 4, uint8_t pinSCL = 5) \
: gbj_twowire(clockSpeed, pinSDA, pinSCL) {};


/*
  Initialize two-wire bus and parameters of the memory.

  DESCRIPTION:
  The method sanitizes and stores input parameters to the class instance object,
  which determine the capacity parameters of the memory.

  PARAMETERS:
  maxPosition - Maximal real position of the memory in bytes. Usually it expresses
                capacity of the memory minus one, but can be less if some end
                part of the memory cannot be used.
                - Data type: non-negative integer
                - Default value: none
                - Limited range: 0 ~ 65535

  pageSize - Size of the memory page in bytes. This is a chunk of bytes that can
             be written to the memory at once.
             - Data type: non-negative integer
             - Default value: none
             - Limited range: 0 ~ 65535

  minPosition - Minimal real memory position where the memory storage starts in
                bytes. For instance, real time clock chips have own read only memory
                starting just after time keeping registers.
                However, memory position in all other methods is counted from 0
                and considered as a logical position, i.e., position from
                the minimal real position.
                - Data type: non-negative integer
                - Default value: 0
                - Limited range: 0 ~ maxPosition
  RETURN:
  Result code.
*/
uint8_t begin(uint16_t maxPosition, uint16_t pageSize, uint16_t minPosition = 0);


/*
  Store byte stream to the memory.

  DESCRIPTION:
  The method writes input data byte stream to the memory chunked by memory pages
  if needed.
  - If length of the stored byte stream spans over memory pages, the method
    executes more bus transmissions, each for corresponding memory page.

  PARAMETERS:
  position - Logical memory position where the storing should start.
             - Data type: non-negative integer
             - Default value: none
             - Limited range: 0 ~ (getCapacityByte() - 1)

  dataBuffer - Pointer to the byte data buffer.
               - Data type: non-negative integer
               - Default value: none
               - Limited range: address space

  dataLen - Number of bytes to be stored in memory.
            - Data type: non-negative integer
            - Default value: none
            - Limited range: 0 ~ 65535

  RETURN:
  Result code.
*/
uint8_t storeStream(uint16_t position, uint8_t *dataBuffer, uint16_t dataLen);


/*
  Retrieve byte stream from the memory.

  DESCRIPTION:
  The method reads data from the memory and places it to the provided data buffer.
  The buffer should be defined outside this library with sufficient length for
  desired data.

  PARAMETERS:
  position - Logical memory position where the retrieving should start.
             - Data type: non-negative integer
             - Default value: none
             - Limited range: 0 ~ (getCapacityByte() - 1)

  dataBuffer - Pointer to the byte data buffer for placing read data.
               - Data type: non-negative integer
               - Default value: none
               - Limited range: address space

  dataLen - Number of bytes to be retrieved from memory.
            - Data type: non-negative integer
            - Default value: none
            - Limited range: 0 ~ 65535

  RETURN:
  Result code.
*/
uint8_t retrieveStream(uint16_t position, uint8_t *dataBuffer, uint16_t dataLen);


/*
  Fill consecutive positions in the memory with a value.

  DESCRIPTION:
  The method writes input byte to defined positions in the memory.

  PARAMETERS:
  position - Logical memory position where the filling should start.
             - Data type: non-negative integer
             - Default value: none
             - Limited range: 0 ~ (getCapacityByte() - 1)

  dataLen - Number of positions to be filled in memory.
            - Data type: non-negative integer
            - Default value: none
            - Limited range: 0 ~ 65535

  fillValue - Byte value that should be write to all defined positions in memory.
               - Data type: non-negative integer
               - Default value: none
               - Limited range: 0 ~ 255

  RETURN:
  Result code.
*/
uint8_t fill(uint16_t position, uint16_t dataLen, uint8_t fillValue);


/*
  Erase entire memory.

  DESCRIPTION:
  The method writes byte value 0xFF (all binary 1s) to whole memory.
  - The methods utilizes the method fill() from 0 position with entire byte
    capacity of the memory while it writes memory page by page.

  PARAMETERS:
  None

  RETURN:
  Result code.
*/
uint8_t erase();


/*
  Store a value to memory.

  DESCRIPTION:
  The method writes a value of particular data type, generic or custom, to the memory.
  - The method is templated utilizing method storeStream(), so that it determines
    data byte stream length automatically.
  - The method does not need to be called by templating syntax, because it is able
    to identify proper data type by data type of the just storing data value
    parameter.

  PARAMETERS:
  position - Logical memory position where the value storing should start.
             - Data type: non-negative integer
             - Default value: none
             - Limited range: 0 ~ (getCapacityByte() - 1)

  data - Value of particular data type that should be stored in the memory.
         - Data type: dynamic
         - Default value: none
         - Limited range: 0 ~ getCapacityByte()

  RETURN:
  Result code.
*/
template<class T>
uint8_t store(uint16_t position, T data)
{
  return storeStream(position, (uint8_t *)(void *)&data, sizeof(T));
}


/*
  Retrive a value from the memory.

  DESCRIPTION:
  The method reads a value of particular data type, generic or custom, from the
  memory to a referenced external variable.
  - The method is templated utilizing method retrieveStream(), so that it
    determines data byte stream length automatically.
  - The method does not need to be called by templating syntax, because it is able
    to identify proper data type from provided referenced variable.

  PARAMETERS:
  position - Logical memory position where the value retrieving should start.
             - Data type: non-negative integer
             - Default value: none
             - Limited range: 0 ~ (getCapacityByte() - 1)

  data - Pointer to a referenced variable for placing read data of desired type.
         - Data type: dynamic
         - Default value: none
         - Limited range: address space

  RETURN:
  Result code.
*/
template<class T>
uint8_t retrieve(uint16_t position, T &data)
{
  T *dataBuffer = &data;
  return retrieveStream(position, (uint8_t *)dataBuffer, sizeof(T));
}


/*
  Read current position

  DESCRIPTION:
  The method reads recently accessed position incremented by 1.

  PARAMETERS:
  data - Referenced variable for placing read data byte.
         - Data type: non-negative integer
         - Default value: none
         - Limited range: 0 ~ 255

  RETURN:
  Result code.
*/
uint8_t retrieveCurrent(uint8_t &data);


//------------------------------------------------------------------------------
// Public setters
//------------------------------------------------------------------------------
inline void setPositionInBytes() { _memoryStatus.positionInBytes = true; };
inline void setPositionInWords() { _memoryStatus.positionInBytes = false; };


//------------------------------------------------------------------------------
// Public getters
//------------------------------------------------------------------------------
inline uint32_t getCapacityByte() { return _memoryStatus.maxPosition + 1; };  // In bytes
inline uint32_t getCapacityBit() { return getCapacityByte() << 3; }; // In bits
inline uint32_t getCapacityKiByte() { return getCapacityByte() >> 10; };  // In Kibibytes
inline uint32_t getCapacityKiBit() { return getCapacityKiByte() << 3; }; // In Kibits
inline uint16_t getPageSize() { return _memoryStatus.pageSize; };  // In bytes
inline uint16_t getPages() { return getCapacityByte() / getPageSize(); };
inline uint16_t getPositionReal(uint16_t logicalPosition) \
  { return logicalPosition + _memoryStatus.minPosition; };
inline bool getPositionInBytes() { return _memoryStatus.positionInBytes; };


private:
//------------------------------------------------------------------------------
// Private attributes
//------------------------------------------------------------------------------
struct
{
  uint16_t maxPosition;  // Maximal available position in bytes
  uint16_t pageSize;  // Size of the memory page in bytes
  uint16_t minPosition;  // Physical position for logical 0 position of memory
  bool positionInBytes;  // Flag about using position long just 1 byte
} _memoryStatus;


//------------------------------------------------------------------------------
// Protected methods - they return result code if not stated else
//------------------------------------------------------------------------------
uint8_t checkPosition(uint16_t position, uint16_t dataLen);

};

#endif
