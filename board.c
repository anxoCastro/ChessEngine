//12 bitboards board representation
typedef struct board{
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
}Chess_board;

unsigned  long char_to_bitboard(int chess_box);
void initBoard(Chess_board *board){
    char board_values[8][8]={
            {'r','n','b','q','k','b','n','r'},
            {'p','p','p','p','p','p','p','p'},
            {'-','-','-','-','-','-','-','-'},
            {'-','-','-','-','-','-','-','-'},
            {'-','-','-','-','-','-','-','-'},
            {'-','-','-','-','-','-','-','-'},
            {'P','P','P','P','P','P','P','P'},
            {'R','N','B','Q','K','B','N','R'}
    };
    //Recorrer tablero
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            switch(board_values[i][j]){
                //Para cada tipo de pieza guardamos en su bitboard correspondiente
                case 'P':
                    board->WP += char_to_bitboard((i*8) + j);
                    break;
                case'N':
                    board->WN += char_to_bitboard((i*8) + j);
                    break;
                case 'B':
                    board->WB += char_to_bitboard((i*8) + j);
                    break;
                case'R':
                    board->WR += char_to_bitboard((i*8) + j);
                    break;
                case 'Q':
                    board->WQ += char_to_bitboard((i*8) + j);
                    break;
                case'K':
                    board->WK += char_to_bitboard((i*8) + j);
                    break;
                case 'p':
                    board->BP += char_to_bitboard((i*8) + j);
                    break;
                case'n':
                    board->BN += char_to_bitboard((i*8) + j);
                    break;
                case 'b':
                    board->BB += char_to_bitboard((i*8) + j);
                    break;
                case'r':
                    board->BR += char_to_bitboard((i*8) + j);
                    break;
                case 'q':
                    board->BQ += char_to_bitboard((i*8) + j);
                    break;
                case'k':
                    board->BK += char_to_bitboard((i*8) + j);
                    break;



            }
        }
    };
}


//Escribe un 1 posición indicada(la primera es el 0)
unsigned long char_to_bitboard(int chess_box) {
    unsigned long value = 0b1;
    //Corremos la cifra hasta el valor adecuado
    value = value << (64 -chess_box);
    return value;
}
