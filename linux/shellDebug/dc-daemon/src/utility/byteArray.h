
#pragma once

#include "serializer.h"

// This is the simplest class that used as QByteArray 
// This class is based on Serializable, so it can be serialized.
//
class ByteArray : public Serializable
{
public:
    virtual ~ByteArray();
    ByteArray(int size = 0, UINT8 *pucData = NULL);
    ByteArray(ByteArray& byteArray);
    void resize(int size);
    UINT8 *data() { return m_data; }
    int size() { return m_size; }
    virtual Serializer& operator<<(Serializer& out);
    virtual Serializer& operator>>(Serializer& in);
    ByteArray& operator=(ByteArray& byteArray);
    void clear();

protected:
    UINT8 *m_data;
    int m_size;
};

Serializer& operator<<(Serializer& out, ByteArray& value);
Serializer& operator>>(Serializer& in, ByteArray& value);
