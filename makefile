all: compile

compile:
	gcc -Ofast -o chessengine main.c board.c bitops.c moves.c perft.c

clean:
	rm chessengine
