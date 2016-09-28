#include <algorithm> // for std::min

#include "CircularBuffer.h"



namespace Base {

CircularBuffer::CircularBuffer(size_t capacity)
: m_beg_index(0)
, m_end_index(0)
, size_(0)
, capacity_(capacity)
{
	data_ = new char[capacity];
}

CircularBuffer::~CircularBuffer()
{
	delete [] data_;
}

size_t CircularBuffer::write(const char *data, size_t bytes)
{
	if (bytes == 0) 
		return 0;

	size_t capacity = capacity_;
	size_t bytes_to_write = std::min(bytes, capacity - size_);

	// Write in a single step
	if (bytes_to_write <= capacity - m_end_index) {
    	memcpy(data_ + m_end_index, data, bytes_to_write);
    	m_end_index += bytes_to_write;
    	if (m_end_index == capacity) {
      		m_end_index = 0;
		}
  	} else { // Write in two steps
    	size_t size_1 = capacity - m_end_index;
    	memcpy(data_ + m_end_index, data, size_1);
    	size_t size_2 = bytes_to_write - size_1;
    	memcpy(data_, data + size_1, size_2);
    	m_end_index = size_2;
  	}

  	size_ += bytes_to_write;

  	return bytes_to_write;
}

size_t CircularBuffer::read(char *data, size_t bytes)
{
  	if (bytes == 0)
    	return 0;

  	size_t capacity = capacity_;
  	size_t bytes_to_read = std::min(bytes, size_);

  	// Read in a single step
  	if (bytes_to_read <= capacity - m_beg_index) {
    	memcpy(data, data_ + m_beg_index, bytes_to_read);
    	m_beg_index += bytes_to_read;
    	if (m_beg_index == capacity) {
      		m_beg_index = 0;
		}
  	} else { // Read in two steps
    	size_t size_1 = capacity - m_beg_index;
    	memcpy(data, data_ + m_beg_index, size_1);
    	size_t size_2 = bytes_to_read - size_1;
    	memcpy(data + size_1, data_, size_2);
    	m_beg_index = size_2;
  	}

  	size_ -= bytes_to_read;

  	return bytes_to_read;
}

} // end namespace Base


