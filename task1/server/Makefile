CFLAGS=-z execstack -fno-stack-protector -no-pie -lpthread

all:server

tcp_server.o:
	gcc -o tcp_server.o -c tcp_server.c $(CFLAGS)

server:tcp_server.o
	gcc -o server tcp_server.o $(CFLAGS)
	
clean:
	rm -r -f server *.o


