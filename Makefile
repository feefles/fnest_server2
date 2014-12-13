all: mmap

mmap: mmap.c
	gcc -Wall -O3 -shared mmap.c -o mmap.so
