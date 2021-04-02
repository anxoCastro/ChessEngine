#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "moves.h"
int main() {
    //Generar un tablero con la partida empezada e imprimirlo
    board board;
    //Lista de movimientos de la partida
    //El primer elemento es un movimiento nulo, se empieza a contar en uno
    moveList *movelist = create_move_list();
    initBoard(&board);
    printBoard(board);
    generate_move_tables();
    moveList *m = generate_white_moves(&board, movelist->list[movelist->nElements]);
    char* string = malloc(sizeof(char) * 4);
    move_to_string(&m->list[0], string);    
    printf("Movimiento blancas: %s", string);
    for(int i = 1; i < m->nElements; i++){
      move_to_string(&m->list[i], string);        
      printf(", %s", string);
    }
    printf(".\n");
    m = generate_black_moves(&board, movelist->list[movelist->nElements]);
    move_to_string(&m->list[0], string);
    printf("Movimiento negras: %s", string);
    for(int i = 1; i < m->nElements; i++){
      move_to_string(&m->list[i], string);
      printf(" %s", string);
        if(m->list[i].enpassantsquare){
            //printf(" Castling: %lu\n", m->list[i].enpassantsquare);
        }
    }
    printf(".\n");
    return 0;
}
