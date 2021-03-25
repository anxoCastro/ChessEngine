all: compile

compile:
	gcc -o chessengine main.c board.c bitops.c moves.c

clean:
	rm chessengine
