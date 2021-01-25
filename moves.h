//
// Created by anxo on 07/05/20.
//
//http://cinnamonchess.altervista.org/bitboard_calculator/Calc.html
#include "board.h"
#ifndef CHESSENGINEC_MOVES_H
#define CHESSENGINEC_MOVES_H
//Tamaño lista movimientos
//Después de buscar un poco he encontrado alguna posición de más de 200 movimientos en un turno
//Pero tienen que estar coronados prácticamente todas las reinas
#define SIZE_LIST_MOVE 200
typedef struct move{
    //
    //
    //Falta implementar promocion
    //Representa la casilla del(0 al 63) de origen y destino del movimiento
    unsigned char from;
    unsigned char to;
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
moveList *generate_legal_moves(board *b);
void move_to_string(move *m, char *string);
moveList *create_move_list();
#endif //CHESSENGINEC_MOVES_H
