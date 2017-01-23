all: hello_can

hello_can: hello_can.c
	cc -g -o hello_can hello_can.c
