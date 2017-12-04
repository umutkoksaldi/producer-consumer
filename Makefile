all: pcsync
	
cost: pcsync.c
	cc -g -o pcsync pcsync.c -lpthread
