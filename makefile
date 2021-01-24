all: compile

compile:
	gcc -o chessengine main.c board.c moves.c

clean:
	rm chessengine
