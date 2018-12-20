#include "gbj_memory.h"
const String gbj_memory::VERSION = "GBJ_MEMORY 1.0.0";


uint8_t gbj_memory::begin(uint16_t maxPosition, uint16_t pageSize, uint16_t minPosition)
{
  // Sanitize
  _memoryStatus.minPosition = min(minPosition, maxPosition);
  _memoryStatus.maxPosition = maxPosition - _memoryStatus.minPosition;
  _memoryStatus.pageSize = max(pageSize, 1);
  if (gbj_twowire::begin()) return getLastResult();
  return getLastResult();
}


uint8_t gbj_memory::checkPosition(uint16_t position, uint16_t dataLen)
{
  initLastResult();
  if (dataLen == 0 || _memoryStatus.maxPosition < (position + dataLen)) \
    return setLastResult(ERROR_POSITION);
  return getLastResult();
}


uint8_t gbj_memory::storeStream(uint16_t position, uint8_t *dataBuffer, uint16_t dataLen)
{
  uint16_t realPosition = getPositionReal(position);
  if (checkPosition(position, dataLen)) return getLastResult();
  while (dataLen)
  {
    uint8_t pageLen = min(dataLen, _memoryStatus.pageSize - position % _memoryStatus.pageSize);
    if (busSendStreamPrefixed(dataBuffer, dataLen, false, \
      (uint8_t *)&realPosition, getPositionInBytes() ? 1 : 2, true, true)) return getLastResult();
    dataLen -= pageLen;
    dataBuffer += pageLen;
    position += pageLen;
  }
  return getLastResult();
}


uint8_t gbj_memory::retrieveStream(uint16_t position, uint8_t *dataBuffer, uint16_t dataLen)
{
  uint16_t realPosition = getPositionReal(position);
  if (checkPosition(position, dataLen)) return getLastResult();
  setBusRpte();
  if (busSendStream((uint8_t*)&realPosition, getPositionInBytes() ? 1 : 2, true)) return getLastResult();
  setBusStop();
  if (busReceive((uint8_t *)dataBuffer, dataLen)) return getLastResult();
  return getLastResult();
}


uint8_t gbj_memory::fill(uint16_t position, uint16_t dataLen, uint8_t fillValue)
{
  // Sanitize
  dataLen = min(dataLen, getCapacityByte() - position);
  if (checkPosition(position, dataLen)) return getLastResult();
  // Store
  uint8_t dataBuffer[dataLen];
  for (uint8_t i = 0; i < dataLen; i++) dataBuffer[i] = fillValue;
  if(storeStream(position, dataBuffer, dataLen)) return getLastResult();
  return getLastResult();
}


uint8_t gbj_memory::erase()
{
  uint16_t pages = getPages();
  uint16_t position = 0;
  while(pages--)
  {
    if (fill(position, getPageSize(), 0xFF)) return getLastResult();
    position += getPageSize();
  }
  return getLastResult();
}


uint8_t gbj_memory::retrieveCurrent(uint8_t &data)
{
  uint8_t *dataBuffer = &data;
  if (busReceive((uint8_t *)dataBuffer, 1)) return getLastResult();
  return getLastResult();
}
