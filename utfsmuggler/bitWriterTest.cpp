#define OUTPUT_FILENAME "5bytes.output"

#include <ostream>
#include <fstream>
#include <stdio.h>
#include "bitWriter.h"

int main(int argc, char** argv) {
	std::ofstream outputFile(OUTPUT_FILENAME, std::ios_base::binary | std::ios_base::out);
	bitWriter writer(&outputFile);
	
	writer.writeByte(0xFF);
	writer.writeByte(0xFF);
	writer.writeByte(0xFF);
	writer.writeByte(0xFF);
	writer.writeByte(0xFF);
	unsigned int written = writer.getTotalBitsWritten();
	if(written != 0)
		fprintf(stderr, "Wrong number of bits written.  Should have been 0, was actually %d\n", written);
	
	written = writer.bitsRemaining();
	if(written != 8)
		fprintf(stderr, "Should be 8 bits remaining, but there were %d\n", written);
	
	outputFile.close();
	if(remove(OUTPUT_FILENAME) != 0)
		fprintf(stderr, "Error deleting temp file\n");
	
	
	std::ofstream outputFile2(OUTPUT_FILENAME, std::ios_base::binary | std::ios_base::out);
	bitWriter writer2(&outputFile);
	for(int i=0; i < 40; ++i)
		writer2.writeBit(1);
	
	written = writer2.getTotalBitsWritten();
	if(written != 8)
		fprintf(stderr, "Wrong number of bits written.  Should have been 8, was actually %d\n", written);
	
	written = writer2.bitsRemaining();
	if(written != 8)
		fprintf(stderr, "Should be 8 bits remaining, but there were %d\n", written);
	
	outputFile2.close();
	if(remove(OUTPUT_FILENAME) != 0)
		fprintf(stderr, "Error deleting temp file\n");

	return 0;
}
