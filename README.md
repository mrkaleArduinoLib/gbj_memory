# gbjMemory

Library for a generic memory on `two-wire` (also known as <abbr title='Inter-Integrated Circuit'>I2C</abbr>) bus as a parent class for specific memory chips.
* Library specifies (inherits from) the library `gbjTwoWire`, which specifies the system `TwoWire` library.
* The class from the library is not intended to be used directly in a sketch, just as a parent class for specific memory chips libraries.


#### Particle hardware configuration
* Connect microcontroller's pin `D0` to memory's pin **SDA** (Serial Data).
* Connect microcontroller's pin `D1` to memory's pin **SCL** (Serial Clock).

#### Arduino UNO hardware configuration
* Connect microcontroller's pin `A4` to memory's pin **SDA** (Serial Data).
* Connect microcontroller's pin `A5` to memory's pin **SCL** (Serial Clock).

#### Espressif - ESP8266, ESP32 default hardware configuration
* Connect microcontroller's pin `D2` to memory's pin **SDA** (Serial Data).
* Connect microcontroller's pin `D1` to memory's pin **SCL** (Serial Clock).


<a id="dependency"></a>
## Dependency

#### Particle platform
* **Particle.h**: Includes alternative (C++) data type definitions.

#### Arduino platform
* **Arduino.h**: Main include file for the Arduino SDK version greater or equal to 100.
* **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.
* **TwoWire**: I2C system library loaded from the file `Wire.h`.

#### Custom Libraries
* **gbjTwoWire**: I2C custom library loaded from the file `gbj_twowire.h`, which provides common bus functionality.


<a id="constants"></a>

