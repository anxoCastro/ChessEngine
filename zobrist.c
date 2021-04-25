//
// Created by castro on 13/04/21.
//

#include <stdlib.h>
#include "zobrist.h"
#include "board.h"
extern board b;
unsigned long rand_uint64() {
    unsigned long r = 0UL;
    for (int i=0; i<64; i += 15 /*30*/) {
        r = r*((unsigned long)RAND_MAX + 1) + rand();
    }
    return r;
}
void initialize_zobrist_keys(){
    for(int i = 0; i <  64; i++){
        for(int j = 0; j < 6; j++){
            //Piece keys
            piece_keys[i][j][BLACK] = rand_uint64();
            piece_keys[i][j][WHITE] = rand_uint64();
        }
        //Enpassant keys
        enpassant_keys[i] = rand_uint64();
    }

    //Castling keys
    for(int i = 0; i < 16; i++){
        castling_keys[i] = rand_uint64();
    }

    side_key = rand_uint64();
}

unsigned long generate_hash_key(){
    unsigned long key = 0UL;
}