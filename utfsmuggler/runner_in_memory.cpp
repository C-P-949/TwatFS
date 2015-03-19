/*
 * File:   main.cpp
 * Author: adam
 *
 * Created on April 20, 2009, 7:34 PM
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "smuggler.h"

//#define TEMP_FILENAME "/tmp/tempFilename"
#define OFFSET 35796
// the offset should be the exact size of the compiled program


/*
 * This program will read bytes from the binary file, decode the UTF-8 characters
 * and writes a file into a temporary file, execute the file, and then delete it.
 */
int main(int argc, char** argv) {
	smuggler s;
	std::ifstream ifs(argv[0], std::ifstream::in | std::ios_base::binary);
	std::stringstream ofs(std::stringstream::out);
	ifs.seekg(OFFSET);

	if(!s.decode(&ifs, &ofs)) {
		std::cerr << "Sorry, but I detected an invalid UTF8 sequence!  Exiting...\n";
		return -1;
	}
	ifs.close();
	const char* entry = ofs.str().c_str();
	FILE* outPUT = fopen("/tmp/outPUT", "w");
	fwrite(entry, 1, sizeof(ofs.str().capacity()), outPUT);
	fclose(outPUT);
//	asm("jmp %0;" :"0"(entry));

	printf("fuck you, chelos!\n");
	return(EXIT_SUCCESS);
}
