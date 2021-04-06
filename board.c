#include <stdio.h>
#include "board.h"
#include "bitops.h"

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

//Escribe un 1 posición indicada(la primera es el 0)
unsigned long char_to_bitboard(int chess_box) {
    unsigned long value = 0b1;
    //Corremos la cifra hasta el valor adecuado
    value = value << chess_box;
    return value;
}

//Inicia el tablero
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
    //Empiezan las blancas
    b->side = 0;
    //Inicializar enroques
    b->castle[0] = 1;
    b->castle[1] = 1;
    b->castle[2] = 1;
    b->castle[3] = 1;
}


//Imprime tablero
void printBoard(board b) {
    char charBoard[8][8] = {[0 ... 7][0 ... 7] = '-'};
    for (int i = 0; i < 64; i++) {
        if((b.WP>>i) &1) charBoard[i / 8][i % 8] = 'P';
        if((b.WN>>i) &1) charBoard[i / 8][i % 8] = 'N';
        if((b.WB>>i) &1) charBoard[i / 8][i % 8] = 'B';
        if((b.WR>>i) &1) charBoard[i / 8][i % 8] = 'R';
        if((b.WQ>>i) &1) charBoard[i / 8][i % 8] = 'Q';
        if((b.WK>>i) &1) charBoard[i / 8][i % 8] = 'K';
        if((b.BP>>i) &1) charBoard[i / 8][i % 8] = 'p';
        if((b.BN>>i) &1) charBoard[i / 8][i % 8] = 'n';
        if((b.BB>>i) &1) charBoard[i / 8][i % 8] = 'b';
        if((b.BR>>i) &1) charBoard[i / 8][i % 8] = 'r';
        if((b.BQ>>i) &1) charBoard[i / 8][i % 8] = 'q';
        if((b.BK>>i) &1) charBoard[i / 8][i % 8] = 'k';
    }
    for (int i = 0; i < 8; i++) {
        //Una fila del tablero
        printf("        %i  %c %c %c %c %c %c %c %c\n",8 - i, charBoard[i][0], charBoard[i][1], charBoard[i][2],
               charBoard[i][3], charBoard[i][4], charBoard[i][5], charBoard[i][6], charBoard[i][7]);
    }
    printf("\n           A B C D E F G H\n");
}

void importFEN(char *fen, board *b){
    //Dejar valores a cero por defecto
    b->side = WHITE;
    b->castle[0] = 0;
    b->castle[1] = 0;
    b->castle[2] = 0;
    b->castle[3] = 0;
    b->WB = 0;
    b->enpassant_square = 0;
    //Recorrer tablero
    int offset;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int square = i * 8 + j;
            switch (*fen) {
                case 'P':
                    set_bit(b->WP, square);
                    break;
                case 'N':
                    set_bit(b->WN, square);
                    break;
                case 'B':
                    set_bit(b->WB, square);
                    break;
                case 'R':
                    set_bit(b->WR, square);
                    break;
                case 'Q':
                    set_bit(b->WQ, square);
                    break;
                case 'K':
                    set_bit(b->WK, square);
                    break;
                case 'p':
                    set_bit(b->BP, square);
                    break;
                case 'n':
                    set_bit(b->BN, square);
                    break;
                case 'b':
                    set_bit(b->BB, square);
                    break;
                case 'r':
                    set_bit(b->BR, square);
                    break;
                case 'q':
                    set_bit(b->BQ, square);
                    break;
                case 'k':
                    set_bit(b->BK, square);
                    break;
                case '1' ... '8':
                    offset = *fen - '0';
                    j += offset - 1;
                    if(j>=8){
                        j = j - 9;
                        i++;
                    }
                    break;
                case '/':
                    j--;
                    if(j == -1){
                        i--;
                        j = 7;
                    }
                    break;

            }
            *fen++;
        }
    }
    //Espacio en blanco del lado que juega
    *fen++;
    //Marcar lado que juega
    if(*fen == 'w') b->side = WHITE;
    else b->side = BLACK;
    //Saltar a enroques
    *fen++;
    *fen++;
    //Setear enroques
    while(*fen != ' '){
        switch(*fen){
            case 'K':
                b->castle[0] = 1;
                break;
            case 'Q':
                b->castle[1] = 1;
                break;
            case 'k':
                b->castle[2] = 1;
                break;
            case 'q':
                b->castle[3] = 1;
                break;
            case '-':
                break;
        }
        *fen++;

    }
    *fen++;
    //Captura al paso
    if(*fen != '-'){
        int i = fen[0] - 'a';
        int j = 8 - (fen[1] - '0');
        int square = j * 8 + i;
        if(b->side == WHITE){
            square+=8;
        }else{
            square-=8;
        }
        b->enpassant_square  = 1UL << square;
    }
}

void print_bitboard(unsigned long b) {
    //Convertir bitboard a caracteres
    char charBoard[8][8] = {[0 ... 7][0 ... 7] = '-'};
    for (int i = 0; i < 64; i++) {
        if(((b>>i)&1)==1) {
            charBoard[i / 8][i % 8] = '+';
        }

    }
    //Imprimir tablero
    for (int i = 0; i < 8; i++) {
        //Una fila del tablero
        printf("%c%c%c%c%c%c%c%c\n", charBoard[i][0], charBoard[i][1], charBoard[i][2],
               charBoard[i][3], charBoard[i][4], charBoard[i][5], charBoard[i][6], charBoard[i][7]);
    }
}