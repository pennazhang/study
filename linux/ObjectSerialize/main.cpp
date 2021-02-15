#include <iostream>
#include <chrono>
#include <unistd.h>
#include "student.h"
#include <fstream>
#include <cassert>
#include <sstream>      // std::ostringstream
#include "utility.h"
#include "config.h"
#include "string.h"
#include "serializer.h"

using namespace std;

void test()
{
	std::stringstream foo;                            // out
	foo << "goodluck: " << 2021;
	cout << "size = " << foo.tellp() << endl;
//	char lpszBuffer[] = "May I help you?";
	int n = 0x55aa55aa;
	foo.write((char *)&n, sizeof(n));
	cout << "size = " << foo.tellp() << endl;
	foo << 2021;
	int len = foo.tellp();
	cout << "size = " << len << endl;

	UINT8 *pucData = new UINT8[len];
	memset(pucData, 0, len);
//	foo.seekg(0);
//	foo.read((char *)pucData, len);

	printf("goodluck\n");
	dumpDecimal(pucData, len, 1, 32);
	delete pucData;

	logInfo("goodluck");


	Serializer ar1;
	INT8 a1 = -1, a2 = -2;
	std::string value1("goodluck"), value2, value3;
	Point p1(3, 4), p2;
//	ar1 << a1 << a2 << (float)2.1 << "how old are you?"  << value1;
	ar1 << a1 << a2 << (float)2.1 << "how old are you?"  << value1 << p1;
	cout << "<< ok" << endl;

	std::vector<UINT8> buffer;
	ar1.saveToVector(buffer);

	cout << "saveToVector ok" << endl;
	
	Serializer ar2;
	ar2.loadFromVector(buffer);
	UINT8 a5, a6;
	double d1;
//	ar2 >> a5 >> a6 >> d1 >> value2 >> value3;
	ar2 >> a5 >> a6 >> d1 >> value2 >> value3 >> p2;



	cout << "a5 = " << (UINT32)a5 << ", a6 = " << (UINT32)a6 << ", d1 = " << d1 << ", value2 = " << value2 << ", value3 = " << value3 << endl;
	cout << "p2 = (" << p2.m_x << ", " << p2.m_y << ")" << endl;
}

int main()
{
	cout << "godluck" << endl;
	logInfo("goodluck");
	test();

	Student stud1(1, "Jack", "4445554455");
	Student stud2(4, "ddd", "4445511111");
	Student stud3(6, "Aadi", "aaa");

	// Open the File
	Serializer out, in;

	// Write objects to file
	out << stud1 << stud2 << stud3;

	std::vector<UINT8> buffer;
	out.saveToVector(buffer);
	in.loadFromVector(buffer);

	Student student1;
	Student student2;
	Student student3;

	// Read objects from file and fill in data
	in >> student1 >> student2 >> student3;

	// Compare the Objects
	assert(stud1 == student1);
	assert(stud2 == student2);
	assert(stud3 == student3);

	Student liMing(3, "Liming", "00998877"), someBody;
	int bufferLen;

//	for (int i = 0; i < 10000000; i++)
	{
		UINT8 *pData = liMing.serializeToBuffer(bufferLen);
		someBody.serializeFromBuffer(pData, bufferLen);
		dumpHex(pData, bufferLen, 1);
		assert(liMing == someBody);
		delete pData;
	}
	return 0;
}
