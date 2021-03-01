
#include "byteArray.h"
#include <string.h> // memcpy

ByteArray::ByteArray(int size, UINT8 *pucData) 
{
    if (size <= 0)
    {
        m_size = 0;
        m_data = NULL;
    }
    else
    {
        m_size = size;
        m_data = new UINT8[m_size];
        
        if (pucData != NULL)
        {
            memcpy(m_data, pucData, m_size);
        }
    }
}

ByteArray::ByteArray(ByteArray& byteArray)
{
    ByteArray(byteArray.size(), byteArray.data());
}


void ByteArray::resize(int size) 
{
    clear();
    if (size > 0)
    {
        m_data = new UINT8[size];
        m_size = size;
    }
}

void ByteArray::clear()
{
    if (m_data != NULL)
    {
        delete []m_data;
    }
    m_data = NULL;
    m_size = 0;
}

ByteArray::~ByteArray() 
{
    clear();
}

ByteArray& ByteArray::operator=(ByteArray& byteArray) 
{
    clear();

    m_size = byteArray.size();
    if (m_size > 0)
    {
        m_data = new UINT8[m_size];
        memcpy(m_data, byteArray.data(), m_size);
    }

    return (*this);
}

Serializer& ByteArray::operator<<(Serializer& out) 
{ 
    out << m_size;
    for (int i = 0; i < m_size; i++)
    {
        out << m_data[i];
    } 
    return out; 
}

Serializer& ByteArray::operator>>(Serializer& in) 
{  
    clear();

    in >> m_size;
    ASSERT(m_size >= 0);
    if (m_size > 0)
    {
        m_data = new UINT8[m_size];
        for (int i = 0; i < m_size; i++)
        {
            in >> m_data[i];
        }
    }
    return in;
}

Serializer& operator<<(Serializer& out, ByteArray& value) {  return value.operator<<(out); }
Serializer& operator>>(Serializer& in, ByteArray& value) {  return value.operator>>(in); }
