#include <stdio.h>
#include "board.h"

unsigned long char_to_bitboard(int chess_box);
void initialize_empty_board(board *b){
    b->WP = 0;
    b->WR = 0;
    b->WB = 0;
    b->WN = 0;
    b->WK = 0;
    b->WQ = 0;
    b->BP = 0;
    b->BR = 0;
    b->BB = 0;
    b->BN = 0;
    b->BK = 0;
    b->BQ = 0;
}
void initBoard(board *b) {
    initialize_empty_board(b);
    char board_values[8][8] = {
            {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
            {'-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-'},
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };
    //Recorrer tablero
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            switch (board_values[i][j]) {
                //Para cada tipo de pieza guardamos en su bitboard correspondiente
                case 'P':
                    b->WP += char_to_bitboard((i * 8) + j);
                    break;
                case 'N':
                    b->WN += char_to_bitboard((i * 8) + j);
                    break;
                case 'B':
                    b->WB += char_to_bitboard((i * 8) + j);
                    break;
                case 'R':
                    b->WR += char_to_bitboard((i * 8) + j);
                    break;
                case 'Q':
                    b->WQ += char_to_bitboard((i * 8) + j);
                    break;
                case 'K':
                    b->WK += char_to_bitboard((i * 8) + j);
                    break;
                case 'p':
                    b->BP += char_to_bitboard((i * 8) + j);
                    break;
                case 'n':
                    b->BN += char_to_bitboard((i * 8) + j);
                    break;
                case 'b':
                    b->BB += char_to_bitboard((i * 8) + j);
                    break;
                case 'r':
                    b->BR += char_to_bitboard((i * 8) + j);
                    break;
                case 'q':
                    b->BQ += char_to_bitboard((i * 8) + j);
                    break;
                case 'k':
                    b->BK += char_to_bitboard((i * 8) + j);
                    break;


            }
        }
    };
}

//Escribe un 1 posición indicada(la primera es el 0)
unsigned long char_to_bitboard(int chess_box) {
    unsigned long value = 0b1;
    //Corremos la cifra hasta el valor adecuado
    value = value << chess_box;
    return value;
}

//Para ver que funciona bien
void printBoard(board b) {
    char charBoard[8][8] = {[0 ... 7][0 ... 7] = '-'};
    //printf("%lu\n", b.WP);
    for (int i = 0; i < 64; i++) {
        if (((b.WP >> i) & 1) == 1) {
            charBoard[i / 8][i % 8] = 'P';
        }
        if(((b.WN>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'N';
        }
        if(((b.WB>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'B';
        }
        if(((b.WR>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'R';
        }
        if(((b.WQ>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'Q';
        }
        if(((b.WK>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'K';
        }
        if(((b.BP>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'p';
        }
        if(((b.BN>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'n';
        }
        if(((b.BB>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'b';
        }
        if(((b.BR>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'r';
        }
        if(((b.BQ>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'q';
        }
        if(((b.BK>>i)&1)==1){
            charBoard[i / 8][i % 8] = 'k';
        }
    }
    for (int i = 0; i < 8; i++) {
        //Una fila del tablero
        printf("%c%c%c%c%c%c%c%c\n", charBoard[i][0], charBoard[i][1], charBoard[i][2],
               charBoard[i][3], charBoard[i][4], charBoard[i][5], charBoard[i][6], charBoard[i][7]);
    }
}
