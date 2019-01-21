//unlooper.cpp
//klasa do komunikacji z 'unlooperem' (zadajnikiem)
//v1.0
//Marcel Szewczyk

#include "unlooper.h"

Unlooper::Unlooper(){
	unsigned char c;
	error = 0;

	cout<<"PL:\nProgram do komunikacji z zadajnikiem oraz przeprowadzania zautomatyzowanych testow dla roznych napiec zasilajacych atakowanego ukladu."
		<<" Kolejne napiecia wyszczegolnione sa w pliku konfiguracyjnym 'test.cfg' w drugiej linii, pierwsza to ilosc tych wartosci.\nEN:\n"
		<< "Unlooper communication program with possibility of carrying out automated tests for different target of attack supply voltages. "
		<<"Consecutive voltages are listed in 'test.cfg' config file within second line, first line includes quantity of these values.\n\nv1.0\nMarcel Szewczyk\n---\n\n";

	cout<<"Unlooper: COM1\nFeedback from target: COM2\n\n";
	if(com1.RSSetup(const_cast<char*>("\\\\.\\COM1"), 9600, 8, 1,0) < 0){
		cout << "\nError while opening COM1\n"; 
		error = 1; 
		return;		 		
	}

	//resetuj unlooper		
	resetUnlooper();

	//opoznienie 100 ms
	Sleep(100);

	cout << "Received welcome message (hex & ASCII):\n";	//odbieranie wiadomosci powitalnej	
	while(com1.RSRead(&c, 1) >= 0){
		cout << hex << static_cast<unsigned>(c) << "\t";
		if((c > 31) && (c < 126))
			cout << c << endl;
		else
			cout << ".\n";
	}

	c = 1; //ustaw napiecie badanego ukladu na 5V
	com1.RSWrite(&c, 1);
	Sleep(100);		
	com1.RSRead(&c, 1);

	if(com2.RSSetup(const_cast<char*>("\\\\.\\COM2"), 9600, 8, 1,0) < 0){
		cout << "\nError while opening COM2\n"; 
		error = 1;
	}
}

int Unlooper::status(){
	return error;
}

void Unlooper::resetUnlooper(){
	com1.RSSetDTR();
	com1.RSClearDTR();	
}

int Unlooper::sendByte(unsigned char byte){
	return com1.RSWrite(&byte, 1);	
}

int Unlooper::receiveByte(unsigned char* byte){
	return com1.RSRead(byte, 1);
}

void Unlooper::closePorts(){
	com1.RSClose();
	com2.RSClose();
}

