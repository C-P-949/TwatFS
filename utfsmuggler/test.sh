#!/bin/bash
#
# This will just run a simple test of encoding utfsmuggler to UTF-8, then decoding it, 
# then checking to make sure it's a bit-perfect copy of the original
#
if [[ -x ./utfsmuggler ]]; then
	./utfsmuggler -i ./utfsmuggler -o utfsmuggler.utf8
	./utfsmuggler -d -i ./utfsmuggler.utf8 -o utfsmuggler.decoded

	check=`md5sum utfsmuggler utfsmuggler.decoded | cut -c1-32 | uniq | wc -l`
	if [[ $check -eq 1 ]]; then
		echo "MD5 sum matches" 
	else
		echo "Error in MD5 sum!"
		md5sum utfsmuggler utfsmuggler.decoded
	fi

	check=`sha1sum utfsmuggler utfsmuggler.decoded | cut -c1-40 | uniq | wc -l`
	if [[ $check -eq 1 ]]; then
		echo "SHA1 sum matches"
	else
		echo "Error in SHA1 sum!"
		sha1sum utfsmuggler utfsmuggler.decoded
	fi
else
	echo "utfsmuggler is not compiled.  Please compile it before running tests."
fi

if [[ -x ./bitReaderTest ]]; then
	echo; echo
	expResult=`printf "Begining getByte() test\nEnd of getByte() test: 5 bytes read\nBegining getBit() test\nEnd of getBit() test: 40 bits read"`
	result=`./bitReaderTest`
	if [[ "$expResult" != "$result" ]]; then
		echo "bitReader test failed"
		./bitReaderTest
	else
		echo "bitReader test passed"
	fi
else
	echo "bitReaderTest is not compiled.  Please compile it before running tests."
fi

if [[ -x ./bitWriterTest ]]; then
	echo; echo
	expResult=""
	result=`./bitWriterTest`
	if [[ "$expResult" != "$result" ]]; then
		echo "bitWriter test failed"
		./bitWriterTest
	else
		echo "bitWriter test passed"
	fi
else
	echo "bitWriterTest is not compiled.  Please compile it before running tests."
fi

echo; echo
