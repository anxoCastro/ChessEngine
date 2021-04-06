//
// Created by castro on 06/04/21.
//

#include <stdio.h>
#include <malloc.h>
#include "perft.h"
#include "moves.h"
#include "board.h"

unsigned long leafs;
//unmake_stack stack;
void perft(board *b, int depth, move lastMove, unmake_stack stack){
    if(depth == 0){
        leafs++;
        return;
    }
    char *string = malloc(sizeof(char) * 4);
    moveList *m = create_move_list();
    if(b->side == WHITE){
        m = generate_white_moves(b, lastMove, m);
    }
    else{
        m = generate_black_moves(b, lastMove, m);
    }
    for(int i = 0; i < m->nElements; i++){
        if(make_legal_move(b, m->list[i], &stack) == 1){
            continue;
        }
        lastMove = m->list[i];
        perft(b, depth - 1, lastMove, stack);
        unmake_move(b, m->list[i], &stack);
    }
    free(m);
}

void do_perft(int depth, char *fen){
    unmake_stack *stack = malloc(sizeof (struct unmake_stack));
    stack->nElements = 0;
    printf("\n      Perft con profundidad: %i\n\n", depth);
    board b;
    importFEN(fen, &b);
    printBoard(b);
    printf("\n\n\n");
    move lastMove;
    initMove(&lastMove);
    lastMove.enpassantsquare = b.enpassant_square;
    print_bitboard(lastMove.enpassantsquare);
    moveList *m = create_move_list();
    generate_move_tables();
    char *string = malloc(sizeof(char) * 4);
    if(b.side == WHITE)m = generate_white_moves(&b, lastMove, m);
    else m = generate_black_moves(&b, lastMove, m);
    for(int i = 0; i < m->nElements; i++){
        int j = make_legal_move(&b, m->list[i], stack);
        if(j== 1){
            continue;
        }
        unsigned long cummulative_nodes = leafs;
        move_to_string(&m->list[i], string);
        perft(&b, depth - 1, lastMove, *stack);
        unsigned long old_nodes = leafs - cummulative_nodes;
        printf("        Movimiento: %s  nodos: %lu\n", string, old_nodes);
        unmake_move(&b, m->list[i], stack);
    }
    printf("\n  Numero de posiciones finales totales: %lu\n", leafs);
    free(string);
    free(m);
}