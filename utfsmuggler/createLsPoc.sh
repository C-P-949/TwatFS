#!/bin/bash


./utfsmuggler -i `which ls` -o ./ls.utf8
cat ./runner2 ./ls.utf8 > runner2.ls
chmod +x ./runner2.ls
./runner2.ls
