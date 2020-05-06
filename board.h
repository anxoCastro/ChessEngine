//
// Created by anxo on 06/05/20.
//

#ifndef CHESSENGINEC_BOARD_H
#define CHESSENGINEC_BOARD_H
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
}board;

void initBoard(board *b);
void printBoard(board b);

#endif //CHESSENGINEC_BOARD_H
