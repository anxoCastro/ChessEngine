//
// Created by anxo on 07/05/20.
//

#include "moves.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>

//Constantes de bitboards equivalentes a partes del tablero utiles para hacer operaciones
const unsigned long row_a = 72340172838076673UL;
const unsigned long row_h = 9259542123273814144UL;
unsigned long black_pieces;
unsigned long white_pieces;
unsigned long any_pieces;

moveList *create_move_list(){
    moveList *list = malloc(sizeof(moveList));
    list->nElements = 0;
    return list;
}

void addElement(moveList *l, move m){
    l->list[l->nElements] = m;
    l->nElements++;
}

void generate_legal_moves(board *b, moveList *mL){
    //Creamos la lista
    mL = create_move_list();

    //Bitboards que representa todas las piezas negras blancas y todas
    black_pieces = b->BP | b->BN | b->BB | b->BR | b->BQ | b->BK;
    white_pieces = b->WP | b->WN | b->WB | b->WR | b->WQ | b->WK;
    any_pieces = black_pieces | white_pieces;

    //White pawns
    //Recorremos todas las posiciones de la bit board
    move m;
    for(unsigned char i = 0; i < 64; i++){
        //Peones blancos
        //Un movimiento hacia delante
        if( (((white_pieces >> i)&1)==1) && (!((any_pieces >> i - 8)&1)==1) ){
            m.from = i;
            m.to = i -8;
            addElement(mL, m);
        }
    }

}
void print_bitboard(unsigned long b) {
    char charBoard[8][8] = {[0 ... 7][0 ... 7] = '-'};
    for (int i = 0; i < 64; i++) {
        if(((b>>i)&1)==1) {
            charBoard[i / 8][i % 8] = '+';
        }
    }
    for (int i = 0; i < 8; i++) {
        //Una fila del tablero
        printf("%c%c%c%c%c%c%c%c\n", charBoard[i][0], charBoard[i][1], charBoard[i][2],
               charBoard[i][3], charBoard[i][4], charBoard[i][5], charBoard[i][6], charBoard[i][7]);
    }
}

//Convertir movimiento(notación en forma de origen + destino)
//Falta promocion
void move_to_string(move *m, char *string){
    //Origen
    string[0] = 'a' + 8 - (m->from % 8);
    string[1]  = '1' + 8 - (m->from / 8);
    //Destino
    string[2] = 'a' + 8 - (m->to % 8);
    string[3] = '1' + 8 - (m->to / 8);
}

void string_to_move(char *string, move *m){
    //Origen
    m->from = ((string[1] - '1') * 8) + (string[0] - 'a');
    //Destino
    m->to = ((string[3] - '1') * 8) + (string[2] - 'a');
}