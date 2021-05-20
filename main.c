#include <stdlib.h>
#include <string.h>
#include "moves.h"
#include "perft.h"

#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
//#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR b KQkq a3"

#define DEPTH 6
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
