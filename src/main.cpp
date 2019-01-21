//main.cpp
//v1.0
//Marcel Szewczyk

#include "unlooper.h" 
#include <iostream>

using namespace std;

int main() {
	Unlooper u;		
	if(u.status())
		return -1;
	
	unsigned int tmp;
	unsigned char c;
	unsigned int log = 0;

	while(1) {
		if(u.receiveByte(&c) >= 0){
			cout << hex << static_cast<unsigned>(c) << "\t";
			if((c > 31) && (c < 126))
				cout << c << endl;
			else
				cout << ".\n";        
		} 
		else{
			cout << "\nCommand (hex: 0-B -> unlooper, C-D -> run test, E -> log to file, F -> quit)\n: ";

			cin >> hex >> tmp;
			c = static_cast<unsigned char>(tmp);

			if( c == 0xf ){
				u.closePorts();
				break; 
			}
			else if( c == 0xc ){ 
				u.runTest(1, log); 
			}
			else if( c == 0xd ){ 
				cout << "1st to 2nd glitch delay (cycles 1-255): "; //opoznienie miedzy pierwszym, a drugim 'glitchem'
				cin >> dec >> tmp;
				unsigned tmp2;
				cout << "2nd to 3rd glitch delay (cycles 1-255): "; //opoznienie miedzy drugim, a trzecim 'glitchem'
				cin >> dec >> tmp2;
				u.runTest(2, log, 3, tmp, tmp2); 
			}
			else if( c == 0xe ){ 
				log = !log; 
				if(log)
					cout << "logging to file during test is enabled.\n\n";
				else
					cout << "logging to file during test is disabled.\n\n";
			}

			else{			
				if(u.sendByte(c) >= 0)
					cout << "\nACK and answer (hex & ASCII):\n";
			}
		}
	}
	return 0;
}

