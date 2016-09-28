#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__


namespace Base {

class CircularBuffer
{
public:
  	CircularBuffer(size_t capacity);
  	~CircularBuffer();

  	size_t size() const { return m_size; }
  	size_t capacity() const { return m_capacity; }
  	// Return number of bytes written.
  	size_t write(const char *data, size_t bytes);
  	// Return number of bytes read.
  	size_t read(char *data, size_t bytes);

private:
  	size_t 	m_beg_index;
  	size_t 	m_end_index;
  	size_t 	m_size;
  	size_t 	m_capacity;
  	char 	*data_;
};


#endif