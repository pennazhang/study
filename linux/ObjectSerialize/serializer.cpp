
#include <linux/unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include "utility.h"
#include "serializer.h"


using namespace std;

#if 0
Serializer & Serializer::operator << (UINT8 value)
{
    m_stringStream.write((char *)&value, sizeof(value));
    return (*this);
}

Serializer & Serializer::operator >> (UINT8 & value)
{
    m_stringStream.read((char *)&value, sizeof(value));
    return (*this);
}

Serializer & Serializer::operator << (INT8 value)
{
    m_stringStream.write((char *)&value, sizeof(value));
    return (*this);
}

Serializer & Serializer::operator >> (INT8 & value)
{
    m_stringStream.read((char *)&value, sizeof(value));
    return (*this);
}














Serializer & Serializer::operator << (UINT16 value)
{
    m_stringStream.write((char *)&value, sizeof(value));
    return (*this);
}

Serializer & Serializer::operator >> (UINT16 & value)
{
    m_stringStream.read((char *)&value, sizeof(value));
    return (*this);
}

Serializer & Serializer::operator << (INT16 value)
{
    m_stringStream.write((char *)&value, sizeof(value));
    return (*this);
}

Serializer & Serializer::operator >> (INT16 & value)
{
    m_stringStream.read((char *)&value, sizeof(value));
    return (*this);
}


Serializer & Serializer::operator >> (int& value)
{
    m_stringStream.read((char *)&value, sizeof(value));
    return (*this);
}

Serializer & Serializer::operator >> (int& value)
{
    m_stringStream.read((char *)&value, sizeof(value));
    return (*this);
}

Serializer & Serializer::operator >> (int& value)
{
    m_stringStream.read((char *)&value, sizeof(value));
    return (*this);
}

#endif

void Serializer::saveToVector(std::vector<unsigned char> & buffer)
{
    m_stringStream.seekp(0, std::ios::end);
    int len = m_stringStream.tellp();
    cout << "len = " << len << endl;
    buffer.resize(len);
    m_stringStream.seekg(0, std::ios::beg);
    m_stringStream.read((char *)buffer.data(), len);
}

void Serializer::loadFromVector(std::vector<unsigned char> & buffer)
{
//    m_stringStream.seekp(0, std::ios::beg);
    m_stringStream.write((char *)buffer.data(), buffer.size());
}
#if 0
Serializer & operator << (Serializer & ar, int value)
{
    ar.m_stringStream.write((char *)&value, sizeof(value));
    return (ar);
}

Serializer & operator >> (Serializer & ar, int &value)
{
    ar.m_stringStream.read((char *)&value, sizeof(value));
    return (ar);
}
#endif

Serializer& operator<<(Serializer& out, const char * value)    
{ 
    int length = strlen(value);
    out.m_stringStream.write((char *)&length, sizeof(length));
    if (length != 0)
    {
        out.m_stringStream.write(value, length);
    }
    return out; 
}

Serializer& operator<<(Serializer& out, const std::string& value)
{
    return operator<<(out, value.c_str());
}

Serializer& operator>>(Serializer& in, std::string& value)
{
    int length;
    in.m_stringStream.read((char *)&length, sizeof(length));
    if (length == 0)
    {
        value.clear();
    }
    else
    {
        value.resize(length);
        char *buffer = new char(length + 1);
        in.m_stringStream.read(buffer, length);
        buffer[length] = 0;
        value = std::string(buffer);
        delete []buffer;
    }
    return in; 
}

Serializer& operator<<(Serializer& out, float value)
{ 
    return operator<<(out, (double)value); 
}

Serializer& operator>>(Serializer& in, float& value)   
{
    double value1;
    operator>>(in, value1);
    return (in);
}
