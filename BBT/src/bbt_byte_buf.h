#ifndef __BBT_BYTE_BUFFER_H__
#define __BBT_BYTE_BUFFER_H__ 1

namespace bbt
{

  class byteBuffer
  {

  public:

    // Default constructor.
    byteBuffer()
      : data(nullptr), len(0)
    {}

    ~byteBuffer()
    {
      release();
    }

    void release();

    void copy   ( const void* const data, size_t len, size_t off = 0 );
    void resize ( size_t newlen );
    void append ( const void* const indata, size_t inlen );

    size_t          Len()   noexcept        { return len; }
    size_t          Len()   const noexcept  { return len; }
    unsigned char*  Data()  noexcept        { return data; }
    unsigned char*  Data()  const noexcept  { return data; }

    const unsigned char* const Data_C() const noexcept { return data; }

  private:

    void reserve(size_t newLen);

    // Pointer to byteArray data.
    unsigned char*  data;
    size_t          len;
  };

}

#endif//__BBT_BYTE_BUFFER_H__