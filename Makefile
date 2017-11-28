all: pcsync
	
cost: pcsync.c
	cc -Wall -g -o pcsync pcsync.c -lpthread