## Constants
The library does not have specific constants. Error codes as well as result code are inherited from the parent library [gbjTwoWire](#dependency) only.


<a id="interface"></a>

## Interface

#### Main
* [gbj_memory()](#gbj_memory)
* [begin()](#begin)
* [store()](#store)
* [storeStream()](#storeStream)
* [retrieve()](#retrieve)
* [retrieveStream()](#retrieveStream)
* [retrieveCurrent()](#retrieveCurrent)
* [fill()](#fill)
* [erase()](#erase)

#### Setters
* [setPositionInBytes()](#setPositionIn)
* [setPositionInWords()](#setPositionIn)

#### Getters
* [getCapacityByte()](#getCapacityByte)
* [getCapacityKiByte()](#getCapacityByte)
* [getCapacityBit()](#getCapacityBit)
* [getCapacityKiBit()](#getCapacityBit)
* [getPageSize()](#getPageSize)
* [getPages()](#getPages)
* [getPositionReal()](#getPositionReal)
* [getPositionInBytes()](#getPositionIn)
* [getPositionInWords()](#getPositionIn)

Other possible setters and getters are inherited from the parent library [gbjTwoWire](#dependency) and described there.


<a id="gbj_memory"></a>

## gbj_memory()

#### Description
The library does not need special constructor and destructor, so that the inherited ones are good enough and there is no need to define them in the library, just use it with default or specific parameters as defined at constructor of parent library [gbjTwoWire](#dependency).
* Constructor sets parameters specific to the two-wire bus in general.
* All the constructor parameters can be changed dynamically with corresponding setters later in a sketch.

#### Syntax
    gbj_memory(ClockSpeeds clockSpeed, uint8_t pinSDA, uint8_t pinSCL)

#### Parameters
* **clockSpeed**: Two-wire bus clock frequency in Hertz.
  * *Valid values*: ClockSpeeds::CLOCK\_100KHZ, ClockSpeeds::CLOCK\_400KHZ
  * *Default value*: ClockSpeeds::CLOCK\_100KHZ

* **pinSDA**: Microcontroller's pin for serial data. It is not a board pin but GPIO number. For hardware two-wire bus platforms it is irrelevant and none of methods utilizes this parameter for such as platforms for communication on the bus. On the other hand, for those platforms the parameters might be utilized for storing some specific attribute in the class instance object.
  * *Valid values*: positive integer
  * *Default value*: 4 (GPIO4, D2)

* **pinSCL**: Microcontroller's pin for serial clock. It is not a board pin but GPIO number. For hardware two-wire bus platforms it is irrelevant and none of methods utilizes this parameter for such as platforms. On the other hand, for those platforms the parameters might be utilized for storing some specific attribute in the class instance object.
  * *Valid values*: positive integer
  * *Default value*: 5 (GPIO5, D1)

#### Returns
Object performing the memory management.
The constructor cannot return [a result or error code](#constants) directly, however, it stores them in the instance object. The result can be tested in the operational code with the inhereted method `getLastResult()`, `isError()`, or `isSuccess()`.

[Back to interface](#interface)


<a id="begin"></a>

## begin()

#### Description
The method sanitizes and stores input parameters to the class instance object, which determine the capacity parameters of the memory.

#### Syntax
    ResultCodes begin(uint16_t maxPosition, uint16_t pageSize, uint16_t minPosition)

#### Parameters
* **maxPosition**: Maximal real position of the memory in bytes. Usually it expresses capacity of the memory minus one, but can be less if some end part of the memory cannot be used.
  * *Valid values*: non-negative integer 0 ~ 65535
  * *Default value*: None

* **pageSize**: Size of the memory page in bytes. This is a chunk of bytes that can be written to the memory at once.
  * *Valid values*: non-negative integer 0 ~ 65535
  * *Default value*: None

* **minPosition**: Minimal real memory position where the memory storage starts in bytes. For instance, real time clock chips have own read only memory starting just after time keeping registers.
* However, memory position in all other methods is counted from 0 and considered as a logical position, i.e., position from the minimal real position.
  * *Valid values*: non-negative integer 0 ~ 65535
  * *Default value*: 0

#### Returns
Some of result or error codes.

[Back to interface](#interface)


<a id="storeStream"></a>

## storeStream()

#### Description
The method writes input data byte stream to the memory chunked by memory pages if needed.
* If length of the stored byte stream spans over memory pages, the method executes more bus transmissions, each for corresponding memory page.

#### Syntax
    ResultCodes storeStream(uint16_t position, uint8_t *dataBuffer, uint16_t dataLen)

#### Parameters
* **position**: Logical memory position where the storing should start. The input value is limited to maximal supported capacity in bytes counting from 0.
  * *Valid values*: non-negative integer 0 ~ ([getCapacityByte()](#getCapacityByte) - 1)
  * *Default value*: None

* **dataBuffer**: Pointer to the byte data buffer.
  * *Valid values*: address space
  * *Default value*: None

* **dataLen**: Number of bytes to be stored in memory.
  * *Valid values*: non-negative integer 0 ~ 65535
  * *Default value*: None

#### Returns
Some of result or error codes.

#### See also
[retrieveStream()](#retrieveStream)

[Back to interface](#interface)


<a id="retrieveStream"></a>

## retrieveStream()

#### Description
The method reads data from memory and places it to the provided data buffer. The buffer should be defined outside this library with sufficient length for desired data.

#### Syntax
    ResultCodes retrieveStream(uint16_t position, uint8_t *dataBuffer, uint16_t dataLen)

#### Parameters
* **position**: Logical memory position where the retrieving should start. The input value is limited to maximal supported capacity in bytes counting from 0.
  * *Valid values*: non-negative integer 0 ~ ([getCapacityByte()](#getCapacityByte) - 1)
  * *Default value*: None

* **dataBuffer**: Pointer to the byte data buffer.
  * *Valid values*: address space
  * *Default value*: None

* **dataLen**: Number of bytes to be retrieved from memory.
  * *Valid values*: non-negative integer 0 ~ 65535
  * *Default value*: None

#### Returns
Some of result or error codes.

#### See also
[storeStream()](#storeStream)

[Back to interface](#interface)


<a id="retrieveCurrent"></a>

## retrieveCurrent()

#### Description
The method reads recently accessed position incremented by 1.

#### Syntax
    ResultCodes retrieveCurrent(uint8_t &data)

#### Parameters
* **data**: Pointer to a referenced variable for placing read data byte.
  * *Valid values*: address space
  * *Default value*: None

#### Returns
Some of result or error codes.

[Back to interface](#interface)


<a id="store"></a>

## store()

#### Description
The method writes a value of particular data type, generic or custom, to the memory.
* The method is templated utilizing method [storeStream()](#storeStream), so that it determines data byte stream length automatically.
* The method does not need to be called by templating syntax, because it is able to identify proper data type by data type of the just storing data value parameter.

#### Syntax
    template<class T>
    ResultCodes store(uint16_t position, T data)

#### Parameters
* **position**: Logical memory position where the storing should start. The input value is limited to maximal supported capacity in bytes counting from 0.
  * *Valid values*: non-negative integer 0 ~ ([getCapacityByte()](#getCapacityByte) - 1)
  * *Default value*: None

* **data**: Value of particular data type that should be stored in the memory. If the value of particular data type needs more memory bytes than there are present from the starting position to the end of the memory, the error is returned.
  * *Valid values*: dynamic data type
  * *Default value*: None

#### Returns
Some of result or error codes.

#### See also
[retrieve()](#retrieve)

[Back to interface](#interface)


<a id="retrieve"></a>

## retrieve()

#### Description
The method reads a value of particular data type, generic or custom, from the memory.
* The method is templated utilizing method [retrieveStream()](#retrieveStream), so that it determines data byte stream length automatically.
* The method does not need to be called by templating syntax, because it is able to identify proper data type by data type of the just storing data value parameter.

#### Syntax
    template<class T>
    ResultCodes retrieve(uint16_t position, T &data)

#### Parameters
* **position**: Logical memory position where the retrieving should start. The input value is limited to maximal supported capacity in bytes counting from 0.
  * *Valid values*: non-negative integer 0 ~ ([getCapacityByte()](#getCapacityByte) - 1)
  * *Default value*: None

* **data**: Pointer to a referenced variable for placing read data of desired type. If the data type of the external variable needs more bytes than there are present from the starting position to the end of the memory, the error is returned.
  * *Valid values*: dynamic data type
  * *Default value*: None

#### Returns
Some of result or error codes.

#### See also
[store()](#store)

[Back to interface](#interface)


<a id="fill"></a>

## fill()

#### Description
The method writes input byte to defined positions in the memory.

#### Syntax
    ResultCodes fill(uint16_t position, uint16_t dataLen, uint8_t fillValue)

#### Parameters
* **position**: Logical memory position where the storing should start. The input value is limited to maximal supported capacity in bytes counting from 0.
  * *Valid values*: non-negative integer 0 ~ ([getCapacityByte()](#getCapacityByte) - 1)
  * *Default value*: None

* **dataLen**: Number of bytes to be filled in memory. If there are provided more bytes to fill from the position to the end of the memory capacity, exceeding memory positions are ignored without generating an error.
  * *Valid values*: non-negative integer 0 ~ 65535
  * *Default value*: None

* **fillValue**: Value used to filling memory.
  * *Valid values*: non-negative integer 0 ~ 255
  * *Default value*: None

#### Returns
Some of result or error codes.

#### See also
[erase()](#erase)

[Back to interface](#interface)


<a id="erase"></a>

## erase()

#### Description
The method writes byte value `0xFF` (all binary 1s) to entire memory.
* The methods utilizes the method [fill()](#fill) from 0 position with entire byte capacity of the memory while it writes memory page by page.
* For higher capacity memory the erasing can take a longer time due to paging by memory pages and two-wire buffer limited size.

#### Syntax
    ResultCodes erase()

#### Parameters
None

#### Returns
Some of result or error codes.

#### See also
[fill()](#fill)

[Back to interface](#interface)


<a id="getCapacityByte"></a>

## getCapacityByte(), getCapacityKiByte()

#### Description
The particular method provides the memory capacity either in bytes or in kibibytes.

#### Syntax
    uint32_t getCapacityByte()
    uint32_t getCapacityKiByte()

#### Parameters
None

#### Returns
Memory capacity in bytes or kibibytes.

#### See also
[getCapacityBit(), getCapacityKiBit()](#getCapacityBit)

[Back to interface](#interface)


<a id="getCapacityBit"></a>

## getCapacityBit(), getCapacityKiBit()

#### Description
The particular method provides the memory capacity either in bits or in kibibits.

#### Syntax
    uint32_t getCapacityBit()
    uint32_t getCapacityKiBit()

#### Parameters
None

#### Returns
Memory capacity in bits or kibibits.

#### See also
[getCapacityByte(), getCapacityKiByte()](#getCapacityByte)

[Back to interface](#interface)


<a id="getPageSize"></a>

## getPageSize()

#### Description
The method provides length of the memory page in bytes.

#### Syntax
    uint16_t getPageSize()

#### Parameters
None

#### Returns
Memory page length in bytes.

#### See also
[getPages()](#getPages)

[Back to interface](#interface)


<a id="getPages"></a>

## getPages()

#### Description
The method provides a number of available memory pages.

#### Syntax
    uint16_t getPages()

#### Parameters
None

#### Returns
Number of memory pages.

#### See also
[getPageSize()](#getPageSize)

[Back to interface](#interface)


<a id="getPositionReal"></a>

## getPositionReal()

#### Description
The method provides real (physical) memory position calculated from the logical one.

#### Syntax
    uint16_t getPositionReal()

#### Parameters
None

#### Returns
Number of memory pages.

[Back to interface](#interface)


<a id="getPositionIn"></a>

## getPositionInBytes(), getPositionInWords()

#### Description
The particular method provides an internal flag whether just byte or word should be used for memory addressing.

#### Syntax
    bool getPositionInBytes()
    bool getPositionInWords()

#### Parameters
None

#### Returns
Logical flag for byte or word addressing of positions.

#### See also
[setPositionInBytes(), setPositionInWords()](#setPositionIn)

[Back to interface](#interface)


<a id="setPositionIn"></a>

## setPositionInBytes(), setPositionInWords()

#### Description
The particular method sets an internal flag whether just byte or entire word of provided memory position should be used for memory addressing.
* Byte addressing ensures, that just the least significant byte is really transmits to the two-wire bus from provided two bytes of a memory position.
* Word addressing ensures, that all two bytes of provided memory position are transmitted to the two-wire bus even if the most significant byte of it is zero.

#### Syntax
    void setPositionInBytes()
    void setPositionInWords()

#### Parameters
None

#### Returns
None

#### See also
[getPositionInBytes(), getPositionInWords()](#getPositionIn)

[Back to interface](#interface)
