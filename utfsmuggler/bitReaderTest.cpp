#define INPUT_FILENAME "5bytes.input"

#include <istream>
#include <fstream>
#include <stdio.h>
#include "bitReader.h"

int main(int argc, char **argv) {
	unsigned int bytesRead = 0, byteRead;
	std::ifstream inputStream(INPUT_FILENAME, std::ios::in | std::ios_base::binary);
	bitReader reader(&inputStream);
	
	printf("Begining getByte() test\n");
	byteRead = reader.getByte();
	while(reader.good()) {
		++bytesRead;
		if((char)byteRead != (char)0xFF)
			printf("Intput was not 0xFF (as expected), it was %#X\n", byteRead);
		byteRead = reader.getByte();
	}
	inputStream.close();
	printf("End of getByte() test: %d bytes read\n", bytesRead);

	bytesRead = 0;
	printf("Begining getBit() test\n");
	std::ifstream inputStream2(INPUT_FILENAME, std::ios::in | std::ios_base::binary);
	bitReader reader2(&inputStream2);
	int bitRead = reader2.getBit();
	while(reader2.good()) {
		++bytesRead;
		if(bitRead == 0)
			printf("Intput was not 1 (as expected), it was %d\n", bitRead);
		bitRead = reader2.getBit();
	}
	inputStream2.close();
	printf("End of getBit() test: %d bits read\n", bytesRead);
	
	bytesRead = 0;
	std::ifstream inputStream3(INPUT_FILENAME, std::ios::in | std::ios_base::binary);
	bitReader reader3(&inputStream3);
	byteRead = reader3.getByte();
	if(reader3.good()) {
		++bytesRead;
		if(reader3.nearFileEnd())
			printf("nearFileEnd check failed\n");
		
		while(!reader3.nearFileEnd())
			byteRead = reader3.getByte(), ++bytesRead;
		
		if(bytesRead != 5)
			printf("nearFileEnd detected near EOF at %d, should have detected it at 5\n", bytesRead);
	} else {
		printf("Second readByte check failed\n");
	}
	inputStream2.close();
	
	
	return 0;
}
