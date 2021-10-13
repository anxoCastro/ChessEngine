//
// Created by castro on 26/05/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"
#include "board.h"
#include "eval.h"
#include "moves.h"
unsigned long long nodes;
move best_move;

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

    if(b->side == WHITE){
        generate_white_moves(b, lastMove, &m);
        in_check = is_attacked(b, get_ls1b_index(b->WK), WHITE);
    }
    else{
        generate_black_moves(b, lastMove, &m);
        in_check = is_attacked(b, get_ls1b_index(b->BK), BLACK);
    }

    for(int i = 0; i< m.nElements; i++){
        if(make_legal_move(b, m.list[i]) == 1){
            continue;
        }
        lastMove = m.list[i];

        int score = -negamax(b, -beta, -alpha, depth - 1, lastMove);

        unmake_move(b, m.list[i]);
        legal_moves++;
        //Poda beta
        if(score >= beta){
            return beta;
        }

        if(score > alpha){
            alpha = score;
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

    return alpha;

}

int do_negamax(board *b, int depth, move lastMove){
    //Inicializar lista de movimientos
    moveList m;
    m.nElements = 0;
    //-infinito +infinito
    int alpha = -50000;
    int beta = 50000;
    int legal_moves = 0;
    int in_check;
    if(b->side == WHITE){
        generate_white_moves(b, lastMove, &m);
        in_check = is_attacked(b, get_ls1b_index(b->WK), WHITE);
    }
    else{
        generate_black_moves(b, lastMove, &m);
        in_check = is_attacked(b, get_ls1b_index(b->BK), BLACK);
    }
    for(int i = 0; i< m.nElements; i++){
        if(make_legal_move(b, m.list[i]) == 1){
            continue;
        }
        lastMove = m.list[i];
        int score = -negamax(b, -beta, -alpha, depth - 1, lastMove);

        unmake_move(b, m.list[i]);
        legal_moves++;
        //Poda beta
        if(score >= beta){
            return beta;
        }

        if(score > alpha){
            alpha = score;
            best_move = m.list[i];
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

}


move search_position(board *b, int depth, move lastMove){
    nodes = 0;
    initMove(&best_move);
    do_negamax(b, depth, lastMove);

    char *string_move = malloc(sizeof(char) * 5);
    move_to_string(&best_move, string_move);
    printf("bestmove %s\n", string_move);

    return best_move;
}