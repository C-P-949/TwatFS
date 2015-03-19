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

/*
 * This program will read bytes from a file, encodes the bits into valid UTF8
 * characters, then writes them to the standard output.  If no file is specified,
 * we'll read from the standard input.  Default action is to encode data, if both
 * are specified, it will decode.  Returns -1 if in decode mode and an invalid
 * UTF8 sequence is detected.
 */
int main(int argc, char** argv) {
	try {
		TCLAP::CmdLine cmd("UTF smuggler will encode/decode files from and to UTF8.", ' ', "0.9");
		TCLAP::ValueArg<std::string> inputFileArg("i", "input",
		                                     "File to use as input (stdin if omitted)",
		                                     false, "", "string");
		cmd.add(inputFileArg);
		TCLAP::ValueArg<std::string> outputFileArg("o", "output",
		                                     "File to use as output (stdout if omitted)",
		                                     false, "", "string");
		cmd.add(outputFileArg);
		TCLAP::SwitchArg decodeSwitch("d", "decode", "Decode input (default is to encode)");
		cmd.add(decodeSwitch);
		cmd.parse(argc, argv);
		
		bool encode = !decodeSwitch.getValue();
		std::string inputFilename = inputFileArg.getValue();
		std::string outputFilename = outputFileArg.getValue();
		
		std::istream *input;
		std::ostream *output;
		
		if(inputFilename != "")
			input = new std::ifstream(inputFilename.c_str(), std::ios_base::binary);
		else
			input = &std::cin;
		
		if(outputFilename != "")
			output = new std::ofstream(outputFilename.c_str(), std::ios_base::binary);
		else
			output = &std::cout;
		
		smuggler s;
		if(encode) {
			s.encode(input, output);
		} else {
			if(!s.decode(input, output)) {
				std::cerr << "Invalid UTF8 sequence detected!\n";
				return -1;
			}
		}
		
		if(inputFilename != "") {
			((std::ifstream*)input)->close();
			delete input;
		}
		
		if(outputFilename != "") {
			((std::ofstream*)output)->close();
			delete output;
		}
		
	} catch (TCLAP::ArgException &e) { // catch any exceptions
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	
	return(EXIT_SUCCESS);
}
