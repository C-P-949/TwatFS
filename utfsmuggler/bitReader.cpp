/* 
 * File:   bitReader.cpp
 * Author: adam
 * 
 * Created on April 20, 2009, 8:59 PM
 */

#include <fstream>
#include "bitReader.h"

bitReader::bitReader(std::istream *filename) {
    this->file = filename;
    position = 0;
    bitPosition = -1; // make sure the first time through we read data
}

/**
 * This will get a bit from the stream and return it.  A zero is false, a
 * positive number is true, and a negitive number is returned at the end of file
 * or when an error occurs.  More specifically, -1 is EOF and -2 is a read error
 * This will read bytes starting from the MSB.
 */
int bitReader::getBit() {
    // don't keep returning data if we're at EOF or FERROR
    if(this->file->eof())
        return -1; // end of file
    if(this->file->fail() || this->file->bad())
        return -2; // file read error

    if(bitPosition < 0) { // check for out of bounds
        char byte;
        bitPosition = 7;
        // try to read some data
        this->file->read(&byte, 1);
	buffer = (unsigned char)byte;
        if(this->file->eof())
            return -1; // end of file
        if(!this->file->good())
            return -2; // file read error
    }

    return buffer & (int)(1 << bitPosition--);
}

/**
 * This will return true if the file ends when we attempt to read one more byte.
 * @return true if EOF is hit after attempting to read another byte (or EOF is alread hit)
 */
bool bitReader::nearFileEnd() {
    if(!this->good()) // if we've had an error or at EOF, return true
        return true;

    this->file->peek();
    return this->file->eof();
}

bool bitReader::good() {
    return this->file->good();
}

/**
 * This will read a byte of data.  Now it's possible that there's not a full
 * byte left in the stream (for example if you read a bit or seven earlier), in
 * the case that we can not get a full 8 bits, it'll read in the first few and
 * then the rest will be zeros.  In order to avoid these awkward moments, I'd
 * suggest that if you mix and match reading bits and bytes from a stream, you
 * keep track of the number of bits you read and make sure it's a multiple of 8.
 */
char bitReader::getByte() {
    char byte = 0;
    if(this->bitPosition == -1) {
        // if the bit position is -1, we can just spit out the entire next byte in a single read
        this->file->read(&byte, 1);
        return byte;
    }

    int bit;
    char bitIndex = 7;
    while(bitIndex >= 0 && (bit = this->getBit()) >= 0)
        byte += ((bit)?1:0) * (1 << bitIndex--);
    
    return byte;
}

std::istream* bitReader::getFilePointer() {
    return this->file;
}

bitReader::~bitReader() {
}
