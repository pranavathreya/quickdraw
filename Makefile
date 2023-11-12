all_server: compile_server run_server

all_client: compile_client run_client

compile_server:
	gcc src/server.c -o server.o -lm -g 

compile_client:
	gcc src/main.c -o client.o -lGL -lGLU -lm -lSDL2 -g

run_server:
	./server.o 0.0.0.0 5001 2> logs/server.log &

run_client:
	./run_client.sh 0.0.0.0

