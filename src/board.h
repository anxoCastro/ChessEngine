#ifndef CHESSENGINEC_BOARD_H
#define CHESSENGINEC_BOARD_H

#include "bitops.h"

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
    bitboard WP;
    //White Knight;
    bitboard WN;
    //White bishop
    bitboard WB;
    //White rook
    bitboard WR;
    //White queen
    bitboard WQ;
    //White king
    bitboard WK;
    //Black Pawn
    bitboard BP;
    //Black Knight;
    bitboard BN;
    //Black bishop
    bitboard BB;
    //Black rook
    bitboard BR;
    //Black queen
    bitboard BQ;
    //Black king
    bitboard BK;

    unsigned black_pieces;
    unsigned white_pieces;
    unsigned any_pieces;
    //Turno
    unsigned side;
    //Posibilidades de enroque
    //0 - Enroque corto blanco
    //1 - Enroque largo blanco
    //2 - Enroque corto negro
    //3 - Enroque largo negro
    unsigned castle[4];

    //Casila de captura al paso(quizás haya que cambiar struct move)
    bitboard enpassant_square;

    //Hash key del tablero
    bitboard hash;

}board;
void initBoard(board *b);
void printBoard(board b);
unsigned char count_bits(bitboard bitboard);
unsigned char get_ls1b_index(bitboard bitboard);
void importFEN(char *fen, board *b);
void print_bitboard(bitboard b);
int piece_square(unsigned char square, board *b);
#endif //CHESSENGINEC_BOARD_H
