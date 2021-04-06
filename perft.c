//
// Created by castro on 06/04/21.
//

#include <stdio.h>
#include <malloc.h>
#include "perft.h"
#include "moves.h"
#include "board.h"

unsigned long nodes;
unmake_stack stack;
void perft(board *b, int depth, move lastMove){
    if(depth == 0){
        nodes++;
        return;
    }

    moveList *m = create_move_list();
    if(b->side == WHITE){
        m = generate_white_moves(b, lastMove, m);
    }
    else{
        m = generate_black_moves(b, lastMove, m);
    }
    for(int i = 0; i < m->nElements; i++){
        if(make_legal_move(b, m->list[i], stack) == 1){
            continue;
        }
        perft(b, depth - 1, lastMove);
        unmake_move(b, m->list[i], stack);
    }
    free(m);
}

void do_perft(int depth){
    stack.nElements = 0;
    printf("\n      Perft move generation for depth: %i\n\n", depth);
    board b;
    initBoard(&b);
    printBoard(b);
    printf("\n\n\n");
    move lastMove;
    moveList *m = create_move_list();
    generate_move_tables();
    char* string = malloc(sizeof(char) * 4);
    m = generate_white_moves(&b, lastMove, m);
    for(int i = 0; i < m->nElements; i++){
        int j = make_legal_move(&b, m->list[i], stack);
        if(j== 1){
            continue;
        }
        unsigned long cummulative_nodes = nodes;
        perft(&b, depth - 1, lastMove);
        unsigned long old_nodes = nodes - cummulative_nodes;
        unmake_move(&b, m->list[i], stack);
        move_to_string(&m->list[i], string);
        printf("    move: %s  nodes: %lu\n", string, old_nodes);
    }

    printf("\n        Numero de nodos: %lu\n", nodes);
}