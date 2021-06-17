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
    unsigned long enpassant_square;

    //Hash key del tablero
    unsigned long hash;

}board;

void initBoard(board *b);
void printBoard(board b);
int floor_log2(unsigned long bitboard);
unsigned char count_bits(unsigned long bitboard);
unsigned char get_ls1b_index(unsigned long bitboard);
void importFEN(char *fen, board *b);
void print_bitboard(unsigned long b);
int piece_square(unsigned char square, board *b);
#endif //CHESSENGINEC_BOARD_H