//funkcja uruchamia badania; number to numer badania (1-2), log - czy logowanie jest wlaczone?(0-1), glitches - ilosc 'glitchy' w jednej probie, second - opoznienie w cyklach
//po pierwszym 'glitchu', third - opoznienie w cyklach po drugim 'glitchu'
int Unlooper::runTest(int number, int log, int glitches, int second, int third)
{
	fstream f;
	unsigned char tmp;
	f.open("test.cfg", ios::in);
	if(!f.is_open())
		return -1;

	//log to file	
	fstream f2;
	if(log == 1){
		f2.open("log.txt", ios::out);
		if(!f2.is_open())
			return -1;
		f2 << "Delays: " << second << ", " << third << endl; 
	}

	if(glitches > 3)
		glitches = 3;

	int n;
	f >> n;
	double *volts = new double [n];
	double *voltsScaled = new double [n];
	for(int i=0; i<n; i++){
		f >> volts[i];	
		//przeliczenie na wartosci od 0-FF
		voltsScaled[i] = ceil((volts[i]*255)/5); //5V = FF (max)
	}

	unsigned char c[10];//dla 3 glitchy

	if(number == 1){
		unsigned int success;//liczy ilosc sukcesow podczas iniekcji glitcha

		c[0] = 2;
		c[1] = 7;	
		c[3] = 5;
		c[4] = 0xB;
		c[5] = glitches;
		c[6] = 0xFF;
		c[7] = 0xFF;
		c[8] = second;
		c[9] = third;

		if(log ==1)
			f2 << "Test number 1\n";
		cout << "Test number 1\n\n";

		//po 1000 pomiarow dla kazdego napiecia
		for(int i=0; i<n; i++){	
			c[2] = static_cast<unsigned char>(voltsScaled[i]);
			success = 0;

			if(log == 1)
				f2 << "Voltage: " << volts[i] << "V";

			for(int j=0; j<1000; j++){ 
				if(glitches == 1)
					com1.RSWrite(c, 8);
				else if(glitches == 2)
					com1.RSWrite(c, 9);
				else if(glitches == 3)
					com1.RSWrite(c, 10);

				Sleep(100);		

				com1.RSRead(&tmp, 1);//czyszczenie bufora po instrukcjach 0x07
				com1.RSRead(&tmp, 1);//0x02
				com1.RSRead(&tmp, 1);//0x05
				com1.RSRead(&tmp, 1);//0x0b		

				if(com2.RSRead(&tmp, 1) >= 0) //odczytywanie z drugiego portu 'S'
					if(tmp == 83)
						success++;

				if(log == 1)
					f2 << tmp;
			}

			cout << "Voltage: " << volts[i] << "V\nSuccessful glitches : " << dec << success << "/1000\n\n";
			if(log == 1)
				f2 << "Successful glitches : " << dec << success << "/1000\n\n";	
		}


	}
	else if(number == 2){//3 glitche	
		unsigned int success[3];//liczy ilosc sukcesow podczas iniekcji glitcha dla kazdego z glitchy osobno

		c[0] = 2;
		c[1] = 7;	
		c[3] = 5;
		c[4] = 0xB;
		c[5] = glitches;
		c[6] = 0xFF;
		c[7] = 0xFF;
		c[8] = second;
		c[9] = third;

		if(log == 1)
			f2 << "Test number 2\n";
		cout << "Test number 2\n\n";

		//po 1000 pomiarow dla kazdego napiecia
		for(int i=0; i<n; i++){	
			c[2] = static_cast<unsigned char>(voltsScaled[i]);
			success[0] = 0;
			success[1] = 0;
			success[2] = 0;
			if(log == 1)
				f2 << "Voltage: " << volts[i] << "V";

			for(int j=0; j<1000; j++){ 
				if(glitches == 1)
					com1.RSWrite(c, 8);
				else if(glitches == 2)
					com1.RSWrite(c, 9);
				else if(glitches == 3)
					com1.RSWrite(c, 10);

				Sleep(100);
				
				com1.RSRead(&tmp, 1);//czyszczenie bufora po instrukcjach 07
				com1.RSRead(&tmp, 1);//02
				com1.RSRead(&tmp, 1);//05
				com1.RSRead(&tmp, 1);//0b

				if(com2.RSRead(&tmp, 1) >= 0){//odczytywanie z drugiego portu danych
					if((tmp&1) == 1)
						success[0]++;
					if((tmp&2) == 2)
						success[1]++;
					if((tmp&4) == 4)
						success[2]++;

					if(log == 1)
						f2 << tmp;

					//odczyt dodatkowej 8bitowej danej do log.txt ('i' z petli 'for') lub pominiecie
					if(com2.RSRead(&tmp, 1) >= 0)	
						if(log == 1)
							f2 << tmp;

				}
			}

			cout << "Voltage: " << volts[i] << "V\nSuccessful glitches - point 1: " << dec << success[0] << "/1000\n";
			if((glitches == 2) || (glitches == 3))
				cout << "Successful glitches - point 2: " << dec << success[1] << "/1000\n";
			if(glitches == 3)
				cout << "Successful glitches - point 3: " << dec << success[2] << "/1000\n\n";
			if(log == 1){	
				f2 << "Voltage: " << volts[i] << "V\nSuccessful glitches - point 1: " << dec << success[0] << "/1000\n";
				if((glitches == 2) || (glitches == 3))
					f2 << "Successful glitches - point 2: " << dec << success[1] << "/1000\n";
				if(glitches == 3)
					f2 << "Successful glitches - point 3: " << dec << success[2] << "/1000\n\n";
			}

		}	
	}
	if(volts)	
		delete [] volts;	
	if(voltsScaled)
		delete [] voltsScaled;	

	f.close();	
	if(log==1)
		f2.close();

	return 0;	
}
