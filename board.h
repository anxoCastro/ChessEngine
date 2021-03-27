//
// Created by anxo on 06/05/20.
//
#ifndef CHESSENGINEC_BOARD_H
#define CHESSENGINEC_BOARD_H


#define BLACK 1
#define WHITE 0
#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5
typedef struct{
    //White Pawn
    unsigned long WP;
    //White Knight;
    unsigned long WN;
    //White bishop
    unsigned long WB;
    //White rook
    unsigned long WR;
    //White queen
    unsigned  long WQ;
    //White king
    unsigned long WK;
    //Black Pawn
    unsigned long BP;
    //Black Knight;
    unsigned long BN;
    //Black bishop
    unsigned long BB;
    //Black rook
    unsigned long BR;
    //Black queen
    unsigned  long BQ;
    //Black king
    unsigned long BK;

    //Turno
    unsigned side;

    //Posibilidades de enroque
    //Bitfield : WKS,WQL,BKS,BQL in that order bits 0-3//
    unsigned castle;

}board;

void initBoard(board *b);
void printBoard(board b);
int floor_log2(unsigned long bitboard);
unsigned char count_bits(unsigned long bitboard);
unsigned char get_ls1b_index(unsigned long bitboard);
#endif //CHESSENGINEC_BOARD_H
