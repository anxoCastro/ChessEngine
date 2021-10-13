//
// Created by castro on 26/05/21.
//

#include "eval.h"
#include "board.h"
#include "bitops.h"


int evaluation(board b){
    int score = 0;
    bitboard aux;
    unsigned char from;
    //Peones
    aux = b.WP;
    while(aux){
        from = get_ls1b_index(aux);
        score += PAWN_SCORE;
        pop_bit(aux, from);
    }
    aux = b.BP;
    while(aux){
        from = get_ls1b_index(aux);
        score -= PAWN_SCORE;
        pop_bit(aux, from);
    }

    //Caballos
    aux = b.WN;
    while(aux){
        from = get_ls1b_index(aux);
        score += KNIGHT_SCORE;
        pop_bit(aux, from);
    }
    aux = b.BN;
    while(aux){
        from = get_ls1b_index(aux);
        score -= KNIGHT_SCORE;
        pop_bit(aux, from);
    }

    //Alfiles
    aux = b.WB;
    while(aux){
        from = get_ls1b_index(aux);
        score += BISHOP_SCORE;
        pop_bit(aux, from);
    }
    aux = b.BB;
    while(aux){
        from = get_ls1b_index(aux);
        score -= BISHOP_SCORE;
        pop_bit(aux, from);
    }

    //Torres
    aux = b.WR;
    while(aux){
        from = get_ls1b_index(aux);
        score += ROOK_SCORE;
        pop_bit(aux, from);
    }
    aux = b.BR;
    while(aux){
        from = get_ls1b_index(aux);
        score -= ROOK_SCORE;
        pop_bit(aux, from);
    }

    //Reinas
    aux = b.WQ;
    while(aux){
        from = get_ls1b_index(aux);
        score += QUEEN_SCORE;
        pop_bit(aux, from);
    }
    aux = b.BQ;
    while(aux){
        from = get_ls1b_index(aux);
        score -= QUEEN_SCORE;
        pop_bit(aux, from);
    }
    if(b.side == WHITE){
        return score;
    }
    else{
        return -score;
    }

}