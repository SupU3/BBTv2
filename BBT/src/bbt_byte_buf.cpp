#include "bbt_byte_buf.h"
#include "bbt_assert.h"

#include <algorithm>

#include <stdlib.h>

void bbt::byteBuffer::release()
{
  if (data)
    free(data);
}

void bbt::byteBuffer::copy(const void* const indata, size_t inlen, size_t inoff)
{
  // Debug sanity checks.
  BBT_ASSERT( data,                   "null data ptr");
  BBT_ASSERT( indata,                 "null indata ptr");
  BBT_ASSERT( (inoff + inlen) <= len,  "off is greater than len");

  // Copy at internal data ptr, in data with offset.
  memcpy( data, (unsigned char*)indata + inoff, inlen );
}

void bbt::byteBuffer::resize(size_t newlen)
{
  unsigned char* tmpBuf = (unsigned char*)malloc( newlen );

  BBT_ASSERT(tmpBuf, "Failed mem allocation");

  size_t cpyLen = std::min( len, newlen );

  if (data)
    memcpy(tmpBuf, data, cpyLen);

  release();

  data = tmpBuf;
  len = newlen;
}

void bbt::byteBuffer::append(const void* const indata, size_t inlen)
{
  size_t oldLen = len;
  resize(len + inlen);
  memcpy(data + oldLen, indata, inlen);
}

void bbt::byteBuffer::reserve(size_t newLen)
{
  data = (unsigned char*)malloc(newLen);
  len = newLen;
}
