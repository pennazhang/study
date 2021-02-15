
#pragma once

#include "typedef.h"
#include <iostream>
#include "serializer.h"

class Student : public Serializable
{
public:
    std::string m_name;
    int m_id;
    std::string m_phoneNumber;
    Student(int id = 0, std::string name = "", std::string phone = "") : m_id(id), m_name(name), m_phoneNumber(phone) { };
//    Student(const Student & obj);
    bool operator==(const Student & obj);
    
    Serializer& operator<<(Serializer& out); 
    Serializer& operator>>(Serializer& in);
};
Serializer& operator<<(Serializer& out, Student& value);
Serializer& operator>>(Serializer& in, Student& value);

class Point
{
public:
    Point(int x = 0, int y = 0) : m_x(x), m_y(y) { };
    Serializer& operator<<(Serializer& out); 
    Serializer& operator>>(Serializer& in);

public: 
    int m_x, m_y;
};

Serializer& operator<<(Serializer& out, Point& value);
Serializer& operator>>(Serializer& in, Point& value);
