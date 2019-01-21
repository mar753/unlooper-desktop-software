//unlooper.h
//klasa do komunikacji z 'unlooperem' (zadajnikiem)
//v1.0
//Marcel Szewczyk

#ifndef _UNLOOPER_H_
#define _UNLOOPER_H_

#include <fstream>
#include <iostream>
#include "rs.h"

using namespace std;

class Unlooper{
	RS com1;
	RS com2;
	int error;

public:	
	Unlooper();
	int status();
	void resetUnlooper();
	int sendByte(unsigned char);
	int receiveByte(unsigned char*);
	void closePorts();
	int runTest(int number = 1, int log = 0, int glitches = 1, int second = 1, int third = 1);	
};

#endif
