all: server client

server: server.c pong.c super-pong.c
	gcc server.c pong.c super-pong.c -o server -lncurses

client: client.c pong.c super-pong.c
	gcc client.c pong.c super-pong.c -o client -lncurses