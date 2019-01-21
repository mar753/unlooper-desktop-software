//rs.h
//klasa do komunikacji przez port RS232 (WINAPI)
//v1.0
//Marcel Szewczyk

#ifndef _RS_H_
#define _RS_H_

#include <windows.h> 

class RS{
	DCB portDCB; 
	COMMTIMEOUTS portCommTimeouts; 
	HANDLE portHandle; 

public:	
	RS();
	int RSSetup(char *port = "COM1", int baud = 9600, int bitSize = 8, int stopBits = 1, int parity = NOPARITY); 
	int RSWrite(unsigned char*, int);
	int RSRead(unsigned char*, int);
	void RSClose();
	int RSPurge();
	void RSSetDTR();
	void RSClearDTR();
	void RSSetRTS();
	void RSClearRTS();
};


#endif
