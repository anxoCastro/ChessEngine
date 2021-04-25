//
// Created by castro on 06/04/21.
//

#include <stdio.h>
#include <malloc.h>
#include "perft.h"
#include "moves.h"
#include "board.h"
#include <string.h>
#include <sys/time.h>

unsigned long leafs;
moveList m;
extern board b;
long get_time_ms() {
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
}
void perft(int depth, move lastMove){
    if(depth == 0){
        leafs++;
        return;
    }
    moveList m;// = create_move_list();
    m.nElements = 0;
    if(b.side == WHITE){
        generate_white_moves(lastMove, &m);
    }
    else{
        generate_black_moves(lastMove, &m);
    }
    for(int i = 0; i < m.nElements; i++){
        if(make_legal_move(m.list[i]) == 1){
            continue;
        }
        perft(depth - 1, m.list[i]);
        unmake_move(m.list[i]);
    }
}

void do_perft(int depth, char *fen){
    //printf("\n      Perft con profundidad: %i\n\n", depth);
    importFEN(fen);
    move lastMove;
    initMove(&lastMove);
    lastMove.enpassantsquare = b.enpassant_square;
    generate_move_tables();
    char *string = malloc(sizeof(char) * 5);
    long start = get_time_ms();
    moveList m;
    m.nElements=0;
    if(b.side == WHITE)generate_white_moves(lastMove, &m);
    else generate_black_moves(lastMove, &m);
    for(int i = 0; i < m.nElements; i++){
        int j = make_legal_move(m.list[i]);
        if(j== 1){
            continue;
        }
        unsigned long cummulative_nodes = leafs;
        lastMove = m.list[i];
        move_to_string(&m.list[i], string);
        perft(depth - 1, lastMove);
        unsigned long old_nodes = leafs - cummulative_nodes;
        //printf("        Movimiento: %s  nodos: %lu\n", string, old_nodes);
        printf("%s %lu\n", string, old_nodes);
        unmake_move(m.list[i]);
    }
    long end = get_time_ms();
    //printf("\n  Numero de posiciones finales totales: %lu\n", leafs);
    printf("\n%lu\n", leafs);
    //free(m);
    free(string);
    printBoard(b);
    printf("Tiempo: %f segundos %f MN/s \n", ( end - start)/ (float)1000,( leafs / (float) 1000) / ((float)(end - start)));
}