//rs.cpp
//klasa do komunikacji przez port RS232 (WINAPI)
//v1.0
//Marcel Szewczyk

#include "rs.h" 

RS::RS(){
	portHandle = NULL;
}

//************************************************************************************

//otwarcie portu o nazwie 'port' oraz konfiguracja
int RS::RSSetup(char *port, int baud, int bitSize, int stopBits, int parity){ 

	//otworz port na uchwycie portHandle
	portHandle = CreateFile(
		TEXT(port),                  //nazwa
		GENERIC_WRITE | GENERIC_READ,   //do zapisu i odczytu
		0,                                  
		NULL,                              
		OPEN_EXISTING,                      
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, //operacje zapisu i odczytu bez buforowania                    
		NULL);                              

	//blad otwarcia portu            
	if (portHandle == INVALID_HANDLE_VALUE)  
		return -1; 

	//pobierz ustawienia
	GetCommState(portHandle, &portDCB); 
	//zmien ustawienia
	portDCB.BaudRate = baud;              // szybkosc 
	portDCB.ByteSize = bitSize;           // ilosc bitow
	portDCB.Parity = parity; 		//parzystosc
	if(stopBits == 1)           //bity stopu
		portDCB.StopBits = ONESTOPBIT; 
	else if(stopBits == 2)
		portDCB.StopBits = TWOSTOPBITS;
	portDCB.fNull = 0;                 //zezwol na odbieranie NULL

	//zapisz nowa konfiguracje
	if (!SetCommState (portHandle, &portDCB)){
		CloseHandle(portHandle);    
		return -1; 
	} 

	//pobierz timeout-y operacji zapisu i odczytu dla portu o uchwycie 'portHandle' i wyzeruj
	GetCommTimeouts(portHandle, &portCommTimeouts); 
	memset(&portCommTimeouts, 0, sizeof(portCommTimeouts)); 

	// ustaw nowe czasy
	portCommTimeouts.ReadIntervalTimeout = 10; 
	portCommTimeouts.ReadTotalTimeoutConstant = 10; 
	portCommTimeouts.WriteTotalTimeoutConstant = 10;      

	// zapisz timeout-y zeby operacje zapisu i odczytu do portu nie byly blokujace
	if (!SetCommTimeouts (portHandle, &portCommTimeouts)){ 
		CloseHandle(portHandle);  
		return -1; 
	} 

	return RSPurge(); //na koniec wyczyszczenie buforow
} 

//************************************************************************************

//wyslij do portu
int RS::RSWrite(unsigned char *buf, int len){ 

	DWORD numberOfBytesWritten; 

	WriteFile(
		portHandle,            //uchwyt portu
		buf,               //wskaznik na dane do wyslania
		len,               //ilosc bajtow do wyslania
		&numberOfBytesWritten,   //ilosc bajtow wyslanych (wskaznik)
		NULL);      

	if(numberOfBytesWritten > 0) 
		return static_cast<int>(numberOfBytesWritten);

	else return -1;    
} 

//************************************************************************************

//odczytaj z portu
int RS::RSRead(unsigned char *buf, int len){ 

	DWORD numberOfBytesRead; 

	ReadFile(
		portHandle,      //uchwyt  portu
		buf,      //wskaznik na bufor gdzie beda zapisane dane
		len,      //ilosc bajtow do odczytu
		&numberOfBytesRead,   //ilosc odczytanych bajtow 
		NULL); 

	if(numberOfBytesRead > 0)            //zwroc ilosc odczytanych bajtow
		return static_cast<int>(numberOfBytesRead);   

	else return -1;
} 

//************************************************************************************

//zamknij port
void RS::RSClose(){ 
	CloseHandle(portHandle); 
}

//************************************************************************************

//czyszczenie buforow portu 
int RS::RSPurge(){
	if(PurgeComm(portHandle, PURGE_TXCLEAR | PURGE_RXCLEAR)==0){ 
		CloseHandle(portHandle); 
		return -1; 
	} 

	return 0;
}

//************************************************************************************

//ustaw linie DTR portu 
void RS::RSSetDTR(){
	EscapeCommFunction(portHandle, SETDTR);
}

//************************************************************************************

//zeruj linie DTR portu 
void RS::RSClearDTR(){
	EscapeCommFunction(portHandle, CLRDTR);
}

//************************************************************************************

//ustaw linie RTS portu
void RS::RSSetRTS(){
	EscapeCommFunction(portHandle, SETRTS);	
}

//************************************************************************************

//zeruj linie RTS portu
void RS::RSClearRTS(){
	EscapeCommFunction(portHandle, CLRRTS);
}


