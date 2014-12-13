import ctypes
import sys

def main(t):
	mmap_lib = ctypes.cdll.LoadLibrary('./mmap.so')
	mmap_lib.change_set_temp(t)
	print t

if __name__== '__main__':
	argv = sys.argv
	main(int(argv[1]));
