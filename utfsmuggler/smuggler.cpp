/*
 * File:   smuggler.cpp
 * Author: adam
 *
 * Created on March 6, 2010, 11:10 PM
 */

#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <istream>
#include <ostream>
#include <string>
#include <math.h>
#include "bitReader.h"
#include "bitWriter.h"
#include "smuggler.h"

/** constructor */
smuggler::smuggler() {
}

/**
 * This will encode the file and output it to the given output file.
 * @param input the input filename to be encoded
 * @param output the output filename to which we should write
 */
void smuggler::encode(const char *inputFilename, const char *outputFilename) {
	std::ifstream input(inputFilename, std::ios_base::binary);
	std::ofstream output(outputFilename, std::ios_base::binary);
	bitReader br(&input);
	bitWriter bw(&output);	
	encodeStream(&br, &bw);
	input.close();
	output.close();
}

/**
 * This will encode the file and output it to the given output file.  This
 * does not close the files in case the caller wishes to use them after
 * this method is called.
 * @param input the input file to be encoded
 * @param output the output file to which we should write
 */
void smuggler::encode(std::istream *input, std::ostream *output) {
        bitReader br(input);
        bitWriter bw(output);	
	encodeStream(&br, &bw);
}

/**
 * This will decode the file and output it to the given output file.  This
 * does not close the files in case the caller wishes to use them after
 * this method is called.
 * @param input the input file to be encoded
 * @param output the output file to which we should write
 */
int smuggler::decode(const char *inputFilename, const char *outputFilename) {
	int returnVal;
	std::ifstream input(inputFilename, std::ios_base::binary);
	std::ofstream output(outputFilename, std::ios_base::binary);
	returnVal = this->decode(&input, &output);
	input.close();
	output.close();
	return returnVal;
}

/**
 * This will decode the file and output it to the given output file.  This
 * does not close the files in case the caller wishes to use them after
 * this method is called.
 * @param input the input file to be encoded
 * @param output the output file to which we should write
 */
int smuggler::decode(std::istream *input, std::ostream *output) {
        bitReader br(input);
        bitWriter bw(output);
	return decodeStream(&br, &bw);
}

/**
 * This will encode all the data which can be extracted from the bitstream
 * object which is passed in and output the UTF data to stdout.
 */
void smuggler::encodeStream(bitReader* br, bitWriter* bw) {
    int bit;
    unsigned char bitIndex = 20;
    unsigned int twentyBitNumber = 0;
    while((bit = br->getBit()) >= 0) {
        twentyBitNumber += ((bit)?1:0) * (1 << (bitIndex-1));

        --bitIndex;
        if(bitIndex <= 0) {
            encodeChar(twentyBitNumber, bw);
            twentyBitNumber = 0;
            bitIndex = 20;
        }
    }

    // if we've read a fraction of a 21 bit sequence, output that
    if(bitIndex != 20) { // if we've read a fraction of a 21 bit sequence, output that
        encodeChar(twentyBitNumber, bw);
        // since we output a byte < 20 bits, we need to write out a count of the number of bits used
        bw->writeByte((char)(20-bitIndex));
    } else {
        // since we output a full last byte, we'll need to write out a count of 20
        bw->writeByte((char)20);
    }
}

/**
 * This will encode a 20-bit number to one valid UTF8 character and output via
 * the bitwriter object.
 */
void smuggler::encodeChar(int bits, bitWriter* bw) {
    if(bits < 128) { // 1-byte UTF char
        // 0xxxxxxx
        bw->writeByte((char)bits);
    } else if(bits < 2048) { // 2-byte UTF char
        // 110xxxxx 10xxxxxx
        bw->writeByte((char)(0xC0 | ((bits >> 6) & 0x1F)));
        bw->writeByte((char)(0x80 | (bits & 0x3F)));
    } else if(bits < 32768) { // 3-byte UTF char (12 to 15-bits goes here)
        // Note: the "-" is a zero b/c it's sometimes not valid as a one
        // 1110xxxx 10-xxxxx 10xxxxxx
        bw->writeByte((char)(0xE0 | ((bits >> 11) & 0x0F))); // TODO change to OR?
        bw->writeByte((char)(0x80 | ((bits >> 6) & 0x1F)));
        bw->writeByte((char)(0x80 | (bits & 0x3F)));
    } else { // 4-byte UTF
        // 16 bits (notice the 21st bit is a 1 to make it a VALID UTF-8 char)
        // 111101xx 10xxxxxx 10xxxxxx 10xxxxxx
        // 17+ bits (automagically valid)
        // 111100xx 10xxxxxx 10xxxxxx 10xxxxxx
        if(bits < 65536)
            bw->writeByte((char)(0xF4 | ((bits >> 18) & 0x03)));
        else
            bw->writeByte((char)(0xF0 | ((bits >> 18) & 0x03)));
        bw->writeByte((char)(0x80 | ((bits >> 12) & 0x3F)));
        bw->writeByte((char)(0x80 | ((bits >> 6) & 0x3F)));
        bw->writeByte((char)(0x80 | (bits & 0x3F)));
    }
}

bool smuggler::readAppendedByte(int byte, int& bitsToOutput, bitWriter& bw) {
    if(byte >> 6 == 2) { // make sure the byte starts with 10
        for(int i=5; i>=0 && bitsToOutput > 0; --i, --bitsToOutput)
            bw.writeBit(byte & (1 << i));
        return true;
    }
    return false;
}

