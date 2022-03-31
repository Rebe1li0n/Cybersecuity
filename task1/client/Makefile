CFLAGS=-z execstack -fno-stack-protector -no-pie -lpthread

all:client

tcp_client.o:
	gcc -o tcp_client.o -c tcp_client.c $(CFLAGS)

client:tcp_client.o
	gcc -o client tcp_client.o $(CFLAGS)

clean:
	rm -r -f client *.o

