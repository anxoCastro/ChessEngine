//
// Created by anxo on 07/05/20.
//

#include "moves.h"
#include "board.h"
#include <stdio.h>
//Constantes de bitboards equivalentes a partes del tablero utiles para hacer operaciones
const unsigned long row_a = 72340172838076673UL;
const unsigned long row_h = 9259542123273814144UL;
//Para realizar pruebas
void print_bitboard(unsigned long b);

//Reserva e inicializa un movimiento
move *generate_move(){

}
//Genera un item para la lista de movimientos
moveItem *generate_move_item(move *m){

}

moveList *generate_legal_moves(board *b, moveList *mL){
    //Creamos la lista
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