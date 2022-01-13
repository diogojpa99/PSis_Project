all: server client

server: server.c pong.c relay-pong.c
	gcc server.c pong.c relay-pong.c -o server -lncurses

client: client.c pong.c relay-pong.c
	gcc client.c pong.c relay-pong.c -o client -lncurses