//
// Created by castro on 27/05/21.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uci.h"
#include "moves.h"
#include "zobrist.h"
#include "search.h"
#include "board.h"
#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

void parse_position(char *input, board *b){
    char *current_char;// = input;
    if(strncmp(input, "startpos", 8) == 0){
        importFEN(DEFAULT_FEN, b);
    }
    else{
        current_char = strstr(input, "fen");
        if(current_char == NULL){
            importFEN(DEFAULT_FEN, b);
        }
        else{
            current_char += 4;
            importFEN(current_char, b);
        }
    }

    current_char = strstr(input, "moves");
    if(current_char != NULL){
        current_char += 6;

        move move;
        char *string = malloc(sizeof(char) * 5);
        while(*current_char){
            initMove(&move);
            string_to_move(current_char, &move, b);
            move_to_string(&move,string);
            //printf("%s", string);
            make_legal_move(b, move);
            //printBoard(*b);
            // move current character mointer to the end of current move
            while (*current_char && *current_char != ' ') current_char++;
            current_char++;
        }

    }

    b->hash = generate_hash(*b);
}

void uci_loop(board *b){
    char input[1024];
    printf("id name " NAME_ENGINE "\n");
    printf("id author " AUTHOR_ENGINE "\n");
    //Aquí irán opciones disponibles
    printf("uciok\n");
    move lastMove;
    initMove(&lastMove);
    while(1){
        //Resetear la linea
        memset(input, 0, sizeof(input));
        fflush(stdout);

        if(!fgets(input, sizeof(input), stdin )) continue;

        if(input[0] == '\n') continue;

        if(strncmp(input, "isready", 7 ) == 0){
            printf("readyok\n");
            continue;
        }

        if(strncmp(input, "go", 2) == 0){
            lastMove = search_position(b, 7, lastMove);
            continue;
        }

        if(strncmp(input, "position", 8) == 0){
            parse_position(input, b);
        }

        if(strncmp(input, "print", 5) == 0){
            printBoard(*b);
        }
        if(strncmp(input, "exit", 4) == 0){
            break;
        }

    }
}
