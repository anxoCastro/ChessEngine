//
// Created by anxo on 07/05/20.
//
//http://cinnamonchess.altervista.org/bitboard_calculator/Calc.html
#ifndef CHESSENGINEC_MOVES_H
#define CHESSENGINEC_MOVES_H
typedef struct move{
    //
    //
    //Falta implementar promocion
    //Representa la casilla del(0 al 63) de origen y destino del movimiento
    unsigned char from;
    unsigned char to;
}move;

//Quizá una lista estática sea mejor para hacer menos asignaciones de memoria(cuanto movimientos puede tener una jugada de ajedrez)?
typedef struct moveItem{
    move *item;
    struct moveItem *siguiente;
}moveItem;

typedef struct moveList{
    moveItem *primero;
    moveItem *ultimo;
}moveList;

//Constantes de bitboards equivalentes a partes del tablero utiles para hacer operaciones
extern const unsigned long row_a;
extern const unsigned long row_h;
void print_bitboard(unsigned long b);
#endif //CHESSENGINEC_MOVES_H
