/* 
 * File:   bitWriter.cpp
 * Author: adam
 * 
 * Created on April 21, 2009, 8:10 PM
 */

#include <stdio.h>
#include <ostream>
#include <fstream>
#include "bitWriter.h"

/**
 * This will create a bitWriter object.  It it the caller's responsibility to
 * ensure the file can be written to, as well as cleaning up the file once they
 * are done writing bits to this file.  Don't forget to call finishByte() at the
 * end to ensure that all data is actually pushed to the file.
 */
bitWriter::bitWriter(std::ostream *filePtr) {
    this->outputFile = filePtr;
    bitIndex = 7;
    byte = 0;
    totalBitsWritten = 0;
}

std::ostream* bitWriter::getFilePointer() {
    return this->outputFile;
}

/**
 * This will let you know how many bits are remaining in this byte.
 */
char bitWriter::bitsRemaining() {
    return bitIndex+1;
}

/**
 * This will just tell us how many bits we're written thus far.  Remember, if it
 * is not a multiple of 8, then not all bits have been written to the output
 * stream yet.
 * @return number of bits written
 */
unsigned int bitWriter::getTotalBitsWritten() {
    return this->totalBitsWritten;
}

/**
 * This will write out one byte as efficiently as possible.
 */
void bitWriter::writeByte(unsigned char byte) {
    if(this->bitIndex == 7) // if we're not in the middle of a byte, we can just
        this->outputFile->put(byte); // write it out, which will be more efficient
    else
        for(int i=7; i>=0; --i)
            this->writeBit(byte & (1 << i));
}

/**
 * This will write a bit to the file.  Technically it's cached until we have a
 * full byte then written out, but the caller just needs to keep sending bits,
 * and the bitWriter will handle the output.  This takes the MSB first, so
 * writing out 1, 0, 0, 0, 0, 0, 0, 0 would be the byte 0x80.
 */
void bitWriter::writeBit(bool bit) {
    // we just stop writing bits if an error has occured.
    if(this->outputFile->fail())
        return;

    byte += (bit * (char)(1 << bitIndex));
    this->totalBitsWritten++;
    if(--bitIndex < 0) {
        this->outputFile->put(byte);
        bitIndex = 7;
        byte = 0;
    }
}

/**
 * This method will round out any bytes which may not have a full 8 bits.  For
 * example, if we currently have 5 bits, this will follow the three bits with
 * three zero bits.  This will not change the number/character being output.
 * For example if we have 7 bits totaling 64, (aka '@' or 1000000) we'd just
 * output a leading 0... thus it remains 64 (aka '@' or 01000000)!
 */
void bitWriter::finishByte() {
    //std::cout << "About to write byte: " << (int)byte << "\n";
    this->outputFile->put(byte);
    this->totalBitsWritten += 8-(7-bitIndex);
    byte = 0;
    bitIndex = 7;
}

/**
 * This method will round out any bytes which may not have a full 8 bits.  For
 * example, if we currently have 5 bits, this will follow the three bits with
 * three zero bits.  This will not change the number/character being output.
 * For example if we have 7 bits totaling 64, (aka '@' or 1000000) we'd just
 * output a leading 0... thus it remains 64 (aka '@' or 01000000)!
 */
void bitWriter::finishByteWithZeros() {
//    std::cout << "Byte read thus far..." << byte << "\n";
    if(this->byte == 0) {
        bitIndex = 7;
        byte = 0;
    } else {
        while(bitIndex != 7)
            this->writeBit(0);
    }
}

bitWriter::~bitWriter() {
}

