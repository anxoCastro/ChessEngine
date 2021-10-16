//
// Created by castro on 13/04/21.
//

#include <stdlib.h>
#include "zobrist.h"
#include "bitops.h"
bitboard piece_keys[64][6][2];
bitboard enpassant_keys[64];
bitboard castle_keys[4];
bitboard side_key;

bitboard random_64bits(){
    return (bitboard)rand() | (bitboard) (rand() << 16 ) |
     ((bitboard)rand() << 32) |  ((bitboard)rand() << 48);
}

void init_keys(){
    //Inicializar claves zobrist
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 6; j++){
            piece_keys[i][j][WHITE] = random_64bits();
            piece_keys[i][j][BLACK] = random_64bits();
        }
        enpassant_keys[i] = random_64bits();
    }
    castle_keys[0] = random_64bits();
    castle_keys[1] = random_64bits();
    castle_keys[2] = random_64bits();
    castle_keys[3] = random_64bits();
    side_key = random_64bits();
}

bitboard generate_hash(board b){
    bitboard hash = 0ULL;
    bitboard aux;
    int square;

    //Peones blancos
    aux = b.WP;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][PAWN][WHITE];
        pop_bit(aux, square);
    }

    //Caballos blancos
    aux = b.WN;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][KNIGHT][WHITE];
        pop_bit(aux, square);
    }

    //Alfiles blancos
    aux = b.WB;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][BISHOP][WHITE];
        pop_bit(aux, square);
    }
    //Torres blancas
    aux = b.WR;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][ROOK][WHITE];
        pop_bit(aux, square);
    }
    //Reinas blancas
    aux = b.WQ;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][QUEEN][WHITE];
        pop_bit(aux, square);
    }
    //Rey blanco
    square = get_ls1b_index(b.WK);
    hash ^= piece_keys[square][KING][WHITE];

    //Peones negros
    aux = b.BP;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][PAWN][BLACK];
        pop_bit(aux, square);
    }

    //Caballos negros
    aux = b.BN;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][KNIGHT][BLACK];
        pop_bit(aux, square);
    }

    //Alfiles negros
    aux = b.BB;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][BISHOP][BLACK];
        pop_bit(aux, square);
    }
    //Torres negras
    aux = b.BR;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][ROOK][BLACK];
        pop_bit(aux, square);
    }
    //Reinas negras
    aux = b.BQ;
    while(aux){
        square = get_ls1b_index(aux);
        hash ^= piece_keys[square][QUEEN][BLACK];
        pop_bit(aux, square);
    }
    //Rey negro
    square = get_ls1b_index(b.BK);
    hash ^= piece_keys[square][KING][BLACK];

    //Captura al paso
    if(b.enpassant_square){
        hash ^= enpassant_keys[get_ls1b_index(b.enpassant_square)];
    }

    //Enroques
    if(b.castle[0]){
        hash ^= castle_keys[0];
    }
    if(b.castle[1]){
        hash ^= castle_keys[1];
    }
    if(b.castle[2]){
        hash ^= castle_keys[2];
    }
    if(b.castle[3]){
        hash ^= castle_keys[3];
    }

    if(b.side == BLACK){
        hash ^= side_key;
    }
    return hash;
}
