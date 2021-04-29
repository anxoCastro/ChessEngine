//
// Created by castro on 13/04/21.
//


#ifndef CHESSENGINEC_ZOBRIST_H
#define CHESSENGINEC_ZOBRIST_H
#include "board.h"
extern unsigned long piece_keys[64][6][2];
extern unsigned long enpassant_keys[64];
extern unsigned long castle_keys[4];
extern unsigned long side_key;

unsigned long init_keys();
unsigned long generate_hash(board b);
#endif //CHESSENGINEC_ZOBRIST_H

