/*
 * File:   main.cpp
 * Author: adam
 *
 * Created on April 20, 2009, 7:34 PM
 */

#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <math.h>
#include <tclap/CmdLine.h>
#include "bitReader.h"
#include "bitWriter.h"
#include "smuggler.h"

#define TEMP_FILENAME "./tempFilename"

/*
 * This program will read bytes from a file, encodes the bits into valid UTF8
 * characters, then writes them to the standard output.  If no file is specified,
 * we'll read from the standard input.  Default action is to encode data, if both
 * are specified, it will decode.  Returns -1 if in decode mode and an invalid
 * UTF8 sequence is detected.
 */
int main(int argc, char** argv) {
    try {
	TCLAP::CmdLine cmd("Runner will decode UTF8 files and run the binary which was smuggled.", ' ', "0.9");
	TCLAP::UnlabeledValueArg<std::string> inputFileArg("inputFile", "Input UTF-8 file", true, "", "inputFile");
	cmd.add(inputFileArg);
	cmd.parse(argc, argv);
	
	smuggler s;
	if(!s.decode(inputFileArg.getValue().c_str(), TEMP_FILENAME)) {
            std::cerr << "Sorry, but I detected an invalid UTF8 sequence!  Exiting...\n";
            return -1;
        }
        
        std::string setExec = "chmod +x ";
        setExec += TEMP_FILENAME;
	int retVal = system(setExec.c_str());
        
        /* now TEMP_FILENAME should be present and executable, so we execute it! */
        retVal = system(TEMP_FILENAME);
        
	/* delete the temporary file */
	remove(TEMP_FILENAME);
    } catch (TCLAP::ArgException &e) { // catch any exceptions
        std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    return(EXIT_SUCCESS);
}

