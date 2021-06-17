//
// Created by castro on 26/05/21.
//

#ifndef CHESSENGINEC_SEARCH_H
#define CHESSENGINEC_SEARCH_H

#include "moves.h"
#include "board.h"
int do_negamax(board *b, int depth, move lastMove);
move search_position(board *b, int depth, move lastMove);
#endif //CHESSENGINEC_SEARCH_H
