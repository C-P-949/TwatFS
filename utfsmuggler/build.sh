#!/bin/bash
#
# The makefile is for Netbeans and works great in the IDE, but I wanted something simple, so I made this
# If you're using Windows, stick with Netbeans, everyone else should be able to use this!
#

if [[ $1 == "clean" ]]; then
	echo rm -f *.o bitWriterTest bitReaderTest utfsmuggler utfsmuggler.utf8 utfsmuggler.decoded runner
	rm -f *.o bitWriterTest bitReaderTest utfsmuggler utfsmuggler.utf8 utfsmuggler.decoded runner
elif [[ $1 == "debug" ]]; then
	g++ -Wall -g -o bitReader.o -c bitReader.cpp
	g++ -Wall -g -o bitWriter.o -c bitWriter.cpp
	g++ -Wall -g -o smuggler.o  -c smuggler.cpp
	g++ -Wall -g -o bitWriterTest bitWriterTest.cpp bitWriter.o
	g++ -Wall -g -o bitReaderTest bitReaderTest.cpp bitReader.o
	g++ -Wall -g -o utfsmuggler -I. main.cpp bitReader.o bitWriter.o smuggler.o
	g++ -Wall -g -o runner -I. runner.cpp bitReader.o bitWriter.o smuggler.o
	g++ -Wall -g -o runner2 -I. runner2.cpp bitReader.o bitWriter.o smuggler.o
else
	echo g++ -Wall     -o bitReader.o -c bitReader.cpp
	g++ -Wall     -o bitReader.o -c bitReader.cpp
	echo g++ -Wall -O3 -o bitWriter.o -c bitWriter.cpp
	g++ -Wall -O3 -o bitWriter.o -c bitWriter.cpp
	echo g++ -Wall -O3 -o smuggler.o  -c smuggler.cpp
	g++ -Wall -O3 -o smuggler.o  -c smuggler.cpp
	echo g++ -Wall -O3 -o bitWriterTest bitWriterTest.cpp bitWriter.o
	g++ -Wall -O3 -o bitWriterTest bitWriterTest.cpp bitWriter.o
	echo g++ -Wall -O3 -o bitReaderTest bitReaderTest.cpp bitReader.o
	g++ -Wall -O3 -o bitReaderTest bitReaderTest.cpp bitReader.o
	echo g++ -Wall -O3 -o utfsmuggler -I. main.cpp bitReader.o bitWriter.o smuggler.o
	g++ -Wall -O3 -o utfsmuggler -I. main.cpp bitReader.o bitWriter.o smuggler.o
	echo g++ -Wall -O3 -o runner -I. runner.cpp bitReader.o bitWriter.o smuggler.o
	g++ -Wall -O3 -o runner -I. runner.cpp bitReader.o bitWriter.o smuggler.o
	echo g++ -Wall -O3 -o runner2 -I. runner2.cpp bitReader.o bitWriter.o smuggler.o
	g++ -Wall -O3 -o runner2 -I. runner2.cpp bitReader.o bitWriter.o smuggler.o
fi

