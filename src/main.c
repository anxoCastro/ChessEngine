#include "moves.h"
#include "perft.h"
#include "uci.h"
#include "zobrist.h"
#include <stdio.h>
#include <string.h>
#include "eval.h"
#include <stdlib.h>

#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
int main(int argc, char *argv[]) {
    //printf(NAME_ENGINE "\n");
    char input[1024];
    board b;
    generate_move_tables();
    init_keys();
    initBoard(&b);
    init_eval_tables();
    b.hash = generate_hash(b);

    //Inicializar hash
    b.hash = generate_hash(b);
    while(1){
        //Resetear la linea
        memset(input, 0, sizeof(input));
        fflush(stdout);

        if(!fgets(input, sizeof(input), stdin )) continue;
        if(input[0] == '\n') continue;

        if(strncmp(input, "uci", 3 ) == 0){
            uci_loop(&b);
            continue;
        }
        if(strncmp(input, "perft", 5) == 0){
            char *split;
            split = strtok(input, " ");
            split = strtok(NULL, " ");
            if(split != NULL) {
                if (strcmp(split, "fen")) {
                    split = strtok(NULL, " ");
                    if (split != NULL) {
                        char *fen = split;
                        printf("%s", fen);
                        split = strtok(NULL, " ");
                        if (split != NULL) {
                            int depth = atoi(split);
                            do_perft(depth, fen);
                        } else {
                            do_perft(7, fen);
                        }
                    }
                }
            }
            else{
                do_perft(6, DEFAULT_FEN);
            }
            continue;
        }
        if(strncmp(input, "exit", 4) == 0){
            break;
        }
    }
    /*if(argc > 3){
        if(strcmp(argv[0], "fen")){
            do_perft(atoi(argv[3]), argv[2]);
        }
    }else{
        do_perft(DEPTH, DEFAULT_FEN);
    }*/
    return 0;
}
