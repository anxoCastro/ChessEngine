all: compile

compile:
	gcc -Wall -Ofast -o chessengine main.c board.c bitops.c moves.c perft.c magic_bitboards.c zobrist.c uci.c search.c eval.c time.c

 clean:
	rm chessengine
