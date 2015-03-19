/*
 * File:   smuggler.h
 * Author: adam
 *
 * Created on March 6, 2010, 11:10 PM
 */

#include <istream>
#include <ostream>
#include <fstream>
#include "bitReader.h"
#include "bitWriter.h"

#ifndef _UTF8_SMUGGLER_H
#define _UTF8_SMUGGLER_H

class smuggler {
public:
	smuggler();
	void encode(const char *input, const char *output);
	void encode(std::istream *input, std::ostream *output);
	int decode(const char *input, const char *output);
	int decode(std::istream *input, std::ostream *output);

private:
	void encodeStream(bitReader*, bitWriter*);
	void encodeChar(int, bitWriter*);
	bool readAppendedByte(int byte, int& bitsToOutput, bitWriter& bw);
	bool decodeStream(bitReader* br, bitWriter* bw);
	int decode1ByteChar(bitReader* br, bitWriter* bw, int byte1);
	int decode2ByteChar(bitReader* br, bitWriter* bw, int byte1);
	int decode3ByteChar(bitReader* br, bitWriter* bw, int byte1);
	int decode4ByteChar(bitReader* br, bitWriter* bw, int byte1);
};

#endif  /* _UTF8_SMUGGLER_H */

