#include <string.h>
#include <algorithm>

#include "base/CircularBuffer.h"



namespace Base {

CCircularBuffer::CCircularBuffer(size_t capacity)
    : m_beg_index(0)
    , m_end_index(0)
    , m_size(0)
    , m_capacity(capacity)
{
    m_data = new char[capacity];
}

CCircularBuffer::~CCircularBuffer()
{
    delete [] m_data;
}

size_t CCircularBuffer::Write(const char *data, size_t bytes)
{
    size_t capacity = m_capacity;
    size_t bytes_to_write = std::min(bytes, capacity - m_size);

    if(bytes_to_write == 0) {
        return 0;
    }

    // Write in a single step
    if (bytes_to_write <= capacity - m_end_index) {
        memcpy(m_data + m_end_index, data, bytes_to_write);
        m_end_index += bytes_to_write;
        if (m_end_index == capacity) {
            m_end_index = 0;
        }
    } else { // Write in two steps
        size_t size_1 = capacity - m_end_index;
        memcpy(m_data + m_end_index, data, size_1);
        size_t size_2 = bytes_to_write - size_1;
        memcpy(m_data, data + size_1, size_2);
        m_end_index = size_2;
    }

    m_size += bytes_to_write;

    return bytes_to_write;
}

size_t CCircularBuffer::Read(char *data, size_t bytes)
{
    size_t capacity = m_capacity;
    size_t bytes_to_read = std::min(bytes, m_size);

    if(bytes_to_read == 0) {
        return 0;
    }

    // Read in a single step
    if (bytes_to_read <= capacity - m_beg_index) {
        memcpy(data, m_data + m_beg_index, bytes_to_read);
        m_beg_index += bytes_to_read;
        if (m_beg_index == capacity) {
            m_beg_index = 0;
        }
    } else { // Read in two steps
        size_t size_1 = capacity - m_beg_index;
        memcpy(data, m_data + m_beg_index, size_1);
        size_t size_2 = bytes_to_read - size_1;
        memcpy(data + size_1, m_data, size_2);
        m_beg_index = size_2;
    }

    m_size -= bytes_to_read;

    return bytes_to_read;
}

} // end namespace Base


