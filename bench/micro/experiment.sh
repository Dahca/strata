#! /bin/bash

#FILESIZE=30G
#THREADS=1

#FILESIZE=15G
#THREADS=2

#FILESIZE=7500M
#THREADS=4

FILESIZE=3750M
THREADS=8

TYPE=sw
#TYPE=rw
#TYPE=sr
#TYPE=rr

./iobench.normal $TYPE $FILESIZE 4K $THREADS
./iobench.normal $TYPE $FILESIZE 4K $THREADS
./iobench.normal $TYPE $FILESIZE 4K $THREADS
./iobench.normal $TYPE $FILESIZE 4K $THREADS
./iobench.normal $TYPE $FILESIZE 4K $THREADS
