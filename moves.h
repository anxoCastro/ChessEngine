//
// Created by anxo on 07/05/20.
#include "board.h"
#ifndef CHESSENGINEC_MOVES_H
#define CHESSENGINEC_MOVES_H
//Tamaño lista movimientos
//Después de buscar un poco he encontrado alguna posición de más de 200 movimientos en un turno
//Pero tienen que estar coronados prácticamente todas los peones
#define SIZE_LIST_MOVE 200

//Operaciones de bit sobre el bitboard
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)
typedef struct move{
    //
    //
    //Falta implementar promocion, ,captura, posibilidad de captura al paso...
    //Representa la casilla del(0 al 63) de origen y destino del movimiento
    unsigned char from;
    unsigned char to;
    //Bitboard del peón donde se podrá hacer captura al paso
    unsigned long castlingsquare;
    //Si se realiza captura al paso
    //0 no
    //1 si
    unsigned castling;

}move;


typedef struct moveList{
    move list[SIZE_LIST_MOVE];
    unsigned short nElements;
}moveList;

//Constantes de bitboards equivalentes a partes del tablero utiles para hacer operaciones
extern const unsigned long column_a;
extern const unsigned long column_h;
extern const unsigned long row_2;
extern const unsigned long row_7;


void print_bitboard(unsigned long b);
moveList *create_move_list();
moveList *generate_legal_moves(board *b, move lastMove);
moveList *generate_black_moves(board *b, move lastMove);
moveList *generate_white_moves(board *b, move lastMove);
void move_to_string(move *m, char *string);
void generate_move_tables();
moveList *create_move_list();
#endif //CHESSENGINEC_MOVES_H
