#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__


namespace Base {

class CCircularBuffer
{
public:
  	CCircularBuffer(size_t capacity);
  	~CCircularBuffer();

  	size_t Size() const { return m_size; }
  	size_t Capacity() const { return m_capacity; }
  	// Return number of bytes written.
  	size_t Write(const char *data, size_t bytes);
  	// Return number of bytes read.
  	size_t Read(char *data, size_t bytes);

private:
  	size_t 	 m_beg_index;
  	size_t 	 m_end_index;
  	size_t 	 m_size;
  	size_t 	 m_capacity;
  	char 	   *m_data;
};

} // end namespace Base

#endif