int smuggler::decode1ByteChar(bitReader* br, bitWriter* bw, int byte1) {
    unsigned char byte2 = br->getByte(); // get the next byte so we know if we're at the EOF
    int bitsToOutput = 20;

    if(br->good()) { // never process the last byte, it's just a count, not actual data
        if(!br->nearFileEnd()) {  // first 13 bits are zero, otherwise we'd have a bigger UTF character
            for(int i=12; i>=0; --i, --bitsToOutput)
                bw->writeBit(0);
        } else { // this is the last byte of actual data, so we look at the
            // next byte to determine how many bytes to use from this character
            bitsToOutput = byte2;
            for(int i=12; i>=0 && bitsToOutput > 0; --i, --bitsToOutput)
                bw->writeBit(0);
        }

        // 0xxxxxxx
        for(int i=6; i >= 0 && bitsToOutput > 0; --i, --bitsToOutput)
            bw->writeBit(byte1 & (1<<i));

        return byte2; // byte2 is actually our next byte1
    }
    return 0;
}

int smuggler::decode2ByteChar(bitReader* br, bitWriter* bw, int byte1) {
    unsigned char byte2 = br->getByte(); // get the next few bytes so we know if we're at the EOF
    unsigned char byte3 = br->getByte();
    int i=4;
    int bitsToOutput = 20; // how many bits do we output (usually 11, unless at EOF with this char)

    if(br->good()) { // never process the last byte, it's just a count, not actual data
        if(!br->nearFileEnd()) {  // first 9 bits are zero, otherwise we'd have a bigger UTF character
            for(int j=8; j>=0; --j, --bitsToOutput)
                bw->writeBit(0);
        } else { // this is the last byte of actual data, so we look at the
            // next byte to determine how many bytes to use from this character
            bitsToOutput = byte3; // e.g. 11 bits used... stop after processing bit index 0
            for(int j=8; j>=0 && bitsToOutput-- > 0; --j)
                bw->writeBit(0);
        }

        // 110xxxxx 10xxxxxx
        // we're going to read in up to 5 bits from the first byte, and up to 6 from the second
        for(; i >= 0 && bitsToOutput > 0; --i, --bitsToOutput)
            bw->writeBit(byte1 & (1<<i));

        if(!readAppendedByte(byte2, bitsToOutput, *bw))
            return false;

        return byte3; // byte3 is actually our next byte1
    }
    return 0;
}

int smuggler::decode3ByteChar(bitReader* br, bitWriter* bw, int byte1) {
    unsigned char byte2 = br->getByte(); // get the next few bytes so we know if we're at the EOF
    unsigned char byte3 = br->getByte();
    unsigned char byte4 = br->getByte();
    int bitsToOutput = 20; // normally output 16 bits, unless at or near EOF

    if(br->good()) {
        if(!br->nearFileEnd()) {
            for(int i=4; i>=0; --i, --bitsToOutput) // first 5 bits are zeros, or we would have
                bw->writeBit(0);                    // needed a larger UTF character
        } else { // this is the last byte of actual data, so we look at the
            // next byte to determine how many bytes to use from this character
            bitsToOutput = byte4; // e.g. 16 bits used... stop after processing bit index 0
            // if we don't have more than 15 bits then we shouldn't pad!
            for(int j=4; j>=0 && bitsToOutput > 0; --j, --bitsToOutput) {
                bw->writeBit(0);
            }
        }

        // 1110xxxx 10-xxxxx 10xxxxxx
        for(int i=3; i>=0 && bitsToOutput > 0; --i, --bitsToOutput)
            bw->writeBit(byte1 & (1 << i));

        if(byte2 >> 6 == 2) { // make sure the byte starts with 10
            for(int i=4; i>=0 && bitsToOutput > 0; --i, --bitsToOutput)
                bw->writeBit(byte2 & (1 << i));
        } else {
            return false;
        }

        if(!readAppendedByte(byte3, bitsToOutput, *bw))
            return false;

        return byte4; // byte4 is actually our next byte1
    }
    return 0;
}

int smuggler::decode4ByteChar(bitReader* br, bitWriter* bw, int byte1) {
    unsigned char byte2 = br->getByte(); // get the next few bytes so we know if we're at the EOF
    unsigned char byte3 = br->getByte();
    unsigned char byte4 = br->getByte();
    unsigned char byte5 = br->getByte();
    int bitsToOutput = 20;

    if(br->good()) {
        if(br->nearFileEnd())
            bitsToOutput = byte5; // how many bits should we use from this character?

        // 111100xx 10xxxxxx 10xxxxxx 10xxxxxx
        for(int i=1; i>=0 && bitsToOutput > 0; --i, --bitsToOutput)
            bw->writeBit(byte1 & (1 << i));

        if(!readAppendedByte(byte2, bitsToOutput, *bw))
            return false;

        if(!readAppendedByte(byte3, bitsToOutput, *bw))
            return false;

        if(!readAppendedByte(byte4, bitsToOutput, *bw))
            return false;

        return byte5; // byte5 is actually our next byte1
    }
    return 0;
}

/**
 * This will decode all the UTF8 data which can be extracted from the bitstream
 * object which is passed in.  It will write the output to stdout.  Returns true
 * on success, false on error (invalid UTF8 sequence, read error, etc.)
 */
bool smuggler::decodeStream(bitReader* br, bitWriter* bw) {
    unsigned char byte1 = br->getByte();
    
    while(br->good()) {
        if(byte1 <= 0x7F) { // 1-byte UTF char... this scores us 7 bits
            byte1 = decode1ByteChar(br, bw, byte1);
        } else if(byte1 <= 0xDF) { // 2-byte UTF char... this scores us 11 bits
            byte1 = decode2ByteChar(br, bw, byte1);
        } else if(byte1 <= 0xEF) { // 3-byte UTF char... this scores us 15 bits
            byte1 = decode3ByteChar(br, bw, byte1);
        } else { // 4-byte UTF... this scores us the full 20 bits
            byte1 = decode4ByteChar(br, bw, byte1);
        }
    }
    return true;
}
