//
// Created by castro on 06/04/21.
//

#include <stdio.h>
#include <stdlib.h>
#include "perft.h"
#include "moves.h"
#include "zobrist.h"
#include <string.h>
#include <sys/time.h>

unsigned long long leafs;
long get_time_ms() {
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
}
void perft(board *b, int depth, move lastMove){
    if(depth == 0){
        leafs++;
        return;
    }
    moveList m;// = create_move_list();
    m.nElements = 0;
    if(b->side == WHITE){
        generate_white_moves(b, lastMove, &m);
    }
    else{
        generate_black_moves(b, lastMove, &m);
    }
    for(int i = 0; i < m.nElements; i++){
        if(make_legal_move(b, m.list[i]) == 1){
            continue;
        }
        lastMove = m.list[i];
        perft(b, depth - 1, lastMove);
        unmake_move(b, m.list[i]);
    }
    //free(m);
}

void do_perft(int depth, char *fen){
    //printf("\n      Perft con profundidad: %i\n\n", depth);
    board b;
    importFEN(fen, &b);
    move lastMove;
    initMove(&lastMove);
    lastMove.enpassantsquare = b.enpassant_square;
    generate_move_tables();
    init_keys();
    //Inicializar hash
    b.hash = generate_hash(b);
    bitboard old_hash = b.hash;
    //printBoard(b);
    char *string = malloc(sizeof(char) * 5);
    long start = get_time_ms();
    moveList m;
    m.nElements=0;
    if(b.side == WHITE)generate_white_moves(&b, lastMove, &m);
    else generate_black_moves(&b, lastMove, &m);
    for(int i = 0; i < m.nElements; i++){
        int j = make_legal_move(&b, m.list[i]);
        if(j== 1){
            continue;
        }
        unsigned long long cummulative_nodes = leafs;
        lastMove = m.list[i];
        move_to_string(&m.list[i], string);
        perft(&b, depth - 1, lastMove);
        unsigned long long old_nodes = leafs - cummulative_nodes;
        //printf("        Movimiento: %s  nodos: %lu\n", string, old_nodes);
        printf("%s %llu\n", string, old_nodes);
        unmake_move(&b, m.list[i]);
    }
    long end = get_time_ms();
    //printf("\n  Numero de posiciones finales totales: %lu\n", leafs);
    printf("\n%llu\n", leafs);
    //free(m);
    free(string);
    printBoard(b);
    if(old_hash == b.hash){
        printf("Los hash coinciden :)\n");
    }
    printf("Tiempo: %f segundos %f MN/s \n", (float)( end - start)/ (float)1000,( (float) leafs / (float) 1000) / ((float)(end - start)));
}