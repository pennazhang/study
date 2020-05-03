#ifndef _MAIN_H
#define _MAIN_H

#include <QThread>
#include <QDataStream>

class People
{
public:
	People() {};
	~People() {};
	People(int id, const QString & name, int age) : m_id(id), m_strName(name), m_age(age) {};
	virtual QDataStream & operator <<(QDataStream &output);
	virtual QDataStream & operator >>(QDataStream &input);

public:
	friend QDataStream &operator<<(QDataStream & output, People &data) { return (data.operator<<(output)); };
	friend QDataStream &operator>>(QDataStream & input, People &data) { return (data.operator>>(input)); };
	virtual void show() = 0;

public:
	int m_id;
	QString m_strName;
	int m_age;
};

class Student: public People
{
public:
	Student() {};
	Student(int id, const QString name, int age, int score) : People(id, name, age), m_averageScore(score) {};
	virtual QDataStream & operator <<(QDataStream &output);
	virtual QDataStream & operator >>(QDataStream &input);

public:
	friend QDataStream &operator<<(QDataStream & output, Student &data) { return (data.operator<<(output)); };
	friend QDataStream &operator>>(QDataStream & input, Student &data) { return (data.operator>>(input)); };
	virtual void show();

public:
	int m_averageScore;
};

#endif
