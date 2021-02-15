

#include <iostream>
#include <string.h>
#include <string>
#include "student.h"
#include "utility.h"

using namespace std;

bool Student::operator==(const Student &obj)
{
	return (m_id == obj.m_id) && (m_name == obj.m_name) && (m_phoneNumber == obj.m_phoneNumber);
}

Serializer& Student::operator<<(Serializer& out) 
{ 
    out << m_name << m_id << m_phoneNumber;
    return out;
}

Serializer& Student::operator>>(Serializer& in) 
{ 
    in >> m_name >> m_id >> m_phoneNumber;
    return in; 
}


Serializer& Point::operator<<(Serializer& out) 
{ 
    out << m_x << m_y; 
    return out;
}

Serializer& Point::operator>>(Serializer& in) 
{ 
    in >> m_x >> m_y; 
    return in; 
}

Serializer& operator<<(Serializer& out, Student& value) {  return value.operator<<(out); }
Serializer& operator>>(Serializer& in, Student& value) {  return value.operator>>(in); }
Serializer& operator<<(Serializer& out, Point& value) {  return value.operator<<(out); }
Serializer& operator>>(Serializer& in, Point& value) {  return value.operator>>(in); }

UINT8 *Student::serializeToBuffer(int &len)
{
  	Serializer out;
    this->operator<<(out);
    std::vector<UINT8> buffer;
	out.saveToVector(buffer);

    len = buffer.size();
    UINT8 *pData = new UINT8[len];
    memcpy(pData, buffer.data(), len);
    return (pData);
}

void Student::serializeFromBuffer(UINT8 * pucData, int len)
{  	
    Serializer in;
    in.loadFromBuffer(pucData, len);
    this->operator>>(in);
}