/*
  NAME:
  gbjMemory

  DESCRIPTION:
  Library for a generic memory on two-wire (I2C) bus as a parent class for
  specific memory chips.
  - Library embraces and provides common methods used at every application
  working with random access memory chips, e.g., memories, RTC RAM, etc.
  - Library does not deal with hardware specialties, e.g., addressing, delays,
  etc.
  - Library specifies (inherits from) the system TwoWire library.
  - Library provides some general system methods implemented differently for
  various platforms, especially Arduino, ESP8266, ESP32, and Particle.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3
  http://www.gnu.org/licenses/gpl-3.0.html (related to original code) and MIT
  License (MIT) for added code.

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
  gbj_memory(ClockSpeed clockSpeed = ClockSpeed::CLOCK_100KHZ,
             uint8_t pinSDA = 4,
             uint8_t pinSCL = 5)
    : gbj_twowire(clockSpeed, pinSDA, pinSCL){};

  /*
    Initialize two-wire bus and parameters of the memory.

    DESCRIPTION:
    The method sanitizes and stores input parameters to the class instance
    object, which determine the capacity parameters of the memory.

    PARAMETERS:
    maxPosition - Maximal real position of the memory in bytes. Usually it
    expresses capacity of the memory minus one, but can be less if some end part
    of the memory cannot be used.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 65535

    pageSize - Size of the memory page in bytes. This is a chunk of bytes that
    can be written to the memory at once.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 65535

    minPosition - Minimal real memory position where the memory storage starts
    in bytes. For instance, real time clock chips have own read only memory
    starting just after time keeping registers. However, memory position in all
    other methods is counted from 0 and considered as a logical position, i.e.,
    position from the minimal real position.
      - Data type: non-negative integer
      - Default value: 0
      - Limited range: 0 ~ maxPosition

    RETURN: Result code
  */
  inline ResultCodes begin(uint16_t maxPosition,
                           uint16_t pageSize,
                           uint16_t minPosition = 0)
  {
    _memoryStatus.minPosition = min(minPosition, maxPosition);
    _memoryStatus.maxPosition = maxPosition - _memoryStatus.minPosition;
    _memoryStatus.pageSize = max(pageSize, 1);
    return gbj_twowire::begin();
  }

  /*
    Store byte stream to the memory.

    DESCRIPTION:
    The method writes input data byte stream to the memory chunked by memory
    pages if needed.
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

    RETURN: Result code
  */
  inline ResultCodes storeStream(uint16_t position,
                                 uint8_t *dataBuffer,
                                 uint16_t dataLen)
  {
    uint16_t realPosition = getPositionReal(position);
    if (checkPosition(position, dataLen))
    {
      return getLastResult();
    }
    while (dataLen)
    {
      uint8_t pageLen = min(
        dataLen, _memoryStatus.pageSize - position % _memoryStatus.pageSize);
      if (busSendStreamPrefixed(dataBuffer,
                                dataLen,
                                false,
                                reinterpret_cast<uint8_t *>(&realPosition),
                                getPositionInBytes() ? 1 : 2,
                                true,
                                true))
      {
        return getLastResult();
      }
      dataLen -= pageLen;
      dataBuffer += pageLen;
      position += pageLen;
    }
    return getLastResult();
  }

  /*
    Retrieve byte stream from the memory.

    DESCRIPTION:
    The method reads data from the memory and places it to the provided data
    buffer. The buffer should be defined outside this library with sufficient
    length for desired data.

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

    RETURN: Result code
  */
  inline ResultCodes retrieveStream(uint16_t position,
                                    uint8_t *dataBuffer,
                                    uint16_t dataLen)
  {
    uint16_t realPosition = getPositionReal(position);
    if (checkPosition(position, dataLen))
    {
      return getLastResult();
    }
    setBusRpte();
    if (busSendStream(reinterpret_cast<uint8_t *>(&realPosition),
                      getPositionInBytes() ? 1 : 2,
                      true))
    {
      return getLastResult();
    }
    setBusStop();
    return busReceive(reinterpret_cast<uint8_t *>(dataBuffer), dataLen);
  }

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

    fillValue - Byte value that should be write to all defined positions in
    memory.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 255

    RETURN: Result code
  */
  inline ResultCodes fill(uint16_t position,
                          uint16_t dataLen,
                          uint8_t fillValue)
  {
    // Sanitize
    dataLen = min(dataLen, getCapacityByte() - position);
    if (checkPosition(position, dataLen))
    {
      return getLastResult();
    }
    // Store
    uint8_t dataBuffer[dataLen];
    for (uint8_t i = 0; i < dataLen; i++)
    {
      dataBuffer[i] = fillValue;
    }
    return storeStream(position, dataBuffer, dataLen);
  }

  /*
    Erase entire memory.

    DESCRIPTION:
    The method writes byte value 0xFF (all binary 1s) to whole memory.
    - The methods utilizes the method fill() from 0 position with entire byte
      capacity of the memory while it writes memory page by page.

    PARAMETERS:
    None

    RETURN: Result code
  */
  inline ResultCodes erase()
  {
    uint16_t pages = getPages();
    uint16_t position = 0;
    while (pages--)
    {
      if (fill(position, getPageSize(), 0xFF))
      {
        return getLastResult();
      }
      position += getPageSize();
    }
    return getLastResult();
  }

  /*
    Store a value to memory.

    DESCRIPTION:
    The method writes a value of particular data type, generic or custom, to the
    memory.
    - The method is templated utilizing method storeStream(), so that it
    determines data byte stream length automatically.
    - The method does not need to be called by templating syntax, because it is
    able to identify proper data type by data type of the just storing data
    value parameter.

    PARAMETERS:
    position - Logical memory position where the value storing should start.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ (getCapacityByte() - 1)

    data - Value of particular data type that should be stored in the memory.
      - Data type: dynamic
      - Default value: none
      - Limited range: 0 ~ getCapacityByte()

    RETURN: Result code
  */
  template<class T>
  inline ResultCodes store(uint16_t position, T data)
  {
    return storeStream(
      position, static_cast<uint8_t *>(static_cast<void *>(&data)), sizeof(T));
  }

  /*
    Retrive a value from the memory.

    DESCRIPTION:
    The method reads a value of particular data type, generic or custom, from
    the memory to a referenced external variable.
    - The method is templated utilizing method retrieveStream(), so that it
    determines data byte stream length automatically.
    - The method does not need to be called by templating syntax, because it is
    able to identify proper data type from provided referenced variable.

    PARAMETERS:
    position - Logical memory position where the value retrieving should start.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ (getCapacityByte() - 1)

    data - Pointer to a referenced variable for placing read data of desired
    type.
      - Data type: dynamic
      - Default value: none
      - Limited range: address space

    RETURN: Result code
  */
  template<class T>
  inline ResultCodes retrieve(uint16_t position, T &data)
  {
    T *dataBuffer = &data;
    return retrieveStream(
      position, reinterpret_cast<uint8_t *>(dataBuffer), sizeof(T));
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

    RETURN: Result code
  */
  inline ResultCodes retrieveCurrent(uint8_t &data)
  {
    uint8_t *dataBuffer = &data;
    return busReceive(static_cast<uint8_t *>(dataBuffer), 1);
  }

  // Setters
  inline void setPositionInBytes() { _memoryStatus.positionInBytes = true; }
  inline void setPositionInWords() { _memoryStatus.positionInBytes = false; }

  // Getters
  inline uint32_t getCapacityByte() { return _memoryStatus.maxPosition + 1; }
  inline uint32_t getCapacityBit() { return getCapacityByte() << 3; }
  inline uint32_t getCapacityKiByte() { return getCapacityByte() >> 10; }
  inline uint32_t getCapacityKiBit() { return getCapacityKiByte() << 3; }
  inline uint16_t getPageSize() { return _memoryStatus.pageSize; } // In bytes
  inline uint16_t getPages() { return getCapacityByte() / getPageSize(); }
  inline uint16_t getPositionReal(uint16_t logicalPosition)
  {
    return logicalPosition + _memoryStatus.minPosition;
  }
  inline bool getPositionInBytes() { return _memoryStatus.positionInBytes; };
  inline bool getPositionInWords() { return !_memoryStatus.positionInBytes; };

private:
  struct MemoryStatus
  {
    uint16_t maxPosition; // Maximal available position in bytes
    uint16_t pageSize; // Size of the memory page in bytes
    uint16_t minPosition; // Physical position for logical 0 position of memory
    bool positionInBytes; // Flag about using position long just 1 byte
  } _memoryStatus;
  inline ResultCodes checkPosition(uint16_t position, uint16_t dataLen)
  {
    setLastResult();
    if (dataLen == 0 || _memoryStatus.maxPosition < (position + dataLen))
    {
      return setLastResult(ResultCodes::ERROR_POSITION);
    }
    return getLastResult();
  }
};

#endif
