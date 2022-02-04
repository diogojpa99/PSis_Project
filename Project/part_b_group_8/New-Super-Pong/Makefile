all: server client

server: server.c pong.c super-pong.c
	gcc -g server.c pong.c super-pong.c -o server -lncurses -lpthread

client: client.c pong.c super-pong.c
	gcc -g client.c pong.c super-pong.c -o client -lncurses -lpthread

clean:
	rm ./server ./client