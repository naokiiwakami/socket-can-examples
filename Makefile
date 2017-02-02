all: can_hello can_filter

can_hello: can_hello.o can_open_socket.o

can_filter: can_filter.o can_open_socket.o
