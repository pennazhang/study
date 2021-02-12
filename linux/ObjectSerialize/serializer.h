
#pragma once

#include <sstream>      // std::ostringstream
#include <vector>

/**************************************************************************************
	Serializer ar1, ar2;
	std::vector<UINT8> buffer;
	int a5, a6;

	ar1 << 5 << 6;
	ar1.saveToVector(buffer);

	ar2.loadFromVector(buffer);
	ar2 >> a5 >> a6;

	cout << "a5 = " << a5 << " a6 = " << a6 << endl;

=>
saveToVector ok
a5 = 5 a6 = 6    
*****************************************************************************************/
class Serializer
{
public:
    Serializer() {};
//    Serializer& operator >> (int& value);
//    Serializer& operator << (int value);
    void saveToVector(std::vector<unsigned char> & buffer);
    void loadFromVector(std::vector<unsigned char> & buffer);

    friend Serializer& operator<<(Serializer& out, UINT8 value)    { out.m_stringStream.write((char *)&value, sizeof(value));  return out; }
    friend Serializer& operator>>(Serializer& in, UINT8& value)   { in.m_stringStream.read((char *)&value, sizeof(value));  return in; }
    friend Serializer& operator<<(Serializer& out, UINT16 value)    { out.m_stringStream.write((char *)&value, sizeof(value));  return out; }
    friend Serializer& operator>>(Serializer& in, UINT16& value)   { in.m_stringStream.read((char *)&value, sizeof(value));  return in; }
    friend Serializer& operator<<(Serializer& out, UINT32 value)    { out.m_stringStream.write((char *)&value, sizeof(value));  return out; }
    friend Serializer& operator>>(Serializer& in, UINT32& value)   { in.m_stringStream.read((char *)&value, sizeof(value));  return in; }
    friend Serializer& operator<<(Serializer& out, UINT64 value)    { out.m_stringStream.write((char *)&value, sizeof(value));  return out; }
    friend Serializer& operator>>(Serializer& in, UINT64& value)   { in.m_stringStream.read((char *)&value, sizeof(value));  return in; }
    friend Serializer& operator<<(Serializer& out, INT8 value)    { out.m_stringStream.write((char *)&value, sizeof(value));  return out; }
    friend Serializer& operator>>(Serializer& in, INT8& value)   { in.m_stringStream.read((char *)&value, sizeof(value));  return in; }
    friend Serializer& operator<<(Serializer& out, INT16 value)    { out.m_stringStream.write((char *)&value, sizeof(value));  return out; }
    friend Serializer& operator>>(Serializer& in, INT16& value)   { in.m_stringStream.read((char *)&value, sizeof(value));  return in; }
    friend Serializer& operator<<(Serializer& out, INT32 value)    { out.m_stringStream.write((char *)&value, sizeof(value));  return out; }
    friend Serializer& operator>>(Serializer& in, INT32& value)   { in.m_stringStream.read((char *)&value, sizeof(value));  return in; }
    friend Serializer& operator<<(Serializer& out, INT64 value)    { out.m_stringStream.write((char *)&value, sizeof(value));  return out; }
    friend Serializer& operator>>(Serializer& in, INT64& value)   { in.m_stringStream.read((char *)&value, sizeof(value));  return in; }
    friend Serializer& operator<<(Serializer& out, double value)    { out.m_stringStream.write((char *)&value, sizeof(value));  return out; }
    friend Serializer& operator>>(Serializer& in, double& value)   { in.m_stringStream.read((char *)&value, sizeof(value));  return in; }



#if 0
    template<typename NType>
    friend Serializer& operator<<(Serializer& out, NType& value)
    {
        value.operator<<(out);
        return out; 
    }

    template<typename NType>
    friend Serializer& operator>>(Serializer& out, NType &value)
    {
        value.operator>>(out);
        return out; 
    }
#endif    
public:
    std::stringstream m_stringStream;
};

    Serializer& operator<<(Serializer& out, float value);
    Serializer& operator>>(Serializer& in, float& value); 
    Serializer& operator<<(Serializer& out, const char * value);
    Serializer& operator<<(Serializer& out, const std::string& value);
    Serializer& operator>>(Serializer& in, std::string& value);


//Serializer & operator << (Serializer & ar, int value);
//Serializer & operator >> (Serializer & ar, int &value);


