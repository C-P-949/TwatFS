/* 
 * File:   bitReader.h
 * Author: adam
 *
 * Created on April 20, 2009, 8:59 PM
 */

#ifndef _BITREADER_H
#define	_BITREADER_H

class bitReader {
public:
    bitReader(std::istream*);
    int getBit();
    char getByte();
    bool good();
    bool nearFileEnd();
    std::istream* getFilePointer();
    virtual ~bitReader();
private:
    std::istream *file;
    unsigned char buffer;
    int position; // location we should read from next
    int bitPosition; // location of the bit we should read next
};

#endif	/* _BITREADER_H */

