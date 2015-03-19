/* 
 * File:   bitWriter.h
 * Author: adam
 *
 * Created on April 21, 2009, 8:10 PM
 */

#ifndef _BITWRITER_H
#define	_BITWRITER_H

class bitWriter {
public:
    bitWriter(std::ostream*);
    void writeBit(bool);
    void writeByte(unsigned char);
    unsigned int getTotalBitsWritten(); // just used for debugging
    char bitsRemaining();
    void finishByte();
    void finishByteWithZeros();
    std::ostream* getFilePointer();
    virtual ~bitWriter();
private:
    std::ostream* outputFile;
    char byte;
    char bitIndex;
    unsigned int totalBitsWritten; // just used for debugging
};

#endif	/* _BITWRITER_H */

