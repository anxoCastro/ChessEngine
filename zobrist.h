//
// Created by castro on 13/04/21.
//


#ifndef CHESSENGINEC_ZOBRIST_H
#define CHESSENGINEC_ZOBRIST_H
#include "board.h"
extern bitboard piece_keys[64][6][2];
extern bitboard enpassant_keys[64];
extern bitboard castle_keys[4];
extern bitboard side_key;

void init_keys();
bitboard generate_hash(board b);
#endif //CHESSENGINEC_ZOBRIST_H

