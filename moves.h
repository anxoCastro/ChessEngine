//
// Created by anxo on 07/05/20.
#include "board.h"
#include "magic_bitboards.h"
#ifndef CHESSENGINEC_MOVES_H
#define CHESSENGINEC_MOVES_H


//Tamaño lista movimientos
#define SIZE_LIST_MOVE 200
//Tamaño pila de unmake_info
#define UNMAKE_STACK_SIZE 200
typedef struct move{
    //Representa la casilla del(0 al 63) de origen y destino del movimiento
    unsigned char from;
    unsigned char to;
    //Pieza movida
    //0 pawn
    //1 knight
    //2 bishop
    //3 rook
    //4 queen
    //5 king
    //6 Nada
    unsigned piece;
    //Bitboard del peón donde se podrá hacer captura al paso
    unsigned long enpassantsquare;

    //0 si no puede haber captura al paso
    //1 si puede haber captura al paso
    unsigned enpassant;

    //0 no promotion
    //1 promotion to kinght
    //2 promotion to bishop
    //3 promotion to rook
    //4 promotion to queen
    unsigned promotion;

    //0 no enroque
    //1 enroque corto
    //2 enroque largo
    unsigned castling;

    //0 no hubo captura en el movimiento
    //1 hubo captura en el movimiento
    unsigned capture;

}move;


typedef struct moveList{
    move list[SIZE_LIST_MOVE];
    unsigned short nElements;
}moveList;

//Estructura con información para poder deshacer movimiento
typedef struct unmake_info{
    //0 enroque corto blancas
    //1 enroque largo blancas
    //0 enroque corto negras
    //1 enroque largo negras
    unsigned castle[4];
    //Pieza capturada(en función de captura normal o captura al paso)
    //0 pawn
    //1 knight
    //2 bishop
    //3 rook
    //4 queen
    //5 king
    unsigned capture_piece;
    unsigned capture_enpassant;
}unmake_info;

//Pila con los datos para deshacer los movimientos
typedef struct unmake_stack{
    struct unmake_info stack[UNMAKE_STACK_SIZE];
    int nElements;
    }unmake_stack;

moveList *create_move_list();
moveList *generate_legal_moves(board *b, move lastMove);
moveList *generate_black_moves(board *b, move lastMove, moveList *mL);
moveList *generate_white_moves(board *b, move lastMove, moveList *mL);
void move_to_string(move *m, char *string);
int is_attacked(board *b, int square, unsigned side);
void unmake_move(board *b, move m, struct unmake_stack *unmakeStack);
void make_move(board *b, move m, unmake_stack *unmakeStack);
int make_legal_move(board *b, move m, unmake_stack *unmakeStack);
void generate_move_tables();
void push_unmake(unmake_stack *stack, unmake_info unmake);
unmake_info pop_unmake(unmake_stack *stack);
void initMove(move *m);
#endif //CHESSENGINEC_MOVES_H
