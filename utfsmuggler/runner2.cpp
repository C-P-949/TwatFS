/*
 * File:   main.cpp
 * Author: adam
 *
 * Created on April 20, 2009, 7:34 PM
 */

#include <stdlib.h>
#include <iostream>
#include "smuggler.h"

#define TEMP_FILENAME "/tmp/tempFilename"
#define OFFSET 30567
// the offset should be the exact size of the compiled program


/*
 * This program will read bytes from the binary file, decode the UTF-8 characters
 * and writes a file into a temporary file, execute the file, and then delete it.
 */
int main(int argc, char** argv) {
	smuggler s;
	std::ifstream ifs(argv[0], std::ifstream::in | std::ios_base::binary);
	std::ofstream ofs(TEMP_FILENAME, std::ifstream::out);
	ifs.seekg(OFFSET);

	if(!s.decode(&ifs, &ofs)) {
		std::cerr << "Sorry, but I detected an invalid UTF8 sequence!  Exiting...\n";
		return -1;
	}
	ifs.close();
	ofs.close();

	// make our output file executable
	int retVal = system("chmod +x " TEMP_FILENAME);

	/* now TEMP_FILENAME should be present and executable, so we execute it! */
	retVal = system(TEMP_FILENAME);

	/* delete the temporary file */
	remove(TEMP_FILENAME);

	return(EXIT_SUCCESS);
}

