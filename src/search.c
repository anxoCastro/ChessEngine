//
// Created by castro on 26/05/21.
//

#include <stdio.h>
#include <stdlib.h>
#include "search.h"
#include "eval.h"
#include "moves.h"
#include "time.h"
unsigned long long nodes;
move best_move;
int ply;


int negamax(board *b, int alpha, int beta, int depth, move lastMove){
    if(depth == 0){
        return evaluation(*b);
    }
    nodes++;

    int in_check;
    int legal_moves = 0;
    //Inicializar lista de movimientos
    moveList m;
    m.nElements = 0;
    move best_sofar;
    int old_alpha = alpha;

    if(b->side == WHITE){
        generate_white_moves(b, lastMove, &m);
        in_check = is_attacked(b, get_ls1b_index(b->WK), WHITE);
    }
    else{
        generate_black_moves(b, lastMove, &m);
        in_check = is_attacked(b, get_ls1b_index(b->BK), BLACK);
    }

    for(int i = 0; i< m.nElements; i++){
        ply++;
        if(make_legal_move(b, m.list[i]) == 1){
            ply--;
            continue;
        }
        lastMove = m.list[i];

        int score = -negamax(b, -beta, -alpha, depth - 1, lastMove);
        ply--;
        unmake_move(b, m.list[i]);
        legal_moves++;
        //Poda beta
        if(score >= beta){
            return beta;
        }

        if(score > alpha){
            alpha = score;

            if(ply==0)
                best_sofar = lastMove;
        }
    }

    if(legal_moves == 0){
        if(in_check){
            //Jaque mate
            return -49000 - depth;
        }
        else{
            //GG
            return 0;
        }
    }
    //Encontrado mejor movimiento
    if (old_alpha != alpha)
        best_move = best_sofar;
    return alpha;

}

move search_position(board *b, int depth, move lastMove){
    nodes = 0;
    initMove(&best_move);

    long start = get_time_ms();
    int score = negamax(b, -50000, 50000, depth, lastMove);
    long end = get_time_ms();

    char *string_move = malloc(sizeof(char) * 5);
    move_to_string(&best_move, string_move);

    printf("info score cp %i depth %i time %ld nodes %llu nps %.0f\n", score, depth,(end-start), nodes, ((float)nodes/(float)(end-start))*1000);
    printf("bestmove %s\n", string_move);
    free(string_move);

    return best_move;
}