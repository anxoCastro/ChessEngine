#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "moves.h"
#include "perft.h"

#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
//#define DEFAULT_FEN "rnbqkbnr/1ppppppp/8/p7/8/4PN2/PPPPBPPP/RNBQK2R b KQkq - 1 3"
#define DEPTH 7
int main(int argc, char *argv[]) {
    if(argc > 3){
        if(strcmp(argv[0], "fen")){
            do_perft(atoi(argv[3]), argv[2]);
        }
    }else{
        do_perft(DEPTH, DEFAULT_FEN);
    }
    return 0;
}
