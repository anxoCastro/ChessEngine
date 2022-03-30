//
// Created by castro on 26/05/21.
//

#include <stdio.h>
#include "eval.h"
#include "board.h"
#include "bitops.h"

// Puntuación posicional peones
const int pawn_score_white[64] =
        {       //White
                0,  0,  0,  0,  0,  0,  0,  0,
                50, 50, 50, 50, 50, 50, 50, 50,
                10, 10, 20, 30, 30, 20, 10, 10,
                5,  5, 10, 25, 25, 10,  5,  5,
                0,  0,  0, 20, 20,  0,  0,  0,
                5, -5,-10,  0,  0,-10, -5,  5,
                5, 10, 10,-20,-20, 10, 10,  5,
                0,  0,  0,  0,  0,  0,  0,  0

        };
int pawn_score_black[64];

// Puntuación posicional caballos
const int knight_score_white[64] =
        {
                -50,-40,-30,-30,-30,-30,-40,-50,
                -40,-20,  0,  0,  0,  0,-20,-40,
                -30,  0, 10, 15, 15, 10,  0,-30,
                -30,  5, 15, 20, 20, 15,  5,-30,
                -30,  0, 15, 20, 20, 15,  0,-30,
                -30,  5, 10, 15, 15, 10,  5,-30,
                -40,-20,  0,  5,  5,  0,-20,-40,
                -50,-40,-30,-30,-30,-30,-40,-50,
        };
int knight_score_black[64];

// Puntuación posicional alfiles
const int bishop_score_white[64] =
        {
                -20,-10,-10,-10,-10,-10,-10,-20,
                -10,  0,  0,  0,  0,  0,  0,-10,
                -10,  0,  5, 10, 10,  5,  0,-10,
                -10,  5,  5, 10, 10,  5,  5,-10,
                -10,  0, 10, 10, 10, 10,  0,-10,
                -10, 10, 10, 10, 10, 10, 10,-10,
                -10,  5,  0,  0,  0,  0,  5,-10,
                -20,-10,-10,-10,-10,-10,-10,-20,


        };
int bishop_score_black[64];

// Puntuación posicional torres
const int rook_score_white[64] =
        {
                0,  0,  0,  0,  0,  0,  0,  0,
                5, 10, 10, 10, 10, 10, 10,  5,
                -5,  0,  0,  0,  0,  0,  0, -5,
                -5,  0,  0,  0,  0,  0,  0, -5,
                -5,  0,  0,  0,  0,  0,  0, -5,
                -5,  0,  0,  0,  0,  0,  0, -5,
                -5,  0,  0,  0,  0,  0,  0, -5,
                0,  0,  0,  5,  5,  0,  0,  0

        };
int rook_score_black[64];

//Puntuación posicional dama
const int queen_score_white[64] =
        {
                -20,-10,-10, -5, -5,-10,-10,-20,
                -10,  0,  0,  0,  0,  0,  0,-10,
                -10,  0,  5,  5,  5,  5,  0,-10,
                -5,  0,  5,  5,  5,  5,  0, -5,
                0,  0,  5,  5,  5,  5,  0, -5,
                -10,  5,  5,  5,  5,  5,  0,-10,
                -10,  0,  5,  0,  0,  0,  0,-10,
                -20,-10,-10, -5, -5,-10,-10,-20
        };
int queen_score_black[64];

// Puntuación posicional rey
const int king_score_white[64] =
        {
                -30,-40,-40,-50,-50,-40,-40,-30,
                -30,-40,-40,-50,-50,-40,-40,-30,
                -30,-40,-40,-50,-50,-40,-40,-30,
                -30,-40,-40,-50,-50,-40,-40,-30,
                -20,-30,-30,-40,-40,-30,-30,-20,
                -10,-20,-20,-20,-20,-20,-20,-10,
                20, 20,  0,  0,  0,  0, 20, 20,
                20, 30, 10,  0,  0, 10, 30, 20
        };
int king_score_black[64];

void init_eval_tables(){
    for(int i = 0; i < 64; i++){
        pawn_score_black[i] = pawn_score_white[63-i];
        knight_score_black[i] = knight_score_white[63-i];
        bishop_score_black[i] = bishop_score_white[63-i];
        rook_score_black[i] = rook_score_white[63-i];
        queen_score_black[i] = queen_score_white[63-i];
        king_score_black[i] = king_score_white[63-i];
    }
}

double evaluation(board b){
    int score = 0;
    bitboard aux;
    unsigned char from;
    //Peones
    aux = b.WP;
    while(aux){
        from = get_ls1b_index(aux);
        score += PAWN_SCORE;
        score += pawn_score_white[from];
        pop_bit(aux, from);
    }
    aux = b.BP;
    while(aux){
        from = get_ls1b_index(aux);
        score -= PAWN_SCORE;
        score -= pawn_score_black[from];
        pop_bit(aux, from);
    }

    //Caballos
    aux = b.WN;
    while(aux){
        from = get_ls1b_index(aux);
        score += KNIGHT_SCORE;
        score += knight_score_white[from];
        pop_bit(aux, from);
    }
    aux = b.BN;
    while(aux){
        from = get_ls1b_index(aux);
        score -= KNIGHT_SCORE;
        score -= knight_score_black[from];
        pop_bit(aux, from);
    }

    //Alfiles
    aux = b.WB;
    while(aux){
        from = get_ls1b_index(aux);
        score += BISHOP_SCORE;
        score += bishop_score_white[from];
        pop_bit(aux, from);
    }
    aux = b.BB;
    while(aux){
        from = get_ls1b_index(aux);
        score -= BISHOP_SCORE;
        score -= bishop_score_black[from];
        pop_bit(aux, from);
    }

    //Torres
    aux = b.WR;
    while(aux){
        from = get_ls1b_index(aux);
        score += ROOK_SCORE;
        score += rook_score_white[from];
        pop_bit(aux, from);
    }
    aux = b.BR;
    while(aux){
        from = get_ls1b_index(aux);
        score -= ROOK_SCORE;
        score -= rook_score_black[from];
        pop_bit(aux, from);
    }

    //Reinas
    aux = b.WQ;
    while(aux){
        from = get_ls1b_index(aux);
        score += QUEEN_SCORE;
        score += queen_score_white[from];
        pop_bit(aux, from);
    }
    aux = b.BQ;
    while(aux){
        from = get_ls1b_index(aux);
        score -= QUEEN_SCORE;
        score -= queen_score_black[from];
        pop_bit(aux, from);
    }
    //Reyes
    aux = b.WK;
    from = get_ls1b_index(aux);
    score += KING_SCORE;
    score += king_score_white[from];


    aux = b.BK;
    from = get_ls1b_index(aux);
    score -= KING_SCORE;
    score -= king_score_black[from];

    if(b.side == WHITE){
        return score;
    }
    else{
        return -score;
    }
}