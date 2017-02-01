all: can_hello

can_hello: can_hello.c
	cc -g -o can_hello can_hello.c
