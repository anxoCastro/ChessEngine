all: compile

compile:
	gcc -Ofast -o chessengine main.c board.c bitops.c moves.c perft.c magic_bitboards.c 

clean:
	rm chessengine
