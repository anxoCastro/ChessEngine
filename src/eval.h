//
// Created by castro on 26/05/21.
//

#include "board.h"
#ifndef CHESSENGINEC_EVAL_H
#define CHESSENGINEC_EVAL_H
#define INFINITE_SCORE 100000
#define PAWN_SCORE 100
#define KNIGHT_SCORE 300
#define BISHOP_SCORE 350
#define ROOK_SCORE 500
#define QUEEN_SCORE 1000
#define KING_SCORE 100000

void init_eval_tables();
double evaluation(board b);

#endif //CHESSENGINEC_EVAL_H